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
for token in 'GCC shared' 'GCC static' 'Clang shared' 'Clang static' 'GCC release' 'address,undefined' 'meson==1.10.2' '--wrap-mode=nofallback'; do
  grep -F -- "$token" "$workflow" "$root/ci/configure-and-test.sh" "$root/ci/build-dependencies.sh" >/dev/null || fail "missing $token"
done
grep -F -- 'repository: zeppe-lin/libpkgapply' "$workflow" >/dev/null || fail 'missing dependency pin: repository: zeppe-lin/libpkgapply'
grep -F -- 'ref: v3.0.0' "$workflow" >/dev/null || fail 'missing dependency pin: ref: v3.0.0'
for repository in libpkgcatalog libpkgresolve libpkgbuild-image libpkgbuild-plan; do
  grep -F -- "repository: zeppe-lin/$repository" "$workflow" >/dev/null ||     fail "missing dependency pin: repository: zeppe-lin/$repository"
done
grep -F -- 'repository: zeppe-lin/libpkgstate-build' "$workflow" >/dev/null || fail 'missing dependency pin: repository: zeppe-lin/libpkgstate-build'
grep -F -- 'repository: zeppe-lin/libpkgsource-plan' "$workflow" >/dev/null || fail 'missing dependency pin: repository: zeppe-lin/libpkgsource-plan'
grep -F -- 'ref: v1.0.0' "$workflow" >/dev/null || fail 'missing dependency pin: ref: v1.0.0'
grep -F -- 'repository: zeppe-lin/libpkgstate-plan' "$workflow" >/dev/null || fail 'missing dependency pin: repository: zeppe-lin/libpkgstate-plan'
grep -F 'libpkgstate-build.so.1' "$root/ci/audit-shared-boundary.sh" >/dev/null || fail 'shared audit omits state-build authority'
grep -F 'libpkgstate-plan.so.2' "$root/ci/audit-shared-boundary.sh" >/dev/null || fail 'shared audit omits state-plan translation'
if grep -F 'missing libpkgstate-source.so.1' "$root/ci/audit-shared-boundary.sh" >/dev/null; then fail 'shared audit still requires test-only source adapter'; fi
if grep -F 'missing libpkgbuild.so.3' "$root/ci/audit-shared-boundary.sh" >/dev/null; then fail 'shared audit still requires transitive build owner'; fi
grep -F 'libpkgapply-posix' "$root/ci/audit-shared-boundary.sh" >/dev/null || fail 'shared audit does not forbid mechanism-provider coupling'

grep -F 'html: enabled' "$root/.github/workflows/ci.yml" >/dev/null || fail 'GCC shared HTML build is absent'
grep -F 'pandoc' "$root/.github/workflows/ci.yml" >/dev/null || fail 'Pandoc qualification dependency is absent'
grep -F -- '-Dhtml_docs=' "$root/.github/workflows/ci.yml" >/dev/null || fail 'HTML Meson feature is not configured'
grep -F 'qualify-html-docs.sh' "$root/.github/workflows/ci.yml" >/dev/null || fail 'installed HTML qualification is absent'
