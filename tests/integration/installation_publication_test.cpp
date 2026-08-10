// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

#include <string>

using namespace test_fixture;

int
main()
{
  installation_fixture fixture;
  const pkgstate::state_publication_request publication =
      pkgstate::apply_adapter::project_completed_application(
          fixture.expected,
          fixture.projection,
          fixture.request,
          fixture.evidence,
          pkgstate::installation_reason::explicit_request());

  CHECK(publication.expected_snapshot() == fixture.expected.identity());
  CHECK(publication.target_binding() == fixture.expected.target_binding());
  CHECK(publication.deltas().size() == 1);
  CHECK(!publication.transaction_evidence().has_value());

  const pkgstate::package_state_delta& delta = publication.deltas().front();
  CHECK(delta.kind() == pkgstate::package_state_delta_kind::install);
  CHECK(delta.package_name() == "tool");
  CHECK(!delta.expected_package().has_value());
  CHECK(delta.proposed_package().has_value());
  CHECK(delta.operation_plan().string() == fixture.plan.identity().string());
  CHECK(delta.application_evidence().string() == fixture.evidence.identity().string());

  const pkgstate::installed_package& installed = *delta.proposed_package();
  CHECK(installed.release().name() == "tool");
  CHECK(installed.release().version() == "1.0");
  CHECK(installed.release().identity().string() ==
        fixture.plan.publication().release().identity().string());
  CHECK(installed.target_binding() == fixture.expected.target_binding());
  CHECK(installed.manifest().size() == 1);
  CHECK(installed.manifest().front().path().string() == "tool");
  CHECK(installed.manifest().front().kind() ==
        pkgstate::owned_object_kind::regular);
  CHECK(installed.manifest().front().origin() ==
        pkgstate::active_object_origin::incoming_payload);

  const pkgstate::installed_object_metadata& object =
      installed.manifest().front().object();
  CHECK(object.mode() == 0755);
  CHECK(object.uid() == 0);
  CHECK(object.gid() == 0);
  CHECK(object.size().has_value() && *object.size() == 4);
  CHECK(object.mtime().seconds() == 10);
  CHECK(object.mtime().nanoseconds() == 0);
  CHECK(object.regular_content().has_value());
  CHECK(!object.hardlink_anchor().has_value());

  const pkgstate::installed_control& control = installed.control();
  CHECK(control.source().runtime_requirements().size() == 1);
  CHECK(control.source().runtime_requirements().front().package().name() == "libc");
  CHECK(control.reason().kind() ==
        pkgstate::installation_reason_kind::explicit_request);
  const auto authority = state_build_authority(fixture.request.incoming());
  CHECK(control.source() == authority.source());
  CHECK(control.source().snapshot() == authority.source().snapshot());
  CHECK(control.source().snapshot().string() ==
        std::string("v1:sha256:") +
            fixture.request.incoming().projection().candidate().source_identity().hex());
  CHECK(control.build() == authority.provenance());
  CHECK(control.build().source_record() == control.source().identity());
  CHECK(control.build().artifact_content().string() ==
        fixture.plan.publication().artifact().string());
  CHECK(installed.receipt().operation_plan().string() ==
        fixture.plan.identity().string());
  CHECK(installed.receipt().application_evidence().string() ==
        fixture.evidence.identity().string());
  CHECK(!installed.receipt().transaction_evidence().has_value());

  const auto transaction =
      state_identity<pkgstate::transaction_evidence_identity>(200);
  const pkgstate::state_publication_request composed =
      pkgstate::apply_adapter::project_completed_application(
          fixture.expected,
          fixture.projection,
          fixture.request,
          fixture.evidence,
          pkgstate::installation_reason::explicit_request(),
          transaction);
  CHECK(composed.transaction_evidence().has_value());
  CHECK(*composed.transaction_evidence() == transaction);
  CHECK(composed.deltas().front().proposed_package().has_value());
  CHECK(composed.deltas().front().proposed_package()->receipt()
            .transaction_evidence().has_value());
  CHECK(*composed.deltas().front().proposed_package()->receipt()
             .transaction_evidence() == transaction);
  CHECK(composed.identity() != publication.identity());

  const pkgstate::state_publication_request repeated =
      pkgstate::apply_adapter::project_completed_application(
          fixture.expected,
          fixture.projection,
          fixture.request,
          fixture.evidence,
          pkgstate::installation_reason::explicit_request());
  CHECK(repeated.identity() == publication.identity());
  return 0;
}
