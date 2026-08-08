// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

using namespace test_fixture;

namespace {

pkgapply::application_path_consequence
installation_after(const installation_fixture& fixture,
                   pkgapply::completed_object_fact object)
{
  const auto& decision = fixture.plan.paths().front();
  return pkgapply::application_path_consequence(
      decision.path(),
      application_role(decision.role()),
      decision.active(),
      decision.rejected(),
      decision.incoming_entry(),
      decision.ownership(),
      pkgapply::application_effect_status::completed,
      pkgapply::application_effect_status::not_attempted,
      pkgapply::application_path_observation::absent(decision.path()),
      pkgapply::application_path_observation::present(std::move(object)),
      std::nullopt,
      pkgapply::ownership_publication_status::eligible);
}

template<class Callable>
void
expect_projection_error(pkgstate::apply_adapter::projection_error_code code,
                        Callable&& callable)
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
  const auto& decision = fixture.plan.paths().front();

  // libpkgapply owns exact plan/path sealing and refuses incomplete path
  // universes before this adapter can see them.
  TEST_THROWS(
      std::invalid_argument,
      pkgapply::completed_application_evidence::installation(
          fixture.request,
          apply_identity<pkgapply::application_attempt_identity>(170),
          fixture.projection.identity(),
          apply_identity<pkgapply::application_journal_identity>(171),
          {}, durability()));

  // A completed application may truthfully carry a known resulting object whose
  // metadata is still partial. State publication requires stronger evidence.
  const auto partial = pkgapply::completed_application_evidence::installation(
      fixture.request,
      apply_identity<pkgapply::application_attempt_identity>(172),
      fixture.projection.identity(),
      apply_identity<pkgapply::application_journal_identity>(173),
      {installation_after(
          fixture,
          completed_regular(decision.path(), 1,
                            pkgapply::object_fact_completeness::partial))},
      durability());
  expect_projection_error(
      pkgstate::apply_adapter::projection_error_code::completed_path_mismatch,
      [&] {
        static_cast<void>(pkgstate::apply_adapter::project_completed_application(
            fixture.expected,
            fixture.projection,
            fixture.request,
            partial,
            pkgstate::installation_reason::explicit_request()));
      });

  // Completeness alone does not invent a missing fact. The application value is
  // constructible, but the publication bridge must refuse the absent mode.
  const auto missing_mode_object = pkgapply::completed_object_fact(
      decision.path(),
      pkgapply::completed_object_kind::regular,
      pkgapply::qualified_fact<std::uint32_t>::unknown(),
      pkgapply::qualified_fact<std::uint64_t>::known(0),
      pkgapply::qualified_fact<std::uint64_t>::known(0),
      pkgapply::qualified_fact<std::uint64_t>::known(4),
      pkgapply::qualified_fact<pkgapply::completed_object_timestamp>::known({10, 0}),
      pkgapply::qualified_fact<pkgapply::completed_regular_content_identity>::known(
          apply_identity<pkgapply::completed_regular_content_identity>(1)),
      pkgapply::qualified_fact<std::string>::not_applicable(),
      pkgapply::qualified_fact<pkgapply::completed_device_number>::not_applicable(),
      pkgapply::qualified_fact<pkgapply::completed_hardlink_relation>::unknown(),
      pkgapply::object_fact_provenance::application_observation,
      pkgapply::object_fact_completeness::complete);
  const auto missing_mode = pkgapply::completed_application_evidence::installation(
      fixture.request,
      apply_identity<pkgapply::application_attempt_identity>(174),
      fixture.projection.identity(),
      apply_identity<pkgapply::application_journal_identity>(175),
      {installation_after(fixture, missing_mode_object)},
      durability());
  expect_projection_error(
      pkgstate::apply_adapter::projection_error_code::completed_path_mismatch,
      [&] {
        static_cast<void>(pkgstate::apply_adapter::project_completed_application(
            fixture.expected,
            fixture.projection,
            fixture.request,
            missing_mode,
            pkgstate::installation_reason::explicit_request()));
      });

  // Complete regular evidence with no asserted hard-link peer remains complete
  // publication evidence and projects no anchor.
  const auto publication = pkgstate::apply_adapter::project_completed_application(
      fixture.expected,
      fixture.projection,
      fixture.request,
      fixture.evidence,
      pkgstate::installation_reason::explicit_request());
  const auto& object = publication.deltas().front().proposed_package()
                           ->manifest().front().object();
  CHECK(!object.hardlink_anchor().has_value());
  return 0;
}
