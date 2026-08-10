# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "architecture-contract: $*" >&2; exit 1; }
grep -F 'lease-bound canonical state + completed application -> state_publication_request' "$root/docs/architecture.md" >/dev/null || fail 'authority flow is undocumented'
grep -F "gnu_symbol_visibility: 'hidden'" "$root/src/meson.build" >/dev/null || fail 'hidden visibility is not enforced'
test -s "$root/abi/libpkgstate-apply.exports" || fail 'reviewed export manifest is absent'
if grep -F 'libpkgbuild_dep' "$root/src/meson.build" >/dev/null; then fail 'redundant production libpkgbuild dependency retained'; fi
grep -F "'libpkgstate-plan'" "$root/meson.build" >/dev/null || fail 'state-to-planner translation edge is not declared'
grep -F 'plan_adapter::project_candidate_control' "$root/src/apply_adapter.cpp" >/dev/null || fail 'durable candidate control is recreated locally'
if grep -F 'source_adapter::project_source' "$root/src/apply_adapter.cpp" >/dev/null; then fail 'source admission is duplicated outside libpkgstate-build'; fi
if grep -R -E 'libpkgapply-posix|pkgapply::posix' "$root/include" "$root/src" "$root/meson.build" >/dev/null; then fail 'application mechanism provider contaminated semantic state admission'; fi
grep -F 'It does not depend
on' "$root/docs/architecture.md" >/dev/null || fail 'mechanism non-dependency is undocumented'
grep -F '`libpkgapply-posix`' "$root/docs/architecture.md" >/dev/null || fail 'mechanism provider name is undocumented'
if grep -R -F 'catch (const std::exception' "$root/src" >/dev/null; then fail 'adapter launders unrelated process failures through std::exception'; fi
