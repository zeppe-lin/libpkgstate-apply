#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "documentation-contract: $*" >&2; exit 1; }
state_include=${2:-}
apply_include=${3:-}
state_build_include=${4:-}
state_plan_include=${5:-}
plan_include=${6:-}

resolve_include()
{
  package=$1
  value=$2
  if [ -n "$value" ]; then
    printf '%s\n' "$value"
    return
  fi
  command -v pkg-config >/dev/null 2>&1 ||
    fail "$package include root is unavailable"
  pkg-config --exists "$package" ||
    fail "$package include root is unavailable"
  pkg-config --variable=includedir "$package"
}

state_include=$(resolve_include libpkgstate "$state_include")
apply_include=$(resolve_include libpkgapply "$apply_include")
state_build_include=$(resolve_include libpkgstate-build "$state_build_include")
state_plan_include=$(resolve_include libpkgstate-plan "$state_plan_include")
plan_include=$(resolve_include libpkgplan "$plan_include")
for file in \
  README.md HISTORY.md CONTRIBUTING.md MAINTAINING.md Doxyfile \
  docs/architecture.md docs/integration.md docs/testing.md docs/abi.md \
  docs/code-style.md docs/meson.build man/libpkgstate-apply.3.scdoc; do
  [ -s "$root/$file" ] || fail "missing $file"
done
python3 "$root/tools/check-public-documentation.py" \
  "$root" libpkgstate-apply libpkgstate-apply.h
if command -v clang++ >/dev/null 2>&1; then
  python3 "$root/tools/check-doxygen-contract.py" \
    --root "$root" --include-subdir libpkgstate-apply \
    --include-root "$state_include" \
    --include-root "$apply_include" \
    --include-root "$state_build_include" \
    --include-root "$state_plan_include" \
    --include-root "$plan_include" \
    --namespace pkgstate --clang "$(command -v clang++)"
fi

python3 "$root/tools/check-man-markdown.py" \
  --root "$root" --project libpkgstate-apply --version 3.0.0
python3 "$root/tools/check-html-manifest.py" \
  --root "$root" --project libpkgstate-apply
