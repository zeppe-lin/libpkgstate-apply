# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').allowed()
  behavior = executable(
    'adapter-behavior-test',
    'apply_adapter_test.cpp',
    dependencies: libpkgstate_apply_dep,
    cpp_args: ['-UNDEBUG'],
  )
  test('adapter-behavior', behavior)
  public_header = executable(
    'public-header-test',
    'public_header_test.cpp',
    dependencies: libpkgstate_apply_dep,
  )
  test('public-header', public_header)
  sh = find_program('sh')
  foreach contract : ['architecture', 'release_metadata', 'repository', 'extraction']
    test(
      contract.replace('_', '-'),
      sh,
      args: [join_paths(meson.current_source_dir(), 'check_' + contract + '.sh'), meson.project_source_root()],
    )
  endforeach
  test(
    'pkgconfig-metadata',
    sh,
    args: [join_paths(meson.current_source_dir(), 'check_pkgconfig_metadata.sh'), meson.project_build_root()],
  )
endif
