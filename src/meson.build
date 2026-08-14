# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later

public_inc = include_directories('../include')

elf_export_generator = find_program(
  meson.project_source_root() / 'tools/generate-elf-export-script.sh',
)

export_script_name = 'libpkgstate-apply.exports.map'
export_script_path = meson.current_build_dir() / export_script_name
export_script = configure_file(
  input: '../abi/libpkgstate-apply.exports',
  output: export_script_name,
  command: [elf_export_generator, '@INPUT@'],
  capture: true,
)

pkgstate_apply = library(
  'pkgstate-apply',
  [
    'apply_adapter.cpp',
    'state_projection.cpp',
  ],
  cpp_args: visibility_cpp_args + [
    '-DPKGSTATE_APPLY_BUILDING_LIBRARY',
    '-D_FILE_OFFSET_BITS=64'
  ],
  gnu_symbol_visibility: 'hidden',
  link_args: ['-Wl,--version-script=' + export_script_path],
  link_depends: [export_script],
  include_directories: public_inc,
  dependencies: [
    libpkgstate_dep,
    libpkgapply_dep,
    libpkgstate_build_dep,
    libpkgstate_plan_dep,
    libpkgplan_dep,
    libcrypto_dep,
  ],
  install: true,
  version: meson.project_version(),
  soversion: '3',
)

libpkgstate_apply_consumer_deps = [
  libpkgstate_dep,
  libpkgapply_dep,
]

if want_static
  libpkgstate_apply_consumer_deps += [
    libpkgstate_build_dep,
    libpkgstate_plan_dep,
    libpkgplan_dep,
    libcrypto_dep,
  ]
endif

libpkgstate_apply_dep = declare_dependency(
  link_with: pkgstate_apply,
  include_directories: public_inc,
  dependencies: libpkgstate_apply_consumer_deps,
  compile_args: ['-D_FILE_OFFSET_BITS=64'],
  version: meson.project_version(),
)

meson.override_dependency('libpkgstate-apply', libpkgstate_apply_dep)

install_headers(
  '../include/libpkgstate-apply/export.h',
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
  requires: [
    'libpkgstate >= 3.0.0',
    'libpkgapply >= 3.0.1',
  ],
  requires_private: [
    'libpkgstate-build >= 3.1.0',
    'libpkgstate-plan >= 3.0.0',
    'libpkgplan >= 0.3.0',
    libcrypto_dep,
  ],
)
