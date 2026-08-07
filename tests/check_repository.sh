# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "repository-contract: $*" >&2; exit 1; }
for f in README.md HISTORY.md CONTRIBUTING.md MAINTAINING.md Doxyfile docs/architecture.md docs/integration.md docs/testing.md docs/abi.md docs/code-style.md docs/meson.build docs/history/libpkgstate-2.5.1-origin.sha256 .clang-format .editorconfig man/libpkgstate-apply.3.scdoc; do [ -s "$root/$f" ] || fail "missing $f"; done
for s in "$root"/ci/*.sh "$root"/tests/*.sh; do sh -n "$s" || fail "invalid shell: ${s#$root/}"; done
for file in abi/libpkgstate-apply.exports include/libpkgstate-apply/export.h tools/generate-elf-export-script.sh ci/qualify-installed.sh ci/installed-apply-consumer.cpp; do [ -s "$root/$file" ] || fail "missing $file"; done
test "$(grep -c '^## 3.0.0' "$root/HISTORY.md")" -eq 1 || fail '3.0.0 history heading is duplicated'
for file in tests/check_style.sh ci/lint-manpage.sh ci/build-dependencies.sh ci/audit-shared-boundary.sh; do [ -x "$root/$file" ] || fail "missing executable $file"; done

[ ! -e "$root/man/pkgstate_apply_adapter.3.scdoc" ] || fail "obsolete in-tree manual retained: pkgstate_apply_adapter.3.scdoc"

test -x "$root/tools/check-public-documentation.py" || fail 'public documentation checker is absent'
test -x "$root/tools/check-doxygen-contract.py" || fail 'Doxygen contract checker is absent'

grep -F -- '--include-root' "$root/tests/check_documentation.sh" >/dev/null ||
  fail 'documentation parser dependency binding is absent'
grep -F "pkgconfig: 'includedir'" "$root/tests/meson.build" >/dev/null ||
  fail 'Meson documentation dependency binding is absent'

for tool in \
  build-html-docs.py check-html-docs.py install-html-docs.py \
  render-man-markdown.py check-man-markdown.py check-html-manifest.py; do
  test -x "$root/tools/$tool" || fail "missing executable tools/$tool"
done

for helper in \
  ci/qualify-html-docs.sh ci/qualify-installed-documentation.py; do
  test -x "$root/$helper" || fail "missing executable $helper"
done
