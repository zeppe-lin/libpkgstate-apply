// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

using namespace test_fixture;

int
main()
{
  removal_fixture fixture;
  const pkgapply::package_application_request request(fixture.request);
  std::vector<std::string> trace;
  recording_lease lease(
      apply_identity<pkgapply::mutation_lease_instance_identity>(130),
      fixture.target.identity(),
      fixture.target.mutation_exclusion_domain(),
      {true}, trace);
  recording_store store(fixture.expected, trace);

  const auto projected =
      pkgstate::apply_adapter::read_application_state(request, lease, store);

  CHECK(store.reads() == 1);
  CHECK(trace == std::vector<std::string>({"held", "read", "held", "held"}));
  CHECK(projected.state().identity() == fixture.expected.identity());
  CHECK(projected.projection().lease() == lease.identity());
  CHECK(projected.projection().snapshot().string() ==
        fixture.expected.identity().string());
  CHECK(projected.projection().ownership_inventory().string() ==
        fixture.expected.ownership_identity().string());
  CHECK(projected.projection().completeness() ==
        pkgapply::state_projection_completeness::complete);
  CHECK(projected.projection().paths().size() ==
        fixture.plan.preconditions().paths().size());
  CHECK(projected.projection().paths().front().path() ==
        fixture.plan.preconditions().paths().front().path());
  CHECK(projected.projection().paths().front().owners() ==
        fixture.plan.preconditions().paths().front().owners());

  std::vector<std::string> repeated_trace;
  recording_lease repeated_lease(
      lease.identity(), fixture.target.identity(),
      fixture.target.mutation_exclusion_domain(), {true}, repeated_trace);
  recording_store repeated_store(fixture.expected, repeated_trace);
  const auto repeated = pkgstate::apply_adapter::read_application_state(
      request, repeated_lease, repeated_store);
  CHECK(repeated.projection().identity() == projected.projection().identity());
  CHECK(repeated.projection().evidence() == projected.projection().evidence());

  std::vector<std::string> other_trace;
  recording_lease other_lease(
      apply_identity<pkgapply::mutation_lease_instance_identity>(131),
      fixture.target.identity(), fixture.target.mutation_exclusion_domain(),
      {true}, other_trace);
  recording_store other_store(fixture.expected, other_trace);
  const auto other = pkgstate::apply_adapter::read_application_state(
      request, other_lease, other_store);
  CHECK(other.projection().identity() != projected.projection().identity());
  CHECK(other.projection().evidence() != projected.projection().evidence());
  return 0;
}
