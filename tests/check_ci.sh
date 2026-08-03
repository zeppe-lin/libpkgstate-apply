#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "ci-contract: $*" >&2; exit 1; }
workflow=$root/.github/workflows/ci.yml
[ -s "$workflow" ] || fail 'workflow is absent'
for script in ci/configure-and-test.sh ci/build-dependencies.sh ci/qualify-installed.sh ci/audit-shared-boundary.sh; do
  [ -x "$root/$script" ] || fail "missing executable $script"
  sh -n "$root/$script" || fail "invalid shell: $script"
done
for token in 'GCC shared' 'GCC static' 'Clang shared' 'Clang static' 'GCC release' 'address,undefined' 'meson==1.6.1' '--wrap-mode=nofallback'; do
  grep -F -- "$token" "$workflow" "$root/ci/configure-and-test.sh" "$root/ci/build-dependencies.sh" >/dev/null || fail "missing $token"
done
grep -F -- 'repository: zeppe-lin/libpkgapply' "$workflow" >/dev/null || fail 'missing dependency pin: repository: zeppe-lin/libpkgapply'
grep -F -- 'ref: v2.3.0' "$workflow" >/dev/null || fail 'missing dependency pin: ref: v2.3.0'
grep -F -- 'repository: zeppe-lin/libpkgstate-build' "$workflow" >/dev/null || fail 'missing dependency pin: repository: zeppe-lin/libpkgstate-build'
grep -F -- 'repository: zeppe-lin/libpkgsource-plan' "$workflow" >/dev/null || fail 'missing dependency pin: repository: zeppe-lin/libpkgsource-plan'
grep -F -- 'ref: v1.0.0' "$workflow" >/dev/null || fail 'missing dependency pin: ref: v1.0.0'
grep -F 'libpkgsource-plan >=2.0.0' "$root/docs/integration.md" >/dev/null || fail 'external generation gate is undocumented'
