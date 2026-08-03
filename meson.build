# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
project(
  'libpkgstate-apply',
  'cpp',
  version: '3.0.0',
  license: 'GPL-3.0-or-later',
  meson_version: '>=1.6.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=3',
    'default_library=shared',
    'b_ndebug=if-release',
  ],
)
library_kind = get_option('default_library')
if library_kind == 'both'
  error('libpkgstate-apply requires separate shared and static builds')
endif
want_static = get_option('link_mode') == 'static'
if (library_kind == 'static') != want_static
  error('default_library and link_mode must both select shared or static')
endif
pkgconf = import('pkgconfig')
libpkgstate_dep = dependency(
  'libpkgstate',
  version: '>=3.0.0',
  required: true,
  static: want_static,
)
libpkgapply_dep = dependency(
  'libpkgapply',
  version: '>=2.3.0',
  required: true,
  static: want_static,
)
libpkgstate_source_dep = dependency(
  'libpkgstate-source',
  version: '>=3.0.0',
  required: true,
  static: want_static,
)
libpkgstate_build_dep = dependency(
  'libpkgstate-build',
  version: '>=3.0.0',
  required: true,
  static: want_static,
)
libpkgplan_dep = dependency(
  'libpkgplan',
  version: '>=0.3.0',
  required: true,
  static: want_static,
)
libcrypto_dep = dependency(
  'libcrypto',
  required: true,
  static: want_static,
)

cpp = meson.get_compiler('cpp')

subdir('src')
subdir('man')
subdir('tests')
