// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../support/test.h"

#include <libpkgstate-apply/state_projection.h>

int
main()
{
  CHECK(pkgstate::apply_adapter::application_state_projection_evidence_schema_version == 1);
  return 0;
}
