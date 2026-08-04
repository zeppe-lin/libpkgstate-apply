# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "architecture-contract: $*" >&2; exit 1; }
grep -F 'lease-bound canonical state + completed application -> state_publication_request' "$root/docs/architecture.md" >/dev/null || fail 'authority flow is undocumented'
grep -F "gnu_symbol_visibility: 'hidden'" "$root/src/meson.build" >/dev/null || fail 'hidden visibility is not enforced'
test -s "$root/abi/libpkgstate-apply.exports" || fail 'reviewed export manifest is absent'
grep -F "'libpkgbuild'" "$root/meson.build" >/dev/null || fail 'direct libpkgbuild use is not declared'
grep -F "'libpkgstate-plan'" "$root/meson.build" >/dev/null || fail 'state-to-planner translation edge is not declared'
grep -F 'plan_adapter::project_candidate_control' "$root/src/apply_adapter.cpp" >/dev/null || fail 'durable candidate control is recreated locally'
