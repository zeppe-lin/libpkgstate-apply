# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "architecture-contract: $*" >&2; exit 1; }
grep -F 'lease-bound canonical state + completed application -> state_publication_request' "$root/docs/architecture.md" >/dev/null || fail 'authority flow is undocumented'
