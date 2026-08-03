#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
python3 - "$root" <<'PY_CHECK'
from pathlib import Path
import hashlib, sys
root=Path(sys.argv[1])
manifest=root/'docs/history/libpkgstate-2.5.1-origin.sha256'
macro='PKGSTATE_APPLY_API '
export_include='\n#include <libpkgstate-apply/export.h>\n'
for line in manifest.read_text().splitlines():
    expected, rel=line.split('  ',1)
    data=(root/rel).read_bytes()
    if rel.endswith('.h'):
        text=data.decode()
        text=text.replace(export_include, '')
        text=text.replace(macro, '')
        data=text.encode()
    actual=hashlib.sha256(data).hexdigest()
    if actual != expected:
        raise SystemExit(f'extraction-contract: {rel} drifted from libpkgstate 2.5.1')
PY_CHECK
