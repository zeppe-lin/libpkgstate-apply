# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
manifest=$root/docs/history/libpkgstate-2.5.1-origin.sha256
cd "$root"
sha256sum --check "$manifest" >/dev/null || { echo 'extracted implementation drifted from libpkgstate 2.5.1' >&2; exit 1; }
