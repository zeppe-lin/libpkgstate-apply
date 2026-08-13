#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "repository-contract: $*" >&2; exit 1; }

for f in \
  README.md DESIGN.md TESTING.md HISTORY.md CONTRIBUTING.md MAINTAINING.md Doxyfile \
  docs/integration.md docs/abi.md docs/code-style.md docs/meson.build \
  docs/history/libpkgstate-2.5.1-origin.sha256 .clang-format .editorconfig \
  docs/man/libpkgstate-apply.3.md docs/man/generated/libpkgstate-apply.3; do
  [ -s "$root/$f" ] || fail "missing $f"
done
[ -s "$root/meson.options" ] || fail 'canonical meson.options is absent'
[ ! -e "$root/meson_options.txt" ] || fail 'legacy meson_options.txt remains'
[ ! -e "$root/docs/architecture.md" ] || fail 'duplicate docs/architecture.md authority remains'
[ ! -e "$root/docs/testing.md" ] || fail 'duplicate docs/testing.md authority remains'
[ ! -e "$root/man" ] || fail 'legacy root man/ authority remains'
if find "$root" -type f \( -name '*.scd' -o -name '*.scdoc' \) | grep . >/dev/null; then
  fail 'scdoc manual authority remains'
fi
for retired in tools/render-man-markdown.py tools/check-man-markdown.py; do
  [ ! -e "$root/$retired" ] || fail "retired manual mirror tool remains: $retired"
done
for s in "$root"/ci/*.sh "$root"/tests/contracts/*.sh; do
  sh -n "$s" || fail "invalid shell: ${s#$root/}"
done
for file in abi/libpkgstate-apply.exports include/libpkgstate-apply/export.h tools/generate-elf-export-script.sh ci/qualify-installed.sh ci/installed-apply-consumer.cpp; do
  [ -s "$root/$file" ] || fail "missing $file"
done
test "$(grep -c '^## 3.0.0' "$root/HISTORY.md")" -eq 1 || fail '3.0.0 history heading is duplicated'
for file in tests/contracts/check_style.sh ci/lint-manpage.sh ci/build-dependencies.sh ci/audit-shared-boundary.sh; do
  [ -x "$root/$file" ] || fail "missing executable $file"
done
[ -x "$root/tools/check-public-documentation.py" ] || fail 'public documentation checker is absent'
[ -x "$root/tools/check-doxygen-contract.py" ] || fail 'Doxygen contract checker is absent'
grep -F -- '--include-root' "$root/tests/contracts/check_documentation.sh" >/dev/null ||
  fail 'documentation parser dependency binding is absent'
grep -F "pkgconfig: 'includedir'" "$root/tests/meson.build" >/dev/null ||
  fail 'Meson documentation dependency binding is absent'
for tool in \
  build-html-docs.py check-html-docs.py install-html-docs.py check-html-manifest.py \
  update-man-pages.sh canonicalize-man-roff.awk; do
  [ -s "$root/tools/$tool" ] || fail "missing tools/$tool"
done
[ -x "$root/tools/update-man-pages.sh" ] || fail 'update-man-pages.sh is not executable'
for helper in ci/qualify-html-docs.sh ci/qualify-installed-documentation.py; do
  [ -x "$root/$helper" ] || fail "missing executable $helper"
done
if grep -F 'meson_options.txt' "$root/tools/check-html-manifest.py" >/dev/null; then
  fail 'HTML manifest checker retains legacy Meson options fallback'
fi
