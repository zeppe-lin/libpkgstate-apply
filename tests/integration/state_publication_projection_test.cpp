// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

#include "../fixtures/state.h"

#include <libpkgstate/publication_projection.h>

using namespace test_fixture;

int
main()
{
  {
    installation_fixture fixture;
    const auto request = pkgstate::apply_adapter::project_completed_application(
        fixture.expected,
        fixture.projection,
        fixture.request,
        fixture.evidence,
        pkgstate::installation_reason::explicit_request());
    const auto projected = pkgstate::project_publication_request(
        request, fixture.expected);
    const auto* installed = projected.find_package("tool");
    CHECK(installed != nullptr);
    CHECK(installed->release().version() == "1.0");
    const auto authority = state_build_authority(fixture.request.incoming());
    CHECK(installed->control().source() == authority.source());
    CHECK(installed->control().source().snapshot() ==
          authority.source().snapshot());
    CHECK(installed->manifest().size() == 1);
    CHECK(projected.owners(pkgstate::package_path::parse("tool")).size() == 1);
  }

  {
    upgrade_fixture fixture;
    const auto request = pkgstate::apply_adapter::project_completed_application(
        fixture.expected,
        fixture.projection,
        fixture.request,
        fixture.evidence);
    const auto projected = pkgstate::project_publication_request(
        request, fixture.expected);
    const auto* installed = projected.find_package("tool");
    CHECK(installed != nullptr);
    CHECK(installed->release().version() == "2.0");
    CHECK(installed->control().reason() == fixture.old_package.control().reason());
    CHECK(projected.owners(pkgstate::package_path::parse("tool")).size() == 1);
  }

  {
    removal_fixture fixture;
    const auto request = pkgstate::apply_adapter::project_completed_application(
        fixture.expected,
        fixture.projection,
        fixture.request,
        fixture.evidence);
    const auto projected = pkgstate::project_publication_request(
        request, fixture.expected);
    CHECK(projected.find_package("tool") == nullptr);
    CHECK(projected.owners(pkgstate::package_path::parse("tool")).empty());
  }

  return 0;
}
