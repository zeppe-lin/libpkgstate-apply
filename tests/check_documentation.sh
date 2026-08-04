#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "documentation-contract: $*" >&2; exit 1; }
for file in \
  README.md HISTORY.md CONTRIBUTING.md MAINTAINING.md Doxyfile \
  docs/architecture.md docs/integration.md docs/testing.md docs/abi.md \
  docs/code-style.md docs/meson.build man/libpkgstate-apply.3.scdoc; do
  [ -s "$root/$file" ] || fail "missing $file"
done
python3 "$root/tools/check-public-documentation.py" \
  "$root" libpkgstate-apply libpkgstate-apply.h

python3 "$root/tools/check-man-markdown.py" \
  --root "$root" --project libpkgstate-apply --version 3.0.0
python3 "$root/tools/check-html-manifest.py" \
  --root "$root" --project libpkgstate-apply
