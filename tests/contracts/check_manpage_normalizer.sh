#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
awk -f "$root/tools/canonicalize-man-roff.awk" /dev/null >/dev/null
printf '%s\n' '.EX' '\f[CR]x\f[]' '.EE' | awk -f "$root/tools/canonicalize-man-roff.awk" | grep -F 'x' >/dev/null
printf '%s\n' 'manpage-normalizer-contract: ok'
