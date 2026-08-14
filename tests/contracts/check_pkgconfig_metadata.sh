#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
build=$1
pc=$build/meson-private/libpkgstate-apply.pc
[ -s "$pc" ] || { echo "pkgconfig-metadata: missing $pc" >&2; exit 1; }
grep -F 'Version: 3.1.1' "$pc" >/dev/null || { echo "pkgconfig-metadata: missing version 3.1.1" >&2; exit 1; }
grep -F -- '-lpkgstate-apply' "$pc" >/dev/null
public=$(sed -n 's/^Requires:[[:space:]]*//p' "$pc")
private=$(sed -n 's/^Requires\.private:[[:space:]]*//p' "$pc")
private_libs=$(sed -n 's/^Libs\.private:[[:space:]]*//p' "$pc")
has_requirement() {
  printf '%s\n' "$1" | tr ',' '\n' | awk \
    -v package="$2" -v version="$3" '
      $1 == package && $2 == ">=" && $3 == version { found = 1 }
      END { exit found ? 0 : 1 }
    '
}

has_requirement "$public" libpkgstate 3.0.0 || { echo 'pkgconfig-metadata: missing public libpkgstate >= 3.0.0' >&2; exit 1; }
has_requirement "$public" libpkgapply 3.0.1 || { echo 'pkgconfig-metadata: missing public libpkgapply >= 3.0.1' >&2; exit 1; }
for leaked in libpkgstate-build libpkgstate-plan libpkgplan libcrypto; do
  if printf '%s\n' "$public" | grep -F "$leaked" >/dev/null; then echo "pkgconfig-metadata: private edge leaked publicly: $leaked" >&2; exit 1; fi
done
has_requirement "$private" libpkgstate-build 3.1.0 || { echo 'pkgconfig-metadata: missing private libpkgstate-build >= 3.1.0' >&2; exit 1; }
has_requirement "$private" libpkgstate-plan 3.0.0 || { echo 'pkgconfig-metadata: missing private libpkgstate-plan >= 3.0.0' >&2; exit 1; }
has_requirement "$private" libpkgplan 0.3.0 || { echo 'pkgconfig-metadata: missing private libpkgplan >= 0.3.0' >&2; exit 1; }
printf '%s\n%s\n' "$private" "$private_libs" | grep -F 'libcrypto' >/dev/null || { echo 'pkgconfig-metadata: missing private libcrypto' >&2; exit 1; }
