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

  pkgapply::application_attempt_nonce::byte_array nonce_bytes{};
  for (std::size_t index = 0; index < nonce_bytes.size(); ++index)
    nonce_bytes[index] = static_cast<std::uint8_t>(20U + index);
  const auto attempt = pkgapply::application_attempt::make(
      request.identity(), request.target().identity(),
      request.target().mutation_backend(),
      pkgapply::application_attempt_nonce::from_bytes(nonce_bytes));
  const auto journal = pkgapply::application_journal_header::make(
      request.kind(), request.identity(), request.plan(), attempt,
      request.target().identity(), request.control().identity(),
      projected.projection().identity(), lease.identity(),
      request.target().mutation_backend());

  std::vector<std::string> restart_trace;
  recording_lease restart_lease(
      apply_identity<pkgapply::mutation_lease_instance_identity>(132),
      fixture.target.identity(), fixture.target.mutation_exclusion_domain(),
      {true}, restart_trace);
  recording_store restart_store(fixture.expected, restart_trace);
  const auto historical =
      pkgstate::apply_adapter::read_historical_application_state(
          request, journal, restart_lease, restart_store);
  CHECK(restart_store.reads() == 1);
  CHECK(restart_trace ==
        std::vector<std::string>({"held", "read", "held", "held"}));
  CHECK(historical.state().identity() == fixture.expected.identity());
  CHECK(historical.projection().identity() == projected.projection().identity());
  CHECK(historical.projection().lease() == lease.identity());
  CHECK(historical.projection().evidence() == projected.projection().evidence());

  std::vector<std::string> current_trace;
  recording_lease current_lease(
      restart_lease.identity(), fixture.target.identity(),
      fixture.target.mutation_exclusion_domain(), {true}, current_trace);
  recording_store current_store(fixture.expected, current_trace);
  const auto current = pkgstate::apply_adapter::read_application_state(
      request, current_lease, current_store);
  CHECK(current.projection().identity() != historical.projection().identity());
  return 0;
}
