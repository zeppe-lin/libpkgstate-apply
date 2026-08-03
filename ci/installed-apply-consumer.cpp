// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include <libpkgstate-apply/libpkgstate-apply.h>

int
main()
{
  auto* volatile function = &pkgstate::apply_adapter::read_application_state;
  return function == nullptr ? 1 : 0;
}
