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
pin_count()
{
  repository=$1
  ref=$2
  awk -v repository="repository: zeppe-lin/$repository" -v ref="ref: $ref" '
    {
      line = $0
      sub(/^[[:space:]]*/, "", line)
      sub(/[[:space:]]*$/, "", line)
    }
    line == repository {
      if ((getline next_line) > 0) {
        sub(/^[[:space:]]*/, "", next_line)
        sub(/[[:space:]]*$/, "", next_line)
        if (next_line == ref)
          count++
      }
    }
    END { print count + 0 }
  ' "$workflow"
}
require_pin()
{
  repository=$1
  ref=$2
  [ "$(pin_count "$repository" "$ref")" -eq 2 ] ||
    fail "dependency pin is not exact in both matrices: $repository $ref"
}
require_pin libpkgsource v4.1.0
require_pin libpkgimage v0.4.1
require_pin libpkgcatalog v4.0.0
require_pin libpkgresolve v4.0.0
require_pin libpkgbuild v3.0.1
require_pin libpkgplan v0.3.1
require_pin libpkgbuild-image v1.0.1
require_pin libpkgsource-plan v2.0.0
require_pin libpkgbuild-plan v1.1.0
require_pin libpkgapply v4.0.0
require_pin libpkgstate v3.1.0
require_pin libpkgstate-source v4.0.0
require_pin libpkgstate-build v3.1.0
require_pin libpkgstate-plan v3.0.0
grep -F 'libpkgstate-build.so.1' "$root/ci/audit-shared-boundary.sh" >/dev/null || fail 'shared audit omits state-build authority'
grep -F 'libpkgstate-plan.so.2' "$root/ci/audit-shared-boundary.sh" >/dev/null || fail 'shared audit omits state-plan translation'
grep -F 'libpkgapply.so.4' "$root/ci/audit-shared-boundary.sh" >/dev/null || fail 'shared audit omits application generation 4'
if grep -F 'libpkgapply.so.2' "$root/ci/audit-shared-boundary.sh" >/dev/null; then fail 'shared audit still names obsolete application generation 2'; fi
if grep -F 'missing libpkgstate-source.so.1' "$root/ci/audit-shared-boundary.sh" >/dev/null; then fail 'shared audit still requires test-only source adapter'; fi
if grep -F 'missing libpkgbuild.so.3' "$root/ci/audit-shared-boundary.sh" >/dev/null; then fail 'shared audit still requires transitive build owner'; fi
grep -F 'libpkgapply-posix' "$root/ci/audit-shared-boundary.sh" >/dev/null || fail 'shared audit does not forbid mechanism-provider coupling'

consumer=$root/ci/installed-apply-consumer.cpp
grep -F 'projection_error publication(' "$consumer" >/dev/null ||
  fail 'installed consumer does not extract completed-publication adapter'
grep -F 'application_state_projection_error state(' "$consumer" >/dev/null ||
  fail 'installed consumer does not extract lease-bound state adapter'
if grep -F '&pkgstate::apply_adapter::' "$consumer" >/dev/null; then
  fail 'installed consumer regressed to address-only linkage'
fi

grep -F 'html: enabled' "$root/.github/workflows/ci.yml" >/dev/null || fail 'GCC shared HTML build is absent'
grep -F 'pandoc' "$root/.github/workflows/ci.yml" >/dev/null || fail 'Pandoc qualification dependency is absent'
grep -F -- '-Dhtml_docs=' "$root/.github/workflows/ci.yml" >/dev/null || fail 'HTML Meson feature is not configured'
grep -F 'qualify-html-docs.sh' "$root/.github/workflows/ci.yml" >/dev/null || fail 'installed HTML qualification is absent'
[ "$(pin_count libpkgresolve v2.0.0)" -eq 0 ] ||
  fail 'retired resolver 2 qualification pin remains'
