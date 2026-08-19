#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "manpage-source-contract: $*" >&2; exit 1; }
source=$root/docs/man/libpkgstate-apply.3.md
[ -s "$source" ] || fail 'canonical Markdown manual is missing'
first=$(sed -n '1p' "$source")
case "$first" in
  "% "*"(3) libpkgstate-apply | Version 3.1.2") ;;
  *) fail "invalid Pandoc title: $first" ;;
esac
grep -F '# NAME' "$source" >/dev/null || fail 'NAME section is missing'
[ -s "$root/docs/man/generated/libpkgstate-apply.3" ] || fail 'committed generated roff is missing'
[ ! -e "$root/man" ] || fail 'legacy root man/ authority remains'
if find "$root" -type f \( -name '*.scd' -o -name '*.scdoc' \) | grep . >/dev/null; then
  fail 'scdoc manual authority remains'
fi
