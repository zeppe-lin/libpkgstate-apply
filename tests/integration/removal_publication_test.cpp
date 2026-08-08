// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

using namespace test_fixture;

int
main()
{
  removal_fixture fixture;
  const pkgstate::state_publication_request publication =
      pkgstate::apply_adapter::project_completed_application(
          fixture.expected,
          fixture.projection,
          fixture.request,
          fixture.evidence);

  CHECK(publication.expected_snapshot() == fixture.expected.identity());
  CHECK(publication.target_binding() == fixture.expected.target_binding());
  CHECK(publication.deltas().size() == 1);
  CHECK(!publication.transaction_evidence().has_value());

  const pkgstate::package_state_delta& delta = publication.deltas().front();
  CHECK(delta.kind() == pkgstate::package_state_delta_kind::remove);
  CHECK(delta.package_name() == "tool");
  CHECK(delta.expected_package().has_value());
  CHECK(*delta.expected_package() == fixture.old_package.identity());
  CHECK(!delta.proposed_package().has_value());
  CHECK(delta.operation_plan().string() == fixture.plan.identity().string());
  CHECK(delta.application_evidence().string() == fixture.evidence.identity().string());

  const auto transaction =
      state_identity<pkgstate::transaction_evidence_identity>(202);
  const pkgstate::state_publication_request composed =
      pkgstate::apply_adapter::project_completed_application(
          fixture.expected,
          fixture.projection,
          fixture.request,
          fixture.evidence,
          transaction);
  CHECK(composed.transaction_evidence().has_value());
  CHECK(*composed.transaction_evidence() == transaction);
  CHECK(composed.deltas().front().kind() ==
        pkgstate::package_state_delta_kind::remove);
  CHECK(!composed.deltas().front().proposed_package().has_value());
  return 0;
}
