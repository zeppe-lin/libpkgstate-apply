#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
build=$1
pc=$build/meson-private/libpkgstate-apply.pc
[ -s "$pc" ] || { echo "pkgconfig-metadata: missing $pc" >&2; exit 1; }
grep -F 'Version: 3.0.0' "$pc" >/dev/null
grep -F -- '-lpkgstate-apply' "$pc" >/dev/null
public=$(sed -n 's/^Requires:[[:space:]]*//p' "$pc")
private=$(sed -n 's/^Requires\.private:[[:space:]]*//p' "$pc")
private_libs=$(sed -n 's/^Libs\.private:[[:space:]]*//p' "$pc")
printf '%s\n' "$public" | grep -F 'libpkgstate >=3.0.0' >/dev/null || { echo 'pkgconfig-metadata: missing public libpkgstate >=3.0.0' >&2; exit 1; }
printf '%s\n' "$public" | grep -F 'libpkgapply >=2.3.0' >/dev/null || { echo 'pkgconfig-metadata: missing public libpkgapply >=2.3.0' >&2; exit 1; }
if printf '%s\n' "$public" | grep -F 'libpkgstate-source' >/dev/null; then echo 'pkgconfig-metadata: private edge leaked publicly: libpkgstate-source' >&2; exit 1; fi
if printf '%s\n' "$public" | grep -F 'libpkgstate-build' >/dev/null; then echo 'pkgconfig-metadata: private edge leaked publicly: libpkgstate-build' >&2; exit 1; fi
if printf '%s\n' "$public" | grep -F 'libpkgplan' >/dev/null; then echo 'pkgconfig-metadata: private edge leaked publicly: libpkgplan' >&2; exit 1; fi
if printf '%s\n' "$public" | grep -F 'libpkgbuild' >/dev/null; then echo 'pkgconfig-metadata: private edge leaked publicly: libpkgbuild' >&2; exit 1; fi
if printf '%s\n' "$public" | grep -F 'libcrypto' >/dev/null; then echo 'pkgconfig-metadata: private edge leaked publicly: libcrypto' >&2; exit 1; fi
printf '%s\n' "$private" | grep -F 'libpkgstate-source >=3.0.0' >/dev/null || { echo 'pkgconfig-metadata: missing private libpkgstate-source >=3.0.0' >&2; exit 1; }
printf '%s\n' "$private" | grep -F 'libpkgstate-build >=3.0.0' >/dev/null || { echo 'pkgconfig-metadata: missing private libpkgstate-build >=3.0.0' >&2; exit 1; }
printf '%s\n' "$private" | grep -F 'libpkgplan >=0.3.0' >/dev/null || { echo 'pkgconfig-metadata: missing private libpkgplan >=0.3.0' >&2; exit 1; }
printf '%s\n' "$private" | grep -F 'libpkgbuild >=2.0.0' >/dev/null || { echo 'pkgconfig-metadata: missing private libpkgbuild >=2.0.0' >&2; exit 1; }
printf '%s\n%s\n' "$private" "$private_libs" | grep -E 'libcrypto' >/dev/null || { echo 'pkgconfig-metadata: missing private libcrypto' >&2; exit 1; }
