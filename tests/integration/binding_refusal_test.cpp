// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

using namespace test_fixture;

namespace {

template<class Callable>
void
expect(pkgstate::apply_adapter::projection_error_code code, Callable&& callable)
{
  try
  {
    callable();
    CHECK(false);
  }
  catch (const pkgstate::apply_adapter::projection_error& error)
  {
    CHECK(error.code() == code);
  }
}

} // namespace

int
main()
{
  installation_fixture fixture;

  // Caller/callee seam: libpkgapply itself rejects an incoming authority that
  // does not bind the accepted plan.
  try
  {
    static_cast<void>(pkgapply::installation_application_request::make(
        fixture.plan, incoming_authority("1.0", 2), fixture.target,
        execution_control()));
    CHECK(false);
  }
  catch (const pkgapply::incoming_package_error& error)
  {
    CHECK(error.code() == pkgapply::incoming_package_error_code::plan_binding);
  }

  const auto different_control =
      pkgapply::installation_application_request::make(
          fixture.plan,
          fixture.incoming,
          fixture.target,
          execution_control(
              pkgapply::application_durability_requirement::visibility_only));
  expect(pkgstate::apply_adapter::projection_error_code::request_binding_mismatch,
         [&] {
           static_cast<void>(pkgstate::apply_adapter::project_completed_application(
               fixture.expected,
               fixture.projection,
               different_control,
               fixture.evidence,
               pkgstate::installation_reason::explicit_request()));
         });

  const auto other_projection =
      application_projection(fixture.expected, fixture.plan, 80);
  expect(pkgstate::apply_adapter::projection_error_code::state_projection_mismatch,
         [&] {
           static_cast<void>(pkgstate::apply_adapter::project_completed_application(
               fixture.expected,
               other_projection,
               fixture.request,
               fixture.evidence,
               pkgstate::installation_reason::explicit_request()));
         });

  installation_fixture wrong_owners(1, true);
  expect(
      pkgstate::apply_adapter::projection_error_code::ownership_projection_mismatch,
      [&] {
        static_cast<void>(pkgstate::apply_adapter::project_completed_application(
            wrong_owners.expected,
            wrong_owners.projection,
            wrong_owners.request,
            wrong_owners.evidence,
            pkgstate::installation_reason::explicit_request()));
      });

  const pkgstate::snapshot other_state =
      pkgstate::snapshot::make(state_target(20));
  expect(pkgstate::apply_adapter::projection_error_code::expected_state_mismatch,
         [&] {
           static_cast<void>(pkgstate::apply_adapter::project_completed_application(
               other_state,
               fixture.projection,
               fixture.request,
               fixture.evidence,
               pkgstate::installation_reason::explicit_request()));
         });

  const auto incomplete_projection = application_projection(
      fixture.expected,
      fixture.plan,
      90,
      false,
      pkgapply::state_projection_completeness::incomplete);
  const auto incomplete_evidence =
      pkgapply::completed_application_evidence::installation(
          fixture.request,
          apply_identity<pkgapply::application_attempt_identity>(91),
          incomplete_projection.identity(),
          apply_identity<pkgapply::application_journal_identity>(92),
          {installation_consequence(fixture.plan.paths().front())},
          durability());
  expect(pkgstate::apply_adapter::projection_error_code::expected_state_mismatch,
         [&] {
           static_cast<void>(pkgstate::apply_adapter::project_completed_application(
               fixture.expected,
               incomplete_projection,
               fixture.request,
               incomplete_evidence,
               pkgstate::installation_reason::explicit_request()));
         });

  const auto foreign_target = pkgapply::application_target_context::make(
      fixture.planner.target,
      apply_identity<pkgapply::managed_target_identity>(100),
      translate_identity<pkgapply::root_view_identity>(
          fixture.expected.target_binding().root_view()),
      apply_identity<pkgapply::observation_backend_identity>(101),
      apply_identity<pkgapply::mutation_backend_identity>(102),
      apply_identity<pkgapply::mutation_exclusion_domain_identity>(103),
      apply_identity<pkgapply::active_object_namespace_identity>(104),
      apply_identity<pkgapply::rejected_object_store_identity>(105),
      apply_identity<pkgapply::staging_namespace_identity>(106),
      apply_identity<pkgapply::journal_namespace_identity>(107),
      apply_identity<pkgapply::execution_capability_profile_identity>(108));
  const auto foreign_request = pkgapply::installation_application_request::make(
      fixture.plan, fixture.incoming, foreign_target, execution_control());
  const auto foreign_evidence =
      pkgapply::completed_application_evidence::installation(
          foreign_request,
          apply_identity<pkgapply::application_attempt_identity>(109),
          fixture.projection.identity(),
          apply_identity<pkgapply::application_journal_identity>(110),
          {installation_consequence(fixture.plan.paths().front())},
          durability());
  expect(pkgstate::apply_adapter::projection_error_code::target_binding_mismatch,
         [&] {
           static_cast<void>(pkgstate::apply_adapter::project_completed_application(
               fixture.expected,
               fixture.projection,
               foreign_request,
               foreign_evidence,
               pkgstate::installation_reason::explicit_request()));
         });

  upgrade_fixture wrong_control(
      plan_identity<pkgplan::installed_control_identity>(120));
  expect(pkgstate::apply_adapter::projection_error_code::package_state_mismatch,
         [&] {
           static_cast<void>(pkgstate::apply_adapter::project_completed_application(
               wrong_control.expected,
               wrong_control.projection,
               wrong_control.request,
               wrong_control.evidence));
         });

  // Installation publication cannot overwrite an already-installed package.
  const auto occupied = pkgstate::snapshot::make(
      fixture.expected.target_binding(),
      {state_package(fixture.expected.target_binding(), "1.0", {})});
  expect(pkgstate::apply_adapter::projection_error_code::expected_state_mismatch,
         [&] {
           static_cast<void>(pkgstate::apply_adapter::project_completed_application(
               occupied,
               fixture.projection,
               fixture.request,
               fixture.evidence,
               pkgstate::installation_reason::explicit_request()));
         });

  return 0;
}
