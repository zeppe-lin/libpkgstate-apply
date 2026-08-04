#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

root=$1

if ! git -C "$root" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  echo 'extraction-contract: repository history unavailable; skipping provenance check' >&2
  exit 77
fi

roots=$(git -C "$root" rev-list --max-parents=0 HEAD)
case "$roots" in
  *'
'*)
    echo 'extraction-contract: repository has more than one root commit' >&2
    exit 1
    ;;
  '')
    echo 'extraction-contract: repository has no root commit' >&2
    exit 1
    ;;
esac

python3 - "$root" "$roots" <<'PY_CHECK'
from pathlib import Path
import hashlib
import subprocess
import sys

root = Path(sys.argv[1])
extraction_commit = sys.argv[2]
manifest = root / 'docs/history/libpkgstate-2.5.1-origin.sha256'
macro = 'PKGSTATE_APPLY_API '
export_include = '\n#include <libpkgstate-apply/export.h>\n'

for line in manifest.read_text(encoding='utf-8').splitlines():
    expected, rel = line.split('  ', 1)
    result = subprocess.run(
        ['git', '-C', str(root), 'show', f'{extraction_commit}:{rel}'],
        check=True,
        stdout=subprocess.PIPE,
    )
    data = result.stdout
    if rel.endswith('.h'):
        text = data.decode('utf-8')
        text = text.replace(export_include, '')
        text = text.replace(macro, '')
        data = text.encode('utf-8')
    actual = hashlib.sha256(data).hexdigest()
    if actual != expected:
        raise SystemExit(
            f'extraction-contract: root extraction {rel} does not match '
            'libpkgstate 2.5.1'
        )
PY_CHECK
