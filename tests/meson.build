# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
if get_option('tests').allowed()
  unit_tests = [
    ['error-model', 'unit/error_model_test.cpp'],
    ['schema', 'unit/schema_test.cpp'],
  ]
  foreach item : unit_tests
    unit_test = executable(
      item[0] + '-test',
      item[1],
      dependencies: libpkgstate_apply_dep,
      cpp_args: ['-UNDEBUG'],
    )
    test(item[0], unit_test, suite: 'unit')
  endforeach

  integration_deps = [
    libpkgstate_apply_dep,
    libpkgstate_source_test_dep,
    libpkgstate_build_dep,
    libpkgsource_test_dep,
    libpkgsource_plan_test_dep,
    libpkgbuild_test_dep,
    libpkgimage_test_dep,
    libpkgbuild_image_test_dep,
    libpkgbuild_plan_test_dep,
    libpkgcatalog_test_dep,
    libpkgresolve_test_dep,
    libpkgplan_dep,
  ]
  integration_tests = [
    ['installation-publication', 'integration/installation_publication_test.cpp'],
    ['upgrade-publication', 'integration/upgrade_publication_test.cpp'],
    ['removal-publication', 'integration/removal_publication_test.cpp'],
    ['object-translation', 'integration/object_translation_test.cpp'],
    ['completed-evidence', 'integration/completed_evidence_test.cpp'],
    ['binding-refusal', 'integration/binding_refusal_test.cpp'],
    ['package-state-refusal', 'integration/package_state_refusal_test.cpp'],
    ['policy-outcome', 'integration/policy_outcome_test.cpp'],
    ['state-projection', 'integration/state_projection_test.cpp'],
    ['state-projection-refusal', 'integration/state_projection_refusal_test.cpp'],
    ['state-publication-projection', 'integration/state_publication_projection_test.cpp'],
  ]
  foreach item : integration_tests
    integration_test = executable(
      item[0] + '-integration-test',
      item[1],
      dependencies: integration_deps,
      cpp_args: ['-UNDEBUG'],
    )
    test(item[0], integration_test, suite: 'integration')
  endforeach

  public_headers = [
    ['export', 'libpkgstate-apply/export.h'],
    ['adapter', 'libpkgstate-apply/adapter.h'],
    ['state_projection', 'libpkgstate-apply/state_projection.h'],
    ['libpkgstate_apply', 'libpkgstate-apply/libpkgstate-apply.h'],
  ]
  foreach header : public_headers
    public_header = executable(
      header[0] + '-public-header-test',
      'header/public_header_test.cpp',
      dependencies: libpkgstate_apply_dep,
      cpp_args: ['-DLIBPKGSTATE_ADAPTER_TEST_HEADER="' + header[1] + '"'],
    )
    test(
      'compile public header ' + header[1],
      public_header,
      suite: 'header',
    )
  endforeach

  abi_layout_test = executable(
    'abi-layout-test',
    'contracts/abi_layout_test.cpp',
    dependencies: libpkgstate_apply_dep,
  )
  test('abi-layout', abi_layout_test, suite: 'contract')

  sh = find_program('sh')
  if library_kind == 'shared'
    nm = find_program('nm')
    test(
      'verify reviewed adapter ABI surface',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_abi_surface.sh'),
        pkgstate_apply.full_path(),
        meson.project_source_root() / 'abi/libpkgstate-apply.exports',
        nm.full_path(),
      ],
      suite: 'contract',
    )
    test(
      'apply-abi-generation',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_apply_abi_generation.sh'),
        pkgstate_apply.full_path(),
      ],
      suite: 'contract',
    )
  endif

  libpkgstate_public_include = libpkgstate_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgapply_public_include = libpkgapply_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgbuild_plan_public_include = libpkgbuild_plan_test_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgplan_public_include = libpkgplan_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgbuild_image_public_include = libpkgbuild_image_test_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgsource_plan_public_include = libpkgsource_plan_test_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgbuild_public_include = libpkgbuild_test_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgimage_public_include = libpkgimage_test_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgsource_public_include = libpkgsource_test_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgresolve_public_include = libpkgresolve_test_dep.get_variable(
    pkgconfig: 'includedir',
  )
  libpkgcatalog_public_include = libpkgcatalog_test_dep.get_variable(
    pkgconfig: 'includedir',
  )
  foreach contract : [
    'architecture',
    'documentation',
    'documentation_source',
    'release_metadata',
    'repository',
    'extraction',
    'ci',
    'test_layout',
    'manpage_normalizer',
    'manpage_source',
  ]
    contract_args = [
      join_paths(meson.current_source_dir(), 'contracts/check_' + contract + '.sh'),
      meson.project_source_root(),
    ]
    if contract == 'documentation'
      contract_args += [
        libpkgstate_public_include,
        libpkgapply_public_include,
        libpkgbuild_plan_public_include,
        libpkgplan_public_include,
        libpkgbuild_image_public_include,
        libpkgsource_plan_public_include,
        libpkgbuild_public_include,
        libpkgimage_public_include,
        libpkgsource_public_include,
        libpkgresolve_public_include,
        libpkgcatalog_public_include,
      ]
    endif
    test(
      contract.replace('_', '-'),
      sh,
      args: contract_args,
      suite: 'contract',
    )
  endforeach

  pandoc = find_program('pandoc', required: false)
  if pandoc.found()
    test(
      'contract generated manpages',
      sh,
      args: [
        join_paths(meson.current_source_dir(), 'contracts/check_manpage_generated.sh'),
        meson.project_source_root(),
        pandoc.full_path(),
      ],
      suite: 'contract',
    )
  endif

  test(
    'style-contract',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_style.sh'),
      meson.project_source_root(),
      meson.project_build_root(),
    ],
    suite: 'contract',
  )
  test(
    'pkgconfig-metadata',
    sh,
    args: [
      join_paths(meson.current_source_dir(), 'contracts/check_pkgconfig_metadata.sh'),
      meson.project_build_root(),
    ],
    suite: 'contract',
  )
endif
