// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

#include <memory>

using namespace test_fixture;

namespace {

template<class Callable>
void
expect(pkgstate::apply_adapter::application_state_projection_error_code code,
       Callable&& callable)
{
  try
  {
    callable();
    CHECK(false);
  }
  catch (const pkgstate::apply_adapter::application_state_projection_error& error)
  {
    CHECK(error.code() == code);
  }
}

class failing_store final : public pkgstate::canonical_store {
public:
  pkgstate::snapshot read() const override
  {
    throw pkgstate::store_error("fixture store read failed");
  }

protected:
  std::unique_ptr<pkgstate::canonical_publication_transaction>
  begin_publication() const override
  {
    throw std::runtime_error("unexpected publication");
  }
};

pkgapply::removal_application_request
removal_request_with_ownership(const removal_fixture& fixture,
                               pkgplan::ownership_inventory_identity identity,
                               bool add_foreign_owner)
{
  const pkgplan::package_path path = pkgplan::package_path::parse("tool");
  std::vector<pkgplan::installed_ownership_claim> claims;
  claims.emplace_back(
      path,
      translate_identity<pkgplan::installed_package_identity>(
          fixture.old_package.identity()),
      planner_regular(2));
  if (add_foreign_owner)
  {
    claims.emplace_back(
        path,
        plan_identity<pkgplan::installed_package_identity>(230),
        planner_regular(2));
  }

  pkgplan::installed_ownership_inventory ownership(
      std::move(identity),
      translate_identity<pkgplan::installed_state_snapshot_identity>(
          fixture.expected.identity()),
      pkgplan::fact_set_completeness::complete,
      std::move(claims));
  pkgplan::removal_request planning_request(
      planner_installed(fixture.expected, fixture.old_package),
      translate_identity<pkgplan::installed_state_snapshot_identity>(
          fixture.expected.identity()),
      std::move(ownership),
      fixture.planner.target,
      pkgplan::target_observation_set(
          plan_identity<pkgplan::observation_set_identity>(231),
          fixture.planner.target,
          pkgplan::fact_set_completeness::complete,
          {pkgplan::target_path_observation::present(
              pkgplan::filesystem_object_fact(path, planner_regular(2)))}),
      policy());
  const auto result = pkgplan::plan_removal(planning_request);
  CHECK(result.has_plan());
  CHECK(result.plan() != nullptr);
  return pkgapply::removal_application_request::make(
      *result.plan(), fixture.target, execution_control());
}

} // namespace

