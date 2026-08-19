#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
build=$1
pc=$build/meson-private/libpkgstate-apply.pc
[ -s "$pc" ] || { echo "pkgconfig-metadata: missing $pc" >&2; exit 1; }
grep -F 'Version: 3.1.2' "$pc" >/dev/null || { echo "pkgconfig-metadata: missing version 3.1.2" >&2; exit 1; }
grep -F -- '-lpkgstate-apply' "$pc" >/dev/null
public=$(sed -n 's/^Requires:[[:space:]]*//p' "$pc")
private=$(sed -n 's/^Requires\.private:[[:space:]]*//p' "$pc")
private_libs=$(sed -n 's/^Libs\.private:[[:space:]]*//p' "$pc")
normalize_requirements() {
  printf '%s\n' "$1" | tr ',' '\n' | awk '{$1=$1; if (NF) print}'
}
count_requirement() {
  normalize_requirements "$1" | awk \
    -v package="$2" -v operator="$3" -v version="$4" '
      $1 == package && $2 == operator && $3 == version { count += 1 }
      END { print count + 0 }
    '
}
has_requirement() {
  [ "$(count_requirement "$1" "$2" '>=' "$3")" -eq 1 ]
}
public_requirements=$(normalize_requirements "$public")
public_count=$(printf '%s\n' "$public_requirements" | awk 'NF { count += 1 } END { print count + 0 }')
[ "$public_count" -eq 3 ] || { echo 'pkgconfig-metadata: public closure must contain exactly three version clauses' >&2; exit 1; }
[ "$(count_requirement "$public" libpkgstate '>=' 3.0.0)" -eq 1 ] || { echo 'pkgconfig-metadata: expected exactly one public libpkgstate >= 3.0.0' >&2; exit 1; }
[ "$(count_requirement "$public" libpkgapply '>=' 4.0.0)" -eq 1 ] || { echo 'pkgconfig-metadata: expected exactly one public libpkgapply >= 4.0.0' >&2; exit 1; }
[ "$(count_requirement "$public" libpkgapply '<' 5.0.0)" -eq 1 ] || { echo 'pkgconfig-metadata: expected exactly one public libpkgapply < 5.0.0' >&2; exit 1; }
for leaked in libpkgstate-build libpkgstate-plan libpkgplan libcrypto; do
  if printf '%s\n' "$public" | grep -F "$leaked" >/dev/null; then echo "pkgconfig-metadata: private edge leaked publicly: $leaked" >&2; exit 1; fi
done
has_requirement "$private" libpkgstate-build 3.1.0 || { echo 'pkgconfig-metadata: missing private libpkgstate-build >= 3.1.0' >&2; exit 1; }
has_requirement "$private" libpkgstate-plan 3.0.0 || { echo 'pkgconfig-metadata: missing private libpkgstate-plan >= 3.0.0' >&2; exit 1; }
has_requirement "$private" libpkgplan 0.3.0 || { echo 'pkgconfig-metadata: missing private libpkgplan >= 0.3.0' >&2; exit 1; }
printf '%s\n%s\n' "$private" "$private_libs" | grep -F 'libcrypto' >/dev/null || { echo 'pkgconfig-metadata: missing private libcrypto' >&2; exit 1; }
