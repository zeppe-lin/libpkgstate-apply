// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

using namespace test_fixture;

int
main()
{
  upgrade_fixture fixture;
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
  CHECK(delta.kind() == pkgstate::package_state_delta_kind::replace);
  CHECK(delta.expected_package().has_value());
  CHECK(*delta.expected_package() == fixture.old_package.identity());
  CHECK(delta.proposed_package().has_value());
  CHECK(delta.proposed_package()->release().version() == "2.0");
  CHECK(delta.proposed_package()->identity() != fixture.old_package.identity());
  CHECK(delta.proposed_package()->control().reason() ==
        fixture.old_package.control().reason());
  CHECK(!delta.proposed_package()->receipt().transaction_evidence().has_value());
  CHECK(delta.operation_plan().string() == fixture.plan.identity().string());
  CHECK(delta.application_evidence().string() == fixture.evidence.identity().string());

  const auto transaction =
      state_identity<pkgstate::transaction_evidence_identity>(201);
  const pkgstate::state_publication_request composed =
      pkgstate::apply_adapter::project_completed_application(
          fixture.expected,
          fixture.projection,
          fixture.request,
          fixture.evidence,
          transaction);
  CHECK(composed.transaction_evidence().has_value());
  CHECK(*composed.transaction_evidence() == transaction);
  CHECK(composed.deltas().front().proposed_package().has_value());
  CHECK(composed.deltas().front().proposed_package()->receipt()
            .transaction_evidence().has_value());
  CHECK(*composed.deltas().front().proposed_package()->receipt()
             .transaction_evidence() == transaction);
  return 0;
}