int
main()
{
  removal_fixture fixture;
  const pkgapply::package_application_request request(fixture.request);

  {
    std::vector<std::string> trace;
    recording_lease lease(
        apply_identity<pkgapply::mutation_lease_instance_identity>(140),
        fixture.target.identity(), fixture.target.mutation_exclusion_domain(),
        {false}, trace);
    recording_store store(fixture.expected, trace);
    expect(pkgstate::apply_adapter::application_state_projection_error_code::lease_not_held,
           [&] {
             static_cast<void>(pkgstate::apply_adapter::read_application_state(
                 request, lease, store));
           });
    CHECK(store.reads() == 0);
  }

  {
    std::vector<std::string> trace;
    recording_lease lease(
        apply_identity<pkgapply::mutation_lease_instance_identity>(141),
        fixture.target.identity(), fixture.target.mutation_exclusion_domain(),
        {true, false}, trace);
    recording_store store(fixture.expected, trace);
    expect(pkgstate::apply_adapter::application_state_projection_error_code::lease_lost,
           [&] {
             static_cast<void>(pkgstate::apply_adapter::read_application_state(
                 request, lease, store));
           });
    CHECK(store.reads() == 1);
  }

  {
    std::vector<std::string> trace;
    recording_lease lease(
        apply_identity<pkgapply::mutation_lease_instance_identity>(142),
        apply_identity<pkgapply::application_target_context_identity>(143),
        fixture.target.mutation_exclusion_domain(), {true}, trace);
    recording_store store(fixture.expected, trace);
    expect(pkgstate::apply_adapter::application_state_projection_error_code::lease_target_mismatch,
           [&] {
             static_cast<void>(pkgstate::apply_adapter::read_application_state(
                 request, lease, store));
           });
    CHECK(store.reads() == 0);
  }

  {
    std::vector<std::string> trace;
    recording_lease lease(
        apply_identity<pkgapply::mutation_lease_instance_identity>(144),
        fixture.target.identity(),
        apply_identity<pkgapply::mutation_exclusion_domain_identity>(145),
        {true}, trace);
    recording_store store(fixture.expected, trace);
    expect(pkgstate::apply_adapter::application_state_projection_error_code::lease_domain_mismatch,
           [&] {
             static_cast<void>(pkgstate::apply_adapter::read_application_state(
                 request, lease, store));
           });
    CHECK(store.reads() == 0);
  }

  {
    std::vector<std::string> trace;
    recording_lease lease(
        apply_identity<pkgapply::mutation_lease_instance_identity>(146),
        fixture.target.identity(), fixture.target.mutation_exclusion_domain(),
        {true}, trace);
    recording_store store(pkgstate::snapshot::make(state_target(20)), trace);
    expect(pkgstate::apply_adapter::application_state_projection_error_code::target_binding_mismatch,
           [&] {
             static_cast<void>(pkgstate::apply_adapter::read_application_state(
                 request, lease, store));
           });
    CHECK(store.reads() == 1);
  }

  {
    std::vector<std::string> trace;
    recording_lease lease(
        apply_identity<pkgapply::mutation_lease_instance_identity>(147),
        fixture.target.identity(), fixture.target.mutation_exclusion_domain(),
        {true}, trace);
    recording_store store(pkgstate::snapshot::make(fixture.target_binding), trace);
    expect(pkgstate::apply_adapter::application_state_projection_error_code::expected_snapshot_mismatch,
           [&] {
             static_cast<void>(pkgstate::apply_adapter::read_application_state(
                 request, lease, store));
           });
    CHECK(store.reads() == 1);
  }

  {
    const auto wrong = removal_request_with_ownership(
        fixture,
        plan_identity<pkgplan::ownership_inventory_identity>(232),
        false);
    std::vector<std::string> trace;
    recording_lease lease(
        apply_identity<pkgapply::mutation_lease_instance_identity>(148),
        wrong.target().identity(), wrong.target().mutation_exclusion_domain(),
        {true}, trace);
    recording_store store(fixture.expected, trace);
    expect(pkgstate::apply_adapter::application_state_projection_error_code::ownership_inventory_mismatch,
           [&] {
             static_cast<void>(pkgstate::apply_adapter::read_application_state(
                 pkgapply::package_application_request(wrong), lease, store));
           });
    CHECK(store.reads() == 1);
  }

  {
    const auto wrong = removal_request_with_ownership(
        fixture,
        translate_identity<pkgplan::ownership_inventory_identity>(
            fixture.expected.ownership_identity()),
        true);
    std::vector<std::string> trace;
    recording_lease lease(
        apply_identity<pkgapply::mutation_lease_instance_identity>(149),
        wrong.target().identity(), wrong.target().mutation_exclusion_domain(),
        {true}, trace);
    recording_store store(fixture.expected, trace);
    expect(pkgstate::apply_adapter::application_state_projection_error_code::path_owners_mismatch,
           [&] {
             static_cast<void>(pkgstate::apply_adapter::read_application_state(
                 pkgapply::package_application_request(wrong), lease, store));
           });
    CHECK(store.reads() == 1);
  }

  {
    std::vector<std::string> trace;
    recording_lease lease(
        apply_identity<pkgapply::mutation_lease_instance_identity>(150),
        fixture.target.identity(), fixture.target.mutation_exclusion_domain(),
        {true}, trace);
    failing_store store;
    try
    {
      static_cast<void>(pkgstate::apply_adapter::read_application_state(
          request, lease, store));
      CHECK(false);
    }
    catch (const pkgstate::store_error& error)
    {
      CHECK(std::string(error.what()) == "fixture store read failed");
    }
  }

  return 0;
}
