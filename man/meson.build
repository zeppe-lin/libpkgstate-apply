# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
scdoc = find_program('scdoc', required: get_option('man_pages'))
if scdoc.found()
  manual = custom_target(
    'pkgstate_apply_adapter.3',
    input: 'pkgstate_apply_adapter.3.scdoc',
    output: 'pkgstate_apply_adapter.3',
    command: [scdoc],
    capture: true,
    install: true,
    install_dir: get_option('mandir') / 'man3',
  )
endif
