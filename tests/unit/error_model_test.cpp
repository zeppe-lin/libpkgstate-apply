// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../support/test.h"

#include <type_traits>

#include <libpkgstate-apply/adapter.h>
#include <libpkgstate-apply/state_projection.h>

int
main()
{
  static_assert(std::is_base_of_v<std::invalid_argument,
                                  pkgstate::apply_adapter::projection_error>);
  static_assert(std::is_base_of_v<
                std::invalid_argument,
                pkgstate::apply_adapter::application_state_projection_error>);

  const pkgstate::apply_adapter::projection_error projection(
      pkgstate::apply_adapter::projection_error_code::completed_path_mismatch,
      "completed path mismatch");
  CHECK(projection.code() ==
        pkgstate::apply_adapter::projection_error_code::completed_path_mismatch);
  CHECK(std::string(projection.what()) == "completed path mismatch");

  const pkgstate::apply_adapter::application_state_projection_error state(
      pkgstate::apply_adapter::application_state_projection_error_code::lease_lost,
      "lease lost");
  CHECK(state.code() ==
        pkgstate::apply_adapter::application_state_projection_error_code::lease_lost);
  CHECK(std::string(state.what()) == "lease lost");
  return 0;
}
