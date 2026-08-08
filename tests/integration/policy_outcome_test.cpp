// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

using namespace test_fixture;

namespace {

pkgplan::package_policy_snapshot
retained_policy(pkgplan::rejected_object_policy rejected,
                pkgplan::retained_active_ownership_policy ownership,
                std::uint8_t seed)
{
  return pkgplan::package_policy_snapshot(
      plan_identity<pkgplan::policy_snapshot_identity>(seed),
      pkgplan::normalized_path_policy(
          pkgplan::incoming_path_policy::retain(rejected, ownership),
          pkgplan::obsolete_path_policy::remove(),
          pkgplan::shared_ownership_policy::forbid,
          pkgplan::directory_cleanup_policy::remove_if_empty),
      {});
}

pkgplan::installation_plan
plan_with_existing_target(const pkgstate::snapshot& expected,
                          const planner_context& context,
                          const pkgapply::incoming_package_authority& incoming,
                          pkgplan::filesystem_object_metadata observed,
                          pkgplan::package_policy_snapshot policy)
{
  const pkgplan::package_path path = pkgplan::package_path::parse("tool");
  const auto archive = incoming.image().receipt().archive_digest();
  pkgplan::installation_request request(
      incoming.candidate(),
      incoming.artifact(),
      archive,
      incoming.image(),
      translate_identity<pkgplan::installed_state_snapshot_identity>(
          expected.identity()),
      pkgplan::installed_ownership_inventory(
          translate_identity<pkgplan::ownership_inventory_identity>(
              expected.ownership_identity()),
          translate_identity<pkgplan::installed_state_snapshot_identity>(
              expected.identity()),
          pkgplan::fact_set_completeness::complete,
          {}),
      context.target,
      pkgplan::target_observation_set(
          plan_identity<pkgplan::observation_set_identity>(211),
          context.target,
          pkgplan::fact_set_completeness::complete,
          {pkgplan::target_path_observation::present(
              pkgplan::filesystem_object_fact(path, std::move(observed)))}),
      context.runtime_closure,
      std::move(policy));
  const auto result = pkgplan::plan_install(request);
  CHECK(result.has_plan());
  CHECK(result.plan() != nullptr);
  return *result.plan();
}

pkgapply::application_path_consequence
retained_consequence(const pkgplan::installation_path_decision& decision,
                     std::uint8_t active_content,
                     std::optional<pkgapply::rejected_object_record_identity>
                         rejected = std::nullopt)
{
  const bool stage = decision.rejected() != pkgplan::planned_rejected_outcome::none;
  return pkgapply::application_path_consequence(
      decision.path(),
      application_role(decision.role()),
      decision.active(),
      decision.rejected(),
      decision.incoming_entry(),
      decision.ownership(),
      pkgapply::application_effect_status::completed,
      stage ? pkgapply::application_effect_status::completed
            : pkgapply::application_effect_status::not_attempted,
      pkgapply::application_path_observation::present(
          completed_regular(decision.path(), active_content)),
      pkgapply::application_path_observation::present(
          completed_regular(decision.path(), active_content)),
      std::move(rejected),
      pkgapply::ownership_publication_status::eligible);
}

} // namespace

int
main()
{
  const pkgstate::snapshot expected = pkgstate::snapshot::make(state_target());
  const planner_context context;
  const auto incoming = incoming_authority("1.0", 1);

  // Retaining a compatible pre-existing active object while adding the package
  // as owner must not claim that the active bytes came from the payload.
  const auto retain_plan = plan_with_existing_target(
      expected,
      context,
      incoming,
      planner_regular(1),
      retained_policy(
          pkgplan::rejected_object_policy::none,
          pkgplan::retained_active_ownership_policy::add_operated_owner,
          212));
  CHECK(retain_plan.paths().size() == 1);
  CHECK(retain_plan.paths().front().active() ==
        pkgplan::planned_active_outcome::retain_observed);
  CHECK(retain_plan.paths().front().ownership().incoming_package_owns_after());
  CHECK(retain_plan.publication().installed_manifest().size() == 1);

  const auto target = application_target(expected.target_binding(), context, 13);
  const auto request = pkgapply::installation_application_request::make(
      retain_plan, incoming, target, execution_control());
  const auto projection = application_projection(expected, retain_plan, 214);
  const auto evidence = pkgapply::completed_application_evidence::installation(
      request,
      apply_identity<pkgapply::application_attempt_identity>(215),
      projection.identity(),
      apply_identity<pkgapply::application_journal_identity>(216),
      {retained_consequence(retain_plan.paths().front(), 1)},
      durability());
  const auto publication = pkgstate::apply_adapter::project_completed_application(
      expected,
      projection,
      request,
      evidence,
      pkgstate::installation_reason::explicit_request());
  CHECK(publication.deltas().front().proposed_package().has_value());
  CHECK(publication.deltas().front().proposed_package()->manifest().size() == 1);
  CHECK(publication.deltas().front().proposed_package()->manifest().front().origin() ==
        pkgstate::active_object_origin::retained_existing);

  // Staging a rejected incoming object does not make the package owner of the
  // retained target object. Publication still records the installed package,
  // with an empty ownership manifest rather than manufacturing ownership.
  const auto reject_plan = plan_with_existing_target(
      expected,
      context,
      incoming,
      planner_regular(9),
      retained_policy(
          pkgplan::rejected_object_policy::stage,
          pkgplan::retained_active_ownership_policy::do_not_claim_operated_package,
          217));
  CHECK(reject_plan.paths().size() == 1);
  CHECK(reject_plan.paths().front().active() ==
        pkgplan::planned_active_outcome::retain_observed);
  CHECK(reject_plan.paths().front().rejected() ==
        pkgplan::planned_rejected_outcome::stage_incoming);
  CHECK(!reject_plan.paths().front().ownership().incoming_package_owns_after());
  CHECK(reject_plan.publication().installed_manifest().empty());

  const auto reject_target = application_target(expected.target_binding(), context, 20);
  const auto reject_request = pkgapply::installation_application_request::make(
      reject_plan, incoming, reject_target, execution_control());
  const auto reject_projection = application_projection(expected, reject_plan, 220);
  const auto rejected_identity =
      apply_identity<pkgapply::rejected_object_record_identity>(221);
  const auto reject_evidence =
      pkgapply::completed_application_evidence::installation(
          reject_request,
          apply_identity<pkgapply::application_attempt_identity>(222),
          reject_projection.identity(),
          apply_identity<pkgapply::application_journal_identity>(223),
          {retained_consequence(
              reject_plan.paths().front(), 9, rejected_identity)},
          durability());
  const auto reject_publication =
      pkgstate::apply_adapter::project_completed_application(
          expected,
          reject_projection,
          reject_request,
          reject_evidence,
          pkgstate::installation_reason::explicit_request());
  CHECK(reject_publication.deltas().front().proposed_package().has_value());
  CHECK(reject_publication.deltas().front().proposed_package()->manifest().empty());
  return 0;
}
