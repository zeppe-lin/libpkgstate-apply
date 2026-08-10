// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include <libpkgstate-apply/libpkgstate-apply.h>

#include <string>

int
main()
{
  pkgstate::apply_adapter::projection_error publication(
      pkgstate::apply_adapter::projection_error_code::completed_path_mismatch,
      "publication");
  pkgstate::apply_adapter::application_state_projection_error state(
      pkgstate::apply_adapter::application_state_projection_error_code::lease_lost,
      "state");

  if (publication.code() !=
          pkgstate::apply_adapter::projection_error_code::completed_path_mismatch ||
      state.code() !=
          pkgstate::apply_adapter::application_state_projection_error_code::lease_lost)
  {
    return 1;
  }
  return std::string(publication.what()) == "publication" &&
                 std::string(state.what()) == "state"
             ? 0
             : 1;
}
