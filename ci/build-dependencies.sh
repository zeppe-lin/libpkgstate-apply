#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu

if [ "$#" -ne 4 ]; then
  echo "usage: build-dependencies.sh SOURCE-ROOT BUILD-ROOT PREFIX {shared|static}" >&2
  exit 2
fi

source_root=$1
build_root=$2
prefix=$3
link_mode=$4
case $link_mode in
  shared|static) ;;
  *) echo "invalid link mode: $link_mode" >&2; exit 2 ;;
esac

export PKG_CONFIG_PATH="$prefix/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
export LD_LIBRARY_PATH="$prefix/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

build_one()
{
  name=$1
  shift
  meson setup "$build_root/$name" "$source_root/$name" \
    --wrap-mode=nofallback \
    --prefix="$prefix" \
    --libdir=lib \
    --buildtype="${MESON_BUILDTYPE:-debug}" \
    -Ddefault_library="$link_mode" \
    -Dlink_mode="$link_mode" \
    -Dtests=disabled \
    -Dman_pages=disabled \
    -Dwerror=true \
    ${MESON_SANITIZE:+-Db_sanitize="$MESON_SANITIZE"} \
    ${MESON_SANITIZE:+-Db_lundef=false} \
    "$@"
  meson compile -C "$build_root/$name"
  meson install -C "$build_root/$name"
}

build_one libpkgsource -Dhtml_docs=disabled
build_one libpkgimage -Dhtml_docs=disabled
build_one libpkgbuild -Dplanner_adapter=disabled
build_one libpkgplan -Dreference_tools=disabled -Dhtml_docs=disabled
build_one libpkgsource-plan -Dhtml_docs=disabled
build_one libpkgapply
build_one libpkgstate -Dtools=disabled
build_one libpkgstate-source
build_one libpkgstate-build
build_one libpkgstate-plan
