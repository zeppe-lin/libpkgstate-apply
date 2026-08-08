// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

using namespace test_fixture;

namespace {

template<class Callable>
void
expect_package_state(Callable&& callable)
{
  try
  {
    callable();
    CHECK(false);
  }
  catch (const pkgstate::apply_adapter::projection_error& error)
  {
    CHECK(error.code() ==
          pkgstate::apply_adapter::projection_error_code::package_state_mismatch);
  }
}

} // namespace

int
main()
{
  const auto target_binding = state_target();
  const auto already_installed = state_package(target_binding, "1.0", {});
  const auto occupied = pkgstate::snapshot::make(
      target_binding, {already_installed});
  const planner_context planner;
  const auto incoming = incoming_authority("1.0", 1);
  const auto plan = installation_plan(occupied, planner, incoming);
  const auto target = application_target(target_binding, planner);
  const auto request = pkgapply::installation_application_request::make(
      plan, incoming, target, execution_control());
  const auto projection = application_projection(occupied, plan);
  const auto evidence = pkgapply::completed_application_evidence::installation(
      request,
      apply_identity<pkgapply::application_attempt_identity>(224),
      projection.identity(),
      apply_identity<pkgapply::application_journal_identity>(225),
      {installation_consequence(plan.paths().front())},
      durability());
  expect_package_state([&] {
    static_cast<void>(pkgstate::apply_adapter::project_completed_application(
        occupied,
        projection,
        request,
        evidence,
        pkgstate::installation_reason::explicit_request()));
  });

  upgrade_fixture wrong_control(
      plan_identity<pkgplan::installed_control_identity>(226));
  expect_package_state([&] {
    static_cast<void>(pkgstate::apply_adapter::project_completed_application(
        wrong_control.expected,
        wrong_control.projection,
        wrong_control.request,
        wrong_control.evidence));
  });
  return 0;
}
