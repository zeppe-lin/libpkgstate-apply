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
printf '%s\n' "$needed" | grep -F 'Shared library: [libpkgstate.so.3]' >/dev/null || { echo 'shared-boundary-audit: missing libpkgstate.so.3' >&2; exit 1; }
printf '%s\n' "$needed" | grep -F 'Shared library: [libpkgapply.so.2]' >/dev/null || { echo 'shared-boundary-audit: missing libpkgapply.so.2' >&2; exit 1; }
printf '%s\n' "$needed" | grep -F 'Shared library: [libpkgstate-source.so.1]' >/dev/null || { echo 'shared-boundary-audit: missing libpkgstate-source.so.1' >&2; exit 1; }
printf '%s\n' "$needed" | grep -F 'Shared library: [libpkgstate-build.so.1]' >/dev/null || { echo 'shared-boundary-audit: missing libpkgstate-build.so.1' >&2; exit 1; }
printf '%s\n' "$needed" | grep -F 'Shared library: [libpkgplan.so.1]' >/dev/null || { echo 'shared-boundary-audit: missing libpkgplan.so.1' >&2; exit 1; }
printf '%s\n' "$needed" | grep -F 'Shared library: [libpkgbuild.so.3]' >/dev/null || { echo 'shared-boundary-audit: missing libpkgbuild.so.3' >&2; exit 1; }
printf '%s\n' "$needed" | grep -E 'Shared library: \[libcrypto\.so[^]]*\]' >/dev/null || { echo 'shared-boundary-audit: missing dependency matching libcrypto\.so' >&2; exit 1; }
if printf '%s\n' "$needed" | grep -E 'libpkgsource\.so|libpkgimage\.so|libpkgsource-plan|libpkgstate-plan|libarchive|libyaml' >/dev/null; then echo 'shared-boundary-audit: forbidden direct dependency' >&2; exit 1; fi
