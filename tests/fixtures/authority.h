// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "../support/test.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <libpkgapply/incoming_package.h>
#include <libpkgbuild-image/libpkgbuild-image.h>
#include <libpkgbuild-plan/libpkgbuild-plan.h>
#include <libpkgcatalog/libpkgcatalog.h>
#include <libpkgresolve/libpkgresolve.h>
#include <libpkgapply/object_fact.h>
#include <libpkgapply/path_consequence.h>
#include <libpkgapply/request.h>
#include <libpkgapply/result.h>
#include <libpkgapply/state_projection.h>
#include <libpkgapply/target_context.h>
#include <libpkgapply/execution_control.h>
#include <libpkgimage/inspection_receipt.h>
#include <libpkgimage/package_entry.h>
#include <libpkgimage/package_image.h>
#include <libpkgplan/install.h>
#include <libpkgplan/remove.h>
#include <libpkgplan/upgrade.h>
#include <libpkgstate-apply/adapter.h>
#include <libpkgstate-apply/state_projection.h>
#include <libpkgstate/canonical_store.h>
#include <libpkgstate-build/adapter.h>
#include <libpkgstate-source/adapter.h>
#include <libpkgstate/installed_control.h>
#include <libpkgstate/installed_package.h>
#include <libpkgstate/package_release.h>
#include <libpkgstate/snapshot.h>
#include <libpkgstate/state_target_binding.h>

