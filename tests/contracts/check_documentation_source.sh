#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "documentation-source-contract: $*" >&2; exit 1; }

for file in \
  README.md HISTORY.md CONTRIBUTING.md MAINTAINING.md \
  docs/architecture.md docs/integration.md docs/testing.md docs/abi.md \
  docs/code-style.md docs/html.md docs/manpage-markdown.md \
  docs/history/3.0-extraction.md tests/fixtures/README.md; do
  [ -s "$root/$file" ] || fail "missing $file"
  first=$(sed -n '/[^[:space:]]/{p;q;}' "$root/$file")
  case "$first" in
    '# '*) ;;
    *) fail "$file does not begin with an ATX level-one heading" ;;
  esac
  if grep -n -E '^(=+|-+|~+)$' "$root/$file" >/dev/null; then
    fail "$file retains Setext/underline Markdown heading syntax"
  fi
done
