// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

using namespace test_fixture;

namespace {

pkgapply::completed_object_fact
completed_object(const pkgplan::package_path& path,
                 pkgapply::completed_object_kind kind)
{
  using pkgapply::completed_device_number;
  using pkgapply::completed_hardlink_relation;
  using pkgapply::completed_object_timestamp;
  using pkgapply::completed_regular_content_identity;
  using pkgapply::qualified_fact;

  const bool regular = kind == pkgapply::completed_object_kind::regular;
  const bool symlink = kind == pkgapply::completed_object_kind::symlink;
  const bool device = kind == pkgapply::completed_object_kind::character_device ||
                      kind == pkgapply::completed_object_kind::block_device;

  return pkgapply::completed_object_fact(
      path,
      kind,
      qualified_fact<std::uint32_t>::known(0671),
      qualified_fact<std::uint64_t>::known(41),
      qualified_fact<std::uint64_t>::known(42),
      regular ? qualified_fact<std::uint64_t>::known(17)
              : qualified_fact<std::uint64_t>::not_applicable(),
      qualified_fact<completed_object_timestamp>::known({-13, 900000001}),
      regular
          ? qualified_fact<completed_regular_content_identity>::known(
                apply_identity<completed_regular_content_identity>(77))
          : qualified_fact<completed_regular_content_identity>::not_applicable(),
      symlink ? qualified_fact<std::string>::known("../target")
              : qualified_fact<std::string>::not_applicable(),
      device ? qualified_fact<completed_device_number>::known({12, 34})
             : qualified_fact<completed_device_number>::not_applicable(),
      regular ? qualified_fact<completed_hardlink_relation>::unknown()
              : qualified_fact<completed_hardlink_relation>::not_applicable(),
      pkgapply::object_fact_provenance::application_observation,
      pkgapply::object_fact_completeness::complete);
}

pkgapply::completed_application_evidence
evidence_with_after(const installation_fixture& fixture,
                    pkgapply::completed_object_fact object,
                    std::uint8_t seed)
{
  const auto& decision = fixture.plan.paths().front();
  return pkgapply::completed_application_evidence::installation(
      fixture.request,
      apply_identity<pkgapply::application_attempt_identity>(seed),
      fixture.projection.identity(),
      apply_identity<pkgapply::application_journal_identity>(seed + 1),
      {pkgapply::application_path_consequence(
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
          pkgapply::ownership_publication_status::eligible)},
      durability());
}

pkgstate::installed_object_metadata
project_object(pkgapply::completed_object_kind kind, std::uint8_t seed)
{
  installation_fixture fixture;
  const auto evidence = evidence_with_after(
      fixture, completed_object(fixture.plan.paths().front().path(), kind), seed);
  const auto publication =
      pkgstate::apply_adapter::project_completed_application(
          fixture.expected,
          fixture.projection,
          fixture.request,
          evidence,
          pkgstate::installation_reason::explicit_request());
  CHECK(publication.deltas().front().proposed_package().has_value());
  CHECK(publication.deltas().front().proposed_package()->manifest().size() == 1);
  return publication.deltas().front().proposed_package()->manifest().front().object();
}

} // namespace

int
main()
{
  struct case_type final {
    pkgapply::completed_object_kind source;
    pkgstate::owned_object_kind expected;
  };
  const std::vector<case_type> cases = {
      {pkgapply::completed_object_kind::regular,
       pkgstate::owned_object_kind::regular},
      {pkgapply::completed_object_kind::directory,
       pkgstate::owned_object_kind::directory},
      {pkgapply::completed_object_kind::symlink,
       pkgstate::owned_object_kind::symlink},
      {pkgapply::completed_object_kind::fifo,
       pkgstate::owned_object_kind::fifo},
      {pkgapply::completed_object_kind::character_device,
       pkgstate::owned_object_kind::character_device},
      {pkgapply::completed_object_kind::block_device,
       pkgstate::owned_object_kind::block_device},
      {pkgapply::completed_object_kind::socket,
       pkgstate::owned_object_kind::socket},
      {pkgapply::completed_object_kind::other,
       pkgstate::owned_object_kind::other},
  };

  std::uint8_t seed = 120;
  for (const auto& item : cases)
  {
    const auto object = project_object(item.source, seed);
    CHECK(object.kind() == item.expected);
    CHECK(object.mode() == 0671);
    CHECK(object.uid() == 41);
    CHECK(object.gid() == 42);
    CHECK(object.mtime().seconds() == -13);
    CHECK(object.mtime().nanoseconds() == 900000001);

    if (item.expected == pkgstate::owned_object_kind::regular)
    {
      CHECK(object.size().has_value() && *object.size() == 17);
      CHECK(object.regular_content().has_value());
      CHECK(!object.hardlink_anchor().has_value());
    }
    else
    {
      CHECK(!object.size().has_value());
      CHECK(!object.regular_content().has_value());
    }

    if (item.expected == pkgstate::owned_object_kind::symlink)
      CHECK(object.symlink_target().has_value() &&
            *object.symlink_target() == "../target");
    else
      CHECK(!object.symlink_target().has_value());

    if (item.expected == pkgstate::owned_object_kind::character_device ||
        item.expected == pkgstate::owned_object_kind::block_device)
    {
      CHECK(object.device().has_value());
      CHECK(object.device()->major() == 12);
      CHECK(object.device()->minor() == 34);
    }
    else
    {
      CHECK(!object.device().has_value());
    }
    ++seed;
  }

  installation_fixture fixture;
  const auto path = fixture.plan.paths().front().path();
  const auto anchor = pkgplan::package_path::parse("hardlink-anchor");
  const auto hardlink = pkgapply::completed_object_fact(
      path,
      pkgapply::completed_object_kind::regular,
      pkgapply::qualified_fact<std::uint32_t>::known(0755),
      pkgapply::qualified_fact<std::uint64_t>::known(0),
      pkgapply::qualified_fact<std::uint64_t>::known(0),
      pkgapply::qualified_fact<std::uint64_t>::known(4),
      pkgapply::qualified_fact<pkgapply::completed_object_timestamp>::known({10, 0}),
      pkgapply::qualified_fact<pkgapply::completed_regular_content_identity>::known(
          apply_identity<pkgapply::completed_regular_content_identity>(1)),
      pkgapply::qualified_fact<std::string>::not_applicable(),
      pkgapply::qualified_fact<pkgapply::completed_device_number>::not_applicable(),
      pkgapply::qualified_fact<pkgapply::completed_hardlink_relation>::known(
          pkgapply::completed_hardlink_relation(anchor)),
      pkgapply::object_fact_provenance::application_observation,
      pkgapply::object_fact_completeness::complete);
  const auto hardlink_evidence = evidence_with_after(fixture, hardlink, 150);
  const auto hardlink_publication =
      pkgstate::apply_adapter::project_completed_application(
          fixture.expected,
          fixture.projection,
          fixture.request,
          hardlink_evidence,
          pkgstate::installation_reason::explicit_request());
  const auto& hardlink_object = hardlink_publication.deltas().front()
                                    .proposed_package()->manifest().front().object();
  CHECK(hardlink_object.hardlink_anchor().has_value());
  CHECK(hardlink_object.hardlink_anchor()->string() == "hardlink-anchor");

  return 0;
}
