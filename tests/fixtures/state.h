// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "application.h"

namespace test_fixture {

class recording_lease final : public pkgapply::target_mutation_lease {
public:
  recording_lease(
      pkgapply::mutation_lease_instance_identity identity,
      pkgapply::application_target_context_identity target,
      pkgapply::mutation_exclusion_domain_identity domain,
      std::vector<bool> held_results,
      std::vector<std::string>& trace)
      : identity_(std::move(identity)), target_(std::move(target)),
        domain_(std::move(domain)), held_results_(std::move(held_results)),
        trace_(trace)
  {
  }

  const pkgapply::mutation_lease_instance_identity&
  identity() const noexcept override
  {
    return identity_;
  }

  const pkgapply::application_target_context_identity&
  target() const noexcept override
  {
    return target_;
  }

  const pkgapply::mutation_exclusion_domain_identity&
  exclusion_domain() const noexcept override
  {
    return domain_;
  }

  bool held() const noexcept override
  {
    trace_.push_back("held");
    const std::size_t index = held_calls_++;
    if (held_results_.empty())
      return false;
    return held_results_[std::min(index, held_results_.size() - 1)];
  }

private:
  pkgapply::mutation_lease_instance_identity identity_;
  pkgapply::application_target_context_identity target_;
  pkgapply::mutation_exclusion_domain_identity domain_;
  std::vector<bool> held_results_;
  std::vector<std::string>& trace_;
  mutable std::size_t held_calls_ = 0;
};

class recording_store final : public pkgstate::canonical_store {
public:
  recording_store(pkgstate::snapshot state, std::vector<std::string>& trace)
      : state_(std::move(state)), trace_(trace)
  {
  }

  pkgstate::snapshot read() const override
  {
    trace_.push_back("read");
    ++reads_;
    return state_;
  }

  std::size_t reads() const noexcept { return reads_; }

protected:
  std::unique_ptr<pkgstate::canonical_publication_transaction>
  begin_publication() const override
  {
    throw std::runtime_error("unexpected publication");
  }

private:
  pkgstate::snapshot state_;
  std::vector<std::string>& trace_;
  mutable std::size_t reads_ = 0;
};

struct installation_fixture final {
  pkgstate::snapshot expected;
  planner_context planner;
  pkgapply::incoming_package_authority incoming;
  pkgplan::installation_plan plan;
  pkgapply::application_target_context target;
  pkgapply::installation_application_request request;
  pkgapply::lease_bound_state_projection projection;
  pkgapply::completed_application_evidence evidence;

  explicit installation_fixture(std::uint8_t target_seed = 1,
                                bool wrong_owners = false)
      : expected(pkgstate::snapshot::make(state_target(target_seed))),
        planner(),
        incoming(incoming_authority("1.0", 1)),
        plan(installation_plan(expected, planner, incoming)),
        target(application_target(
            expected.target_binding(), planner, target_seed)),
        request(pkgapply::installation_application_request::make(
            plan, incoming, target, execution_control())),
        projection(application_projection(
            expected, plan, 30, wrong_owners)),
        evidence(pkgapply::completed_application_evidence::installation(
            request,
            apply_identity<pkgapply::application_attempt_identity>(40),
            projection.identity(),
            apply_identity<pkgapply::application_journal_identity>(41),
            {installation_consequence(plan.paths().front())},
            durability()))
  {
  }
};

struct upgrade_fixture final {
  pkgstate::state_target_binding target_binding;
  pkgstate::installed_package old_package;
  pkgstate::snapshot expected;
  planner_context planner;
  pkgapply::incoming_package_authority incoming;
  pkgplan::upgrade_plan plan;
  pkgapply::application_target_context target;
  pkgapply::upgrade_application_request request;
  pkgapply::lease_bound_state_projection projection;
  pkgapply::completed_application_evidence evidence;

  explicit upgrade_fixture(
      std::optional<pkgplan::installed_control_identity> control_override =
          std::nullopt)
      : target_binding(state_target()),
        old_package(state_package(
            target_binding,
            "1.0",
            {pkgstate::owned_entry::make(
              pkgstate::package_path::parse("tool"), state_regular(2),
              pkgstate::active_object_origin::incoming_payload)})),
        expected(pkgstate::snapshot::make(target_binding, {old_package})),
        planner(),
        incoming(incoming_authority("2.0", 3)),
        plan(upgrade_plan(
            expected, old_package, planner, incoming, control_override)),
        target(application_target(target_binding, planner)),
        request(pkgapply::upgrade_application_request::make(
            plan, incoming, target, execution_control())),
        projection(application_projection(expected, plan)),
        evidence(pkgapply::completed_application_evidence::upgrade(
            request,
            apply_identity<pkgapply::application_attempt_identity>(42),
            projection.identity(),
            apply_identity<pkgapply::application_journal_identity>(43),
            {upgrade_consequence(plan.paths().front())},
            durability()))
  {
  }
};

struct removal_fixture final {
  pkgstate::state_target_binding target_binding;
  pkgstate::installed_package old_package;
  pkgstate::snapshot expected;
  planner_context planner;
  pkgplan::removal_plan plan;
  pkgapply::application_target_context target;
  pkgapply::removal_application_request request;
  pkgapply::lease_bound_state_projection projection;
  pkgapply::completed_application_evidence evidence;

  removal_fixture()
      : target_binding(state_target()),
        old_package(state_package(
            target_binding,
            "1.0",
            {pkgstate::owned_entry::make(
              pkgstate::package_path::parse("tool"), state_regular(2),
              pkgstate::active_object_origin::incoming_payload)})),
        expected(pkgstate::snapshot::make(target_binding, {old_package})),
        planner(),
        plan(removal_plan(expected, old_package, planner)),
        target(application_target(target_binding, planner)),
        request(pkgapply::removal_application_request::make(
            plan, target, execution_control())),
        projection(application_projection(expected, plan)),
        evidence(pkgapply::completed_application_evidence::removal(
            request,
            apply_identity<pkgapply::application_attempt_identity>(44),
            projection.identity(),
            apply_identity<pkgapply::application_journal_identity>(45),
            {removal_consequence(plan.paths().front())},
            durability()))
  {
  }
};

} // namespace test_fixture
