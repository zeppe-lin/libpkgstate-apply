# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "repository-contract: $*" >&2; exit 1; }
for f in README.md HISTORY.md CONTRIBUTING.md MAINTAINING.md Doxyfile docs/architecture.md docs/integration.md docs/testing.md docs/history/libpkgstate-2.5.1-origin.sha256 .clang-format .editorconfig; do [ -s "$root/$f" ] || fail "missing $f"; done
for s in "$root"/ci/*.sh "$root"/tests/*.sh; do sh -n "$s" || fail "invalid shell: ${s#$root/}"; done
