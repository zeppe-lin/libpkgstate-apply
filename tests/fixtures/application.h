// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "authority.h"

namespace test_fixture {

struct planner_context final {
  pkgplan::target_system_context_identity target =
      plan_identity<pkgplan::target_system_context_identity>(60);
  pkgplan::observation_set_identity observations =
      plan_identity<pkgplan::observation_set_identity>(61);
  pkgplan::runtime_dependency_closure_identity runtime_closure =
      plan_identity<pkgplan::runtime_dependency_closure_identity>(62);
};

pkgplan::installation_plan
installation_plan(const pkgstate::snapshot& expected,
                  const planner_context& context,
                  const pkgapply::incoming_package_authority& incoming)
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
          context.observations,
          context.target,
          pkgplan::fact_set_completeness::complete,
          {pkgplan::target_path_observation::absent(path)}),
      context.runtime_closure,
      policy());

  const pkgplan::installation_result result = pkgplan::plan_install(request);
  CHECK(result.has_plan());
  CHECK(result.plan() != nullptr);
  return *result.plan();
}

pkgplan::installed_package_fact
planner_installed(const pkgstate::snapshot& expected,
                  const pkgstate::installed_package& installed,
                  std::optional<pkgplan::installed_control_identity>
                      control_override = std::nullopt)
{
  const pkgstate::package_release& release = installed.release();
  return pkgplan::installed_package_fact(
      translate_identity<pkgplan::installed_package_identity>(
          installed.identity()),
      control_override.value_or(
          translate_identity<pkgplan::installed_control_identity>(
              installed.control().identity())),
      translate_identity<pkgplan::installed_state_snapshot_identity>(
          expected.identity()),
      pkgplan::package_release(
          translate_identity<pkgplan::package_release_identity>(
              release.identity()),
          release.name(), release.version(), std::to_string(release.release())),
      planner_control(installed.control()));
}

pkgplan::installed_ownership_inventory
planner_ownership(const pkgstate::snapshot& expected,
                  const pkgstate::installed_package& installed)
{
  const pkgplan::package_path path = pkgplan::package_path::parse("tool");
  return pkgplan::installed_ownership_inventory(
      translate_identity<pkgplan::ownership_inventory_identity>(
          expected.ownership_identity()),
      translate_identity<pkgplan::installed_state_snapshot_identity>(
          expected.identity()),
      pkgplan::fact_set_completeness::complete,
      {pkgplan::installed_ownership_claim(
          path,
          translate_identity<pkgplan::installed_package_identity>(
              installed.identity()),
          planner_regular(2))});
}

pkgplan::upgrade_plan
upgrade_plan(const pkgstate::snapshot& expected,
             const pkgstate::installed_package& installed,
             const planner_context& context,
             const pkgapply::incoming_package_authority& incoming,
             std::optional<pkgplan::installed_control_identity>
                 control_override = std::nullopt)
{
  const pkgplan::package_path path = pkgplan::package_path::parse("tool");
  const auto archive = incoming.image().receipt().archive_digest();

  pkgplan::upgrade_request request(
      planner_installed(expected, installed, control_override),
      incoming.candidate(),
      incoming.artifact(),
      archive,
      incoming.image(),
      translate_identity<pkgplan::installed_state_snapshot_identity>(
          expected.identity()),
      planner_ownership(expected, installed),
      context.target,
      pkgplan::target_observation_set(
          context.observations,
          context.target,
          pkgplan::fact_set_completeness::complete,
          {pkgplan::target_path_observation::present(
              pkgplan::filesystem_object_fact(path, planner_regular(2)))}),
      context.runtime_closure,
      policy());

  const pkgplan::upgrade_result result = pkgplan::plan_upgrade(request);
  CHECK(result.has_plan());
  CHECK(result.plan() != nullptr);
  return *result.plan();
}

pkgplan::removal_plan
removal_plan(const pkgstate::snapshot& expected,
             const pkgstate::installed_package& installed,
             const planner_context& context)
{
  const pkgplan::package_path path = pkgplan::package_path::parse("tool");
  pkgplan::removal_request request(
      planner_installed(expected, installed),
      translate_identity<pkgplan::installed_state_snapshot_identity>(
          expected.identity()),
      planner_ownership(expected, installed),
      context.target,
      pkgplan::target_observation_set(
          context.observations,
          context.target,
          pkgplan::fact_set_completeness::complete,
          {pkgplan::target_path_observation::present(
              pkgplan::filesystem_object_fact(path, planner_regular(2)))}),
      policy());

  const pkgplan::removal_result result = pkgplan::plan_removal(request);
  CHECK(result.has_plan());
  CHECK(result.plan() != nullptr);
  return *result.plan();
}

