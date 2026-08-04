#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
[ "$#" -eq 1 ] || { echo "usage: $0 INSTALLED-LIBRARY" >&2; exit 2; }
library=$1
[ -s "$library" ] || { echo "shared-boundary-audit: missing $library" >&2; exit 1; }
output=$(readelf -d "$library")
printf '%s\n' "$output"
printf '%s\n' "$output" | grep -F 'Library soname: [libpkgstate-apply.so.3]' >/dev/null || { echo 'shared-boundary-audit: wrong SONAME' >&2; exit 1; }
needed=$(printf '%s\n' "$output" | grep 'Shared library:' || true)
for dependency in \
  'libpkgstate.so.4' \
  'libpkgapply.so.2' \
  'libpkgstate-build.so.1' \
  'libpkgstate-plan.so.2' \
  'libpkgplan.so.1'
do
  printf '%s\n' "$needed" | grep -F "Shared library: [$dependency]" >/dev/null || {
    echo "shared-boundary-audit: missing $dependency" >&2
    exit 1
  }
done
printf '%s\n' "$needed" | grep -E 'Shared library: \[libcrypto\.so[^]]*\]' >/dev/null || {
  echo 'shared-boundary-audit: missing libcrypto provider' >&2
  exit 1
}
if printf '%s\n' "$needed" | grep -E 'libpkgstate-source|libpkgbuild|libpkgsource\.so|libpkgimage\.so|libpkgsource-plan|libarchive|libyaml|libpkgapply-posix' >/dev/null; then
  echo 'shared-boundary-audit: redundant or mechanism dependency is direct' >&2
  exit 1
fi