namespace test_fixture {

template<typename Identity>
Identity
state_identity(std::uint8_t value)
{
  pkgstate::sha256_digest_bytes bytes{};
  bytes.fill(value);
  return Identity::from_sha256(bytes);
}

template<typename Identity>
Identity
plan_identity(std::uint8_t value)
{
  pkgplan::sha256_digest_bytes bytes{};
  bytes.fill(value);
  return Identity::from_sha256(bytes);
}

template<typename Identity>
Identity
apply_identity(std::uint8_t value)
{
  std::string text = "v1:sha256:";
  constexpr char hex[] = "0123456789abcdef";
  for (std::size_t index = 0; index < 32; ++index)
  {
    const std::uint8_t byte =
        static_cast<std::uint8_t>(value + index);
    text.push_back(hex[(byte >> 4) & 0x0f]);
    text.push_back(hex[byte & 0x0f]);
  }
  return Identity::parse(text);
}

template<typename Destination, typename Source>
Destination
translate_identity(const Source& source)
{
  return Destination::parse(source.string());
}

pkgstate::state_target_binding
state_target(std::uint8_t seed = 1)
{
  return pkgstate::state_target_binding::make(
      state_identity<pkgstate::managed_target_identity>(seed),
      state_identity<pkgstate::state_store_identity>(seed + 1),
      state_identity<pkgstate::root_view_identity>(seed + 2),
      state_identity<pkgstate::state_backend_identity>(seed + 3),
      state_identity<pkgstate::publication_domain_identity>(seed + 4));
}

pkgsource::source_snapshot
source_snapshot(const char* version,
                const char* removal_program = "prepare-new-remove")
{
  using namespace pkgsource;
  return seal_source(
      source_origin("recipe.yml"),
      recipe_declaration(
          package_release(package_reference("tool"), version, 1),
          package_metadata("Tool", std::nullopt, std::nullopt,
                           {"GPL-3.0-or-later"}),
          {},
          program(program_language::posix_shell,
                  "install -m755 tool \"$PKG/tool\"\n"),
          {requirement_declaration(
              requirement_scope::run(),
              requirement_subject(package_reference("libc")),
              declaration_provenance(
                  "recipe.yml", "requirements.run[0]", 10, 3))},
          {lifecycle_program(
              lifecycle_action::pre_remove,
              program(program_language::posix_shell, removal_program))},
          architecture_requirements(
              {architecture_reference("x86_64")},
              {architecture_reference("x86_64")}),
          declaration_provenance("recipe.yml", "$", 1, 1)),
      profile_catalog::seal({}));
}

pkgstate::package_source_record
state_source(const char* version, std::uint8_t,
             const char* removal_program = "prepare-new-remove")
{
  const pkgsource::source_snapshot source =
      source_snapshot(version, removal_program);
  return pkgstate::source_adapter::project_source(
      source, pkgsource::architecture_reference("x86_64"),
      pkgsource::architecture_reference("x86_64"));
}

pkgstate::installed_package
state_package(const pkgstate::state_target_binding& target,
              const char* version,
              std::vector<pkgstate::owned_entry> manifest)
{
  const std::uint8_t release_seed = 69;
  pkgstate::package_source_record source =
      state_source(version, release_seed, "finish-old-remove");
  pkgstate::installed_control control = pkgstate::installed_control::make(
      source,
      pkgstate::installation_reason::runtime_dependency(
          pkgstate::package_reference("base-system")),
      pkgstate::build_provenance(
          source.identity(),
          state_identity<pkgstate::build_request_identity>(90),
          state_identity<pkgstate::build_input_set_identity>(92),
          state_identity<pkgstate::environment_policy_identity>(93),
          state_identity<pkgstate::build_policy_identity>(94),
          state_identity<pkgstate::build_result_identity>(95),
          state_identity<pkgstate::payload_manifest_identity>(96),
          state_identity<pkgstate::build_artifact_identity>(97),
          state_identity<pkgstate::artifact_content_identity>(98),
          state_identity<pkgstate::artifact_binding_identity>(99),
          state_identity<pkgstate::execution_evidence_identity>(100),
          state_identity<pkgstate::build_image_identity>(101),
          state_identity<pkgstate::artifact_image_identity>(102),
          state_identity<pkgstate::artifact_inspection_identity>(103)));
  return pkgstate::installed_package::make(
      pkgstate::installation_receipt::make(
          std::move(control), target, std::move(manifest),
          state_identity<pkgstate::operation_plan_identity>(96),
          state_identity<pkgstate::application_evidence_identity>(97)));
}

pkgstate::installed_object_metadata
state_regular(std::uint8_t content)
{
  return pkgstate::installed_object_metadata(
      pkgstate::owned_object_kind::regular, 0755, 0, 0,
      pkgstate::installed_object_timestamp(10, 0), std::uint64_t{4},
      state_identity<pkgstate::installed_regular_content_identity>(content));
}

pkgplan::filesystem_object_metadata
planner_regular(std::uint8_t content)
{
  return pkgplan::filesystem_object_metadata(
      pkgplan::filesystem_object_kind::regular,
      0755,
      0,
      0,
      4,
      pkgplan::object_timestamp(10, 0),
      plan_identity<pkgplan::filesystem_regular_content_identity>(content));
}

pkgplan::installed_control_projection
planner_control(const pkgstate::installed_control& source)
{
  std::vector<pkgplan::runtime_dependency_declaration> dependencies;
  for (const auto& item : source.source().runtime_requirements())
    dependencies.push_back(
        pkgplan::runtime_dependency_declaration::make(
            item.package().name()));

  std::vector<pkgplan::removal_lifecycle_declaration> lifecycle;
  for (const pkgstate::lifecycle_action action : {
           pkgstate::lifecycle_action::pre_remove,
           pkgstate::lifecycle_action::post_remove})
  {
    const pkgstate::lifecycle_program* item = source.source().lifecycle(action);
    if (item == nullptr)
      continue;
    lifecycle.push_back(pkgplan::removal_lifecycle_declaration::make(
        action == pkgstate::lifecycle_action::pre_remove
            ? pkgplan::removal_lifecycle_phase::pre_remove
            : pkgplan::removal_lifecycle_phase::post_remove,
        "text/x-posix-shell", item->value().material()));
  }

  pkgplan::installed_control_completeness completeness;
  completeness.runtime_dependencies =
      pkgplan::control_fact_availability::known;
  completeness.removal_lifecycle =
      pkgplan::control_fact_availability::known;
  completeness.target_profile =
      pkgplan::control_fact_availability::known;
  return pkgplan::installed_control_projection(
      completeness, std::move(dependencies), std::move(lifecycle),
      {pkgplan::target_profile_fact::make(
          "pkgsource.target-architectures", "x86_64")});
}

pkgimage::inspected_package_image
incoming_image(std::uint8_t content)
{
  pkgimage::package_entry entry(
      pkgimage::package_path::parse("tool"),
      pkgimage::entry_type::regular);
  entry.mode = 0755;
  entry.uid = 0;
  entry.gid = 0;
  entry.size = 4;
  entry.mtime = 10;
  entry.mtime_nanoseconds = 0;
  pkgimage::sha256_digest_bytes content_bytes{};
  content_bytes.fill(content);
  entry.regular_content =
      pkgimage::regular_content_digest::from_sha256(content_bytes);

  pkgimage::package_image image({entry});
  pkgimage::sha256_digest_bytes archive_bytes{};
  archive_bytes.fill(static_cast<std::uint8_t>(content + 30));
  pkgimage::archive_inspection_receipt receipt(
      pkgimage::archive_backend_identity::parse("test/pkgstate-apply-v1"),
      pkgimage::complete_archive_digest::from_sha256(archive_bytes),
      image.identity(),
      image.size());
  return pkgimage::inspected_package_image(
      std::move(image), std::move(receipt));
}

std::string
byte_digest(std::uint8_t byte)
{
  constexpr char hex[] = "0123456789abcdef";
  std::string result;
  result.reserve(64);
  for (std::size_t index = 0; index < 32; ++index)
  {
    result.push_back(hex[(byte >> 4) & 0x0f]);
    result.push_back(hex[byte & 0x0f]);
  }
  return result;
}

pkgsource::source_snapshot
dependency_snapshot(const char* name)
{
  using namespace pkgsource;
  return seal_source(
      source_origin(std::string(name) + "/recipe.yml"),
      recipe_declaration(
          package_release(package_reference(name), "1.0", 1),
          package_metadata(name, std::nullopt, std::nullopt,
                           {"GPL-3.0-or-later"}),
          {},
          program(program_language::posix_shell, "install -d \"$PKG\"\n"),
          {}, {},
          architecture_requirements(
              {architecture_reference("x86_64")},
              {architecture_reference("x86_64")}),
          declaration_provenance("recipe.yml", "$", 1, 1)),
      profile_catalog::seal({}));
}

pkgresolve::resolution_result
resolution(const char* version)
{
  auto profiles = pkgsource::profile_catalog::seal({});
  std::vector<pkgsource::source_snapshot> sources;
  sources.push_back(source_snapshot(version));
  sources.push_back(dependency_snapshot("libc"));
  pkgcatalog::collection_declaration declaration(
      pkgcatalog::collection_reference("core"),
      pkgcatalog::collection_provenance(
          "/collections/core", std::nullopt,
          pkgsource::declaration_provenance(
              "catalog.yml", "collections[0]", 1, 1)),
      std::move(sources));
  std::vector<pkgcatalog::catalog_collection> collections;
  collections.emplace_back(
      0, pkgcatalog::seal_collection(std::move(declaration)));
  auto catalog = pkgcatalog::catalog_snapshot::seal(
      std::move(profiles), std::move(collections));

  std::vector<pkgresolve::resolution_goal> goals;
  goals.emplace_back(
      pkgsource::requirement_scope::build(),
      pkgsource::requirement_subject(pkgsource::package_reference("tool")),
      "build-tool");
  goals.emplace_back(
      pkgsource::requirement_scope::check(),
      pkgsource::requirement_subject(pkgsource::package_reference("tool")),
      "check-tool");
  return pkgresolve::resolve(pkgresolve::resolution_request::seal(
      std::move(catalog), pkgstate::snapshot::make(state_target(40)),
      pkgresolve::architecture_context(
          pkgsource::architecture_reference("x86_64"),
          pkgsource::architecture_reference("x86_64")),
      std::move(goals), pkgresolve::resolution_policy()));
}

const pkgresolve::selected_package&
resolved_subject(const pkgresolve::resolution_result& resolved)
{
  for (const auto& selection : resolved.selections())
  {
    if (selection.environment() == pkgresolve::resolution_environment::target &&
        selection.package().name() == "tool")
      return selection;
  }
  throw std::runtime_error("fixture resolution lacks tool subject");
}

std::string
sha256_hex(const std::string& value)
{
  constexpr std::string_view prefix = "v1:sha256:";
  if (value.compare(0, prefix.size(), prefix) != 0)
    throw std::invalid_argument("fixture digest is not canonical SHA-256");
  return value.substr(prefix.size());
}

pkgapply::incoming_package_authority
incoming_authority(const char* version, std::uint8_t content)
{
  auto resolved = resolution(version);
  const pkgbuild::build_request request = pkgbuild::build_request::seal(
      resolved, resolved_subject(resolved).identity(),
      pkgbuild::build_policy::make(
          pkgbuild::environment_policy::hermetic(1, 0022, 1700000000)));
  const pkgbuild::payload_manifest payload = pkgbuild::payload_manifest::seal({
      pkgbuild::payload_entry::regular(
          pkgbuild::payload_path::parse("tool"), 0755, 0, 0, 4,
          pkgbuild::payload_time{10, 0},
          pkgbuild::sha256_digest(byte_digest(content))),
  });
  pkgimage::inspected_package_image image = incoming_image(content);
  const pkgbuild::sealed_artifact artifact = pkgbuild::sealed_artifact::make(
      pkgbuild::artifact_encoding::package_tar,
      pkgbuild::artifact_compression::none, 4,
      pkgbuild::sha256_digest(
          sha256_hex(image.receipt().archive_digest().string())));
  pkgbuild::build_result result = pkgbuild::build_result::succeeded(
      request, payload, artifact,
      pkgbuild::execution_evidence_identity::from_sha256(
          byte_digest(static_cast<std::uint8_t>(content + 60))));
  auto admitted = pkgbuild::image_adapter::build_image_authority::admit(
      std::move(result), std::move(image));
  return pkgapply::incoming_package_authority::admit(
      pkgbuild::plan_adapter::project_artifact(admitted));
}

pkgstate::build_adapter::build_authority
state_build_authority(const pkgapply::incoming_package_authority& incoming)
{
  return pkgstate::build_adapter::project_build(incoming.authority());
}

pkgplan::package_policy_snapshot
policy()
{
  return pkgplan::package_policy_snapshot(
      plan_identity<pkgplan::policy_snapshot_identity>(50),
      pkgplan::normalized_path_policy(
          pkgplan::incoming_path_policy::activate(),
          pkgplan::obsolete_path_policy::remove(),
          pkgplan::shared_ownership_policy::forbid,
          pkgplan::directory_cleanup_policy::remove_if_empty),
      {});
}

} // namespace test_fixture