pkgapply::application_target_context
application_target(const pkgstate::state_target_binding& state_target,
                   const planner_context& planner,
                   std::uint8_t seed = 1)
{
  return pkgapply::application_target_context::make(
      planner.target,
      translate_identity<pkgapply::managed_target_identity>(
          state_target.managed_target()),
      translate_identity<pkgapply::root_view_identity>(
          state_target.root_view()),
      apply_identity<pkgapply::observation_backend_identity>(seed + 10),
      apply_identity<pkgapply::mutation_backend_identity>(seed + 11),
      apply_identity<pkgapply::mutation_exclusion_domain_identity>(seed + 12),
      apply_identity<pkgapply::active_object_namespace_identity>(seed + 13),
      apply_identity<pkgapply::rejected_object_store_identity>(seed + 14),
      apply_identity<pkgapply::staging_namespace_identity>(seed + 15),
      apply_identity<pkgapply::journal_namespace_identity>(seed + 16),
      apply_identity<pkgapply::execution_capability_profile_identity>(seed + 17));
}

pkgapply::application_execution_control
execution_control(pkgapply::application_durability_requirement durability =
                      pkgapply::application_durability_requirement::
                          all_application_domains)
{
  return pkgapply::application_execution_control::make(
      pkgapply::application_recovery_requirement::best_effort,
      durability,
      pkgapply::application_cancellation_policy::recover_after_target_mutation);
}

pkgapply::application_durability_profile
durability()
{
  using D = pkgapply::application_durability_domain;
  using S = pkgapply::application_durability_status;
  return pkgapply::application_durability_profile({
      {D::journal, S::confirmed},
      {D::incoming_staging, S::confirmed},
      {D::recovery_staging, S::confirmed},
      {D::active_namespace, S::confirmed},
      {D::rejected_object_store, S::confirmed},
      {D::completed_evidence, S::confirmed},
  });
}

pkgapply::completed_object_fact
completed_regular(
    const pkgplan::package_path& path,
    std::uint8_t content,
    pkgapply::object_fact_completeness completeness =
        pkgapply::object_fact_completeness::complete)
{
  return pkgapply::completed_object_fact(
      path,
      pkgapply::completed_object_kind::regular,
      pkgapply::qualified_fact<std::uint32_t>::known(0755),
      pkgapply::qualified_fact<std::uint64_t>::known(0),
      pkgapply::qualified_fact<std::uint64_t>::known(0),
      pkgapply::qualified_fact<std::uint64_t>::known(4),
      pkgapply::qualified_fact<pkgapply::completed_object_timestamp>::known(
          {10, 0}),
      pkgapply::qualified_fact<pkgapply::completed_regular_content_identity>::
          known(apply_identity<pkgapply::completed_regular_content_identity>(
              content)),
      pkgapply::qualified_fact<std::string>::not_applicable(),
      pkgapply::qualified_fact<pkgapply::completed_device_number>::
          not_applicable(),
      pkgapply::qualified_fact<pkgapply::completed_hardlink_relation>::
          unknown(),
      pkgapply::object_fact_provenance::application_observation,
      completeness);
}

std::vector<pkgplan::installed_package_identity>
planner_owners(const pkgstate::snapshot& expected,
               const pkgplan::package_path& path)
{
  std::vector<pkgplan::installed_package_identity> owners;
  const pkgstate::package_path state_path =
      pkgstate::package_path::parse(path.string());
  for (const pkgstate::installed_package* owner : expected.owners(state_path))
  {
    owners.push_back(
        translate_identity<pkgplan::installed_package_identity>(
            owner->identity()));
  }
  return owners;
}

