# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
build=$1
pc=$build/meson-private/libpkgstate-apply.pc
[ -s "$pc" ] || { echo "missing $pc" >&2; exit 1; }
grep -F 'Version: 3.0.0' "$pc" >/dev/null
grep -F -- '-lpkgstate-apply' "$pc" >/dev/null
grep -F 'libpkgstate >=3.0.0' "$pc" >/dev/null || { echo 'missing libpkgstate >=3.0.0 metadata' >&2; exit 1; }
grep -F 'libpkgapply >=2.3.0' "$pc" >/dev/null || { echo 'missing libpkgapply >=2.3.0 metadata' >&2; exit 1; }
grep -F 'libpkgstate-source >=3.0.0' "$pc" >/dev/null || { echo 'missing libpkgstate-source >=3.0.0 metadata' >&2; exit 1; }
grep -F 'libpkgstate-build >=3.0.0' "$pc" >/dev/null || { echo 'missing libpkgstate-build >=3.0.0 metadata' >&2; exit 1; }
grep -F 'libpkgplan >=0.3.0' "$pc" >/dev/null || { echo 'missing libpkgplan >=0.3.0 metadata' >&2; exit 1; }
