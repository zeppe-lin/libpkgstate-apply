# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
public_inc = include_directories('../include')
pkgstate_apply = library(
  'pkgstate-apply',
  [
    'apply_adapter.cpp',
    'state_projection.cpp',
  ],
  cpp_args: ['-D_FILE_OFFSET_BITS=64'],
  include_directories: public_inc,
  dependencies: [
    libpkgstate_dep,
    libpkgapply_dep,
    libpkgstate_source_dep,
    libpkgstate_build_dep,
    libpkgplan_dep,
    libcrypto_dep,
  ],
  install: true,
  version: meson.project_version(),
  soversion: '3',
)
libpkgstate_apply_dep = declare_dependency(
  link_with: pkgstate_apply,
  include_directories: public_inc,
  dependencies: [
    libpkgstate_dep,
    libpkgapply_dep,
    libpkgstate_source_dep,
    libpkgstate_build_dep,
    libpkgplan_dep,
    libcrypto_dep,
  ],
  compile_args: ['-D_FILE_OFFSET_BITS=64'],
  version: meson.project_version(),
)
meson.override_dependency('libpkgstate-apply', libpkgstate_apply_dep)
install_headers(
  '../include/libpkgstate-apply/adapter.h',
  '../include/libpkgstate-apply/state_projection.h',
  '../include/libpkgstate-apply/libpkgstate-apply.h',
  subdir: 'libpkgstate-apply',
)
pkgconf.generate(
  pkgstate_apply,
  name: 'libpkgstate-apply',
  filebase: 'libpkgstate-apply',
  description: 'Completed application to installed-state publication adapter',
  subdirs: '.',
  requires: [libpkgstate_dep, libpkgapply_dep],
  requires_private: [libpkgstate_source_dep, libpkgstate_build_dep, libpkgplan_dep, libcrypto_dep],
)