template<typename Plan>
pkgapply::lease_bound_state_projection
application_projection(
    const pkgstate::snapshot& expected,
    const Plan& plan,
    std::uint8_t seed = 30,
    bool wrong_owners = false,
    pkgapply::state_projection_completeness completeness =
        pkgapply::state_projection_completeness::complete)
{
  std::vector<pkgapply::projected_path_owners> paths;
  for (const auto& precondition : plan.preconditions().paths())
  {
    auto owners = planner_owners(expected, precondition.path());
    if (wrong_owners)
      owners.push_back(
          plan_identity<pkgplan::installed_package_identity>(99));
    paths.emplace_back(precondition.path(), std::move(owners));
  }

  return pkgapply::lease_bound_state_projection::make(
      apply_identity<pkgapply::mutation_lease_instance_identity>(seed),
      plan.preconditions().installed_snapshot(),
      plan.preconditions().ownership_inventory(),
      completeness,
      std::move(paths),
      apply_identity<pkgapply::state_projection_evidence_identity>(seed + 1));
}

pkgapply::completed_object_fact
completed_directory(const pkgplan::package_path& path)
{
  return pkgapply::completed_object_fact(
      path,
      pkgapply::completed_object_kind::directory,
      pkgapply::qualified_fact<std::uint32_t>::known(0755),
      pkgapply::qualified_fact<std::uint64_t>::known(0),
      pkgapply::qualified_fact<std::uint64_t>::known(0),
      pkgapply::qualified_fact<std::uint64_t>::not_applicable(),
      pkgapply::qualified_fact<pkgapply::completed_object_timestamp>::known(
          {10, 0}),
      pkgapply::qualified_fact<pkgapply::completed_regular_content_identity>::
          not_applicable(),
      pkgapply::qualified_fact<std::string>::not_applicable(),
      pkgapply::qualified_fact<pkgapply::completed_device_number>::
          not_applicable(),
      pkgapply::qualified_fact<pkgapply::completed_hardlink_relation>::
          not_applicable(),
      pkgapply::object_fact_provenance::application_observation,
      pkgapply::object_fact_completeness::complete);
}

pkgapply::application_path_role
application_role(pkgplan::installation_path_role role)
{
  return role == pkgplan::installation_path_role::incoming_entry
      ? pkgapply::application_path_role::incoming_entry
      : pkgapply::application_path_role::structural_parent;
}

pkgapply::application_path_role
application_role(pkgplan::upgrade_path_role role)
{
  switch (role)
  {
    case pkgplan::upgrade_path_role::incoming_entry:
      return pkgapply::application_path_role::incoming_entry;
    case pkgplan::upgrade_path_role::obsolete_old_path:
      return pkgapply::application_path_role::obsolete_old_path;
    case pkgplan::upgrade_path_role::structural_parent:
      return pkgapply::application_path_role::structural_parent;
  }
  return pkgapply::application_path_role::incoming_entry;
}

pkgapply::application_path_consequence
installation_consequence(const pkgplan::installation_path_decision& decision,
                         bool resulting_directory = false)
{
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
      pkgapply::application_path_observation::present(
          resulting_directory ? completed_directory(decision.path())
                              : completed_regular(decision.path(), 1)),
      std::nullopt,
      pkgapply::ownership_publication_status::eligible);
}

pkgapply::application_path_consequence
upgrade_consequence(const pkgplan::upgrade_path_decision& decision)
{
  return pkgapply::application_path_consequence(
      decision.path(),
      application_role(decision.role()),
      decision.active(),
      decision.rejected(),
      decision.incoming_entry(),
      decision.ownership(),
      pkgapply::application_effect_status::completed,
      pkgapply::application_effect_status::not_attempted,
      pkgapply::application_path_observation::present(
          completed_regular(decision.path(), 2)),
      pkgapply::application_path_observation::present(
          completed_regular(decision.path(), 3)),
      std::nullopt,
      pkgapply::ownership_publication_status::eligible);
}

pkgapply::application_path_consequence
removal_consequence(const pkgplan::removal_path_decision& decision)
{
  return pkgapply::application_path_consequence(
      decision.path(),
      pkgapply::application_path_role::installed_owned_path,
      decision.active(),
      decision.rejected(),
      std::nullopt,
      decision.ownership(),
      pkgapply::application_effect_status::completed,
      pkgapply::application_effect_status::not_attempted,
      pkgapply::application_path_observation::present(
          completed_regular(decision.path(), 2)),
      pkgapply::application_path_observation::absent(decision.path()),
      std::nullopt,
      pkgapply::ownership_publication_status::eligible);
}

} // namespace test_fixture
