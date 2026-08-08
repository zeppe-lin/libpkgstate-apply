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
cpp = meson.get_compiler('cpp')
visibility_cpp_args = []
if cpp.has_argument('-fvisibility-inlines-hidden')
  visibility_cpp_args += ['-fvisibility-inlines-hidden']
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
libpkgstate_plan_dep = dependency(
  'libpkgstate-plan',
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
libpkgsource_test_dep = dependency(
  'libpkgsource',
  version: '>=3.0.0',
  required: get_option('tests'),
  static: want_static,
)
libpkgsource_plan_test_dep = dependency(
  'libpkgsource-plan',
  version: ['>=1.0.0', '<2.0.0'],
  required: get_option('tests'),
  static: want_static,
)
libpkgbuild_test_dep = dependency(
  'libpkgbuild',
  version: ['>=3.0.0', '<4.0.0'],
  required: get_option('tests'),
  static: want_static,
)
libpkgstate_source_test_dep = dependency(
  'libpkgstate-source',
  version: '>=3.0.0',
  required: get_option('tests'),
  static: want_static,
)
libpkgimage_test_dep = dependency(
  'libpkgimage',
  version: '>=0.4.0',
  required: get_option('tests'),
  static: want_static,
)

libpkgbuild_image_test_dep = dependency(
  'libpkgbuild-image',
  version: ['>=1.0.0', '<2.0.0'],
  required: get_option('tests'),
  static: want_static,
)
libpkgbuild_plan_test_dep = dependency(
  'libpkgbuild-plan',
  version: ['>=1.0.0', '<2.0.0'],
  required: get_option('tests'),
  static: want_static,
)
libpkgcatalog_test_dep = dependency(
  'libpkgcatalog',
  version: ['>=3.0.0', '<4.0.0'],
  required: get_option('tests'),
  static: want_static,
)
libpkgresolve_test_dep = dependency(
  'libpkgresolve',
  version: ['>=2.0.0', '<3.0.0'],
  required: get_option('tests'),
  static: want_static,
)
subdir('src')
subdir('docs')
subdir('man')
subdir('tests')
