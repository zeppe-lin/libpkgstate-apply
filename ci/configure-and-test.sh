# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
[ "$#" -ge 2 ] || exit 2
build=$1; mode=$2; shift 2
case $mode in shared|static) ;; *) exit 2 ;; esac
set -- --wrap-mode=nofallback --fatal-meson-warnings -Ddefault_library="$mode" -Dlink_mode="$mode" -Dtests=enabled -Dwerror=true "$@"
if [ -f "$build/meson-private/coredata.dat" ]; then meson setup --wipe "$build" "$@"; else meson setup "$build" "$@"; fi
meson compile -C "$build"
meson test -C "$build" --no-rebuild --print-errorlogs
