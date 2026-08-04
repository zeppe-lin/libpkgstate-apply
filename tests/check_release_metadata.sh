# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "release-metadata: $*" >&2; exit 1; }
grep -F "version: '3.0.0'" "$root/meson.build" >/dev/null || fail 'version is not 3.0.0'
grep -F "soversion: '3'" "$root/src/meson.build" >/dev/null || fail 'SONAME generation is wrong'
grep -F 'PROJECT_NUMBER         = 3.0.0' "$root/Doxyfile" >/dev/null || fail 'Doxygen version is wrong'
grep -F '## 3.0.0' "$root/HISTORY.md" >/dev/null || fail 'history omits release'
grep -F "'libpkgstate'" "$root/meson.build" >/dev/null || fail 'missing dependency libpkgstate'
grep -F "version: '>=3.0.0'" "$root/meson.build" >/dev/null || fail 'missing floor libpkgstate >=3.0.0'
grep -F "'libpkgapply'" "$root/meson.build" >/dev/null || fail 'missing dependency libpkgapply'
grep -F "version: '>=3.0.0'" "$root/meson.build" >/dev/null || fail 'missing floor libpkgapply >=3.0.0'
grep -F "'libpkgstate-build'" "$root/meson.build" >/dev/null || fail 'missing dependency libpkgstate-build'
grep -F "version: '>=3.0.0'" "$root/meson.build" >/dev/null || fail 'missing floor libpkgstate-build >=3.0.0'
grep -F "'libpkgstate-plan'" "$root/meson.build" >/dev/null || fail 'missing dependency libpkgstate-plan'
grep -F "version: '>=3.0.0'" "$root/meson.build" >/dev/null || fail 'missing floor libpkgstate-plan >=3.0.0'
grep -F "'libpkgplan'" "$root/meson.build" >/dev/null || fail 'missing dependency libpkgplan'
grep -F "version: '>=0.3.0'" "$root/meson.build" >/dev/null || fail 'missing floor libpkgplan >=0.3.0'
grep -F "'libcrypto'" "$root/meson.build" >/dev/null || fail 'missing dependency libcrypto'
grep -F "soversion: '3'" "$root/src/meson.build" >/dev/null || fail 'SONAME generation changed'
test -s "$root/abi/libpkgstate-apply.exports" || fail 'ABI manifest is absent'
if grep -F "'libpkgstate-source'" "$root/src/meson.build" >/dev/null; then fail 'production closure retains libpkgstate-source'; fi
if grep -F "'libpkgbuild'" "$root/src/meson.build" >/dev/null; then fail 'production closure retains libpkgbuild'; fi
