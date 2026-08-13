// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*!
 * \file adapter.h
 * \brief Projection of completed package application into installed state.
 */

#pragma once

#include <libpkgstate-apply/export.h>

#include <cstdint>
#include <stdexcept>
#include <string>

#include <libpkgapply/request.h>
#include <libpkgapply/result.h>
#include <libpkgapply/state_projection.h>
#include <libpkgstate/installed_control.h>
#include <libpkgstate/publication_request.h>
#include <libpkgstate/snapshot.h>

/*! \brief Application-state admission and publication projection. */
namespace pkgstate::apply_adapter {

/*! \brief Stable reason that completed application projection was refused. */
enum class projection_error_code : std::uint8_t {
  request_binding_mismatch = 1,     //!< Evidence names another request.
  operation_binding_mismatch = 2,   //!< Operation kind or package disagrees.
  plan_binding_mismatch = 3,        //!< Evidence names another accepted plan.
  target_binding_mismatch = 4,      //!< Managed target authorities disagree.
  state_projection_mismatch = 5,    //!< Lease-bound projection is unrelated.
  expected_state_mismatch = 6,      //!< Expected canonical state is unrelated.
  ownership_projection_mismatch = 7,//!< Path-owner universe is inconsistent.
  package_state_mismatch = 8,       //!< Required old package state disagrees.
  completed_path_mismatch = 9,      //!< Completed paths differ from the plan.
  identity_translation = 10,        //!< Foreign identity text was invalid.
  path_translation = 11,            //!< Planner/application paths were invalid.
  incoming_authority_mismatch = 12, //!< Incoming build authority disagrees.
  publication_construction = 13,    //!< Native publication invariants refused.
};

/*! \brief Typed completed-application projection failure. */
class PKGSTATE_APPLY_API projection_error final : public std::invalid_argument {
public:
  /*!
   * \brief Construct a typed projection failure.
   * \param code Stable refusal category.
   * \param message Human-readable diagnostic text.
   */
  projection_error(projection_error_code code, std::string message);

  /*! \brief Destroy the polymorphic projection failure. */
  ~projection_error() override;

  /*!
   * \brief Return the stable refusal category.
   * \return The stable refusal category.
   */
  [[nodiscard]] projection_error_code code() const noexcept;

private:
  projection_error_code code_;
};

/*!
 * \brief Construct installation publication from completed native application.
 *
 * Incoming source and build authority are taken only from the exact
 * libpkgbuild-image admission retained by \p request. That authority is
 * projected through libpkgstate-build. The function binds completed object
 * truth, accepted plan authority, the lease-bound state projection, and the
 * caller-selected initial installation reason.
 *
 * \param expected_state Exact canonical state admitted before application.
 * \param application_state Matching lease-bound application path projection.
 * \param request Exact installation request named by completed evidence.
 * \param evidence Complete immutable application completion evidence.
 * \param reason Exact initial installation reason.
 * \return One immutable installation publication request.
 * \throws projection_error when any request, plan, target, state, ownership,
 * path, incoming authority, or publication invariant disagrees.
 */
[[nodiscard]] PKGSTATE_APPLY_API state_publication_request
project_completed_application(
    const snapshot& expected_state,
    const pkgapply::lease_bound_state_projection& application_state,
    const pkgapply::installation_application_request& request,
    const pkgapply::completed_application_evidence& evidence,
    installation_reason reason);

/*!
 * \brief Construct installation publication with transaction evidence.
 * \param expected_state Exact canonical state admitted before application.
 * \param application_state Matching lease-bound application path projection.
 * \param request Exact installation request named by completed evidence.
 * \param evidence Complete immutable application completion evidence.
 * \param reason Exact initial installation reason.
 * \param transaction_evidence Exact orchestration-owned transaction identity.
 * \return One immutable installation publication request retaining transaction
 * evidence in both receipt and publication authority.
 * \throws projection_error when any immutable authority disagrees.
 */
[[nodiscard]] PKGSTATE_APPLY_API state_publication_request
project_completed_application(
    const snapshot& expected_state,
    const pkgapply::lease_bound_state_projection& application_state,
    const pkgapply::installation_application_request& request,
    const pkgapply::completed_application_evidence& evidence,
    installation_reason reason,
    transaction_evidence_identity transaction_evidence);

/*!
 * \brief Construct replacement publication from completed native application.
 *
 * The prior installed package's reason is retained. Incoming source and build
 * authority are taken only from the exact build-image admission named by the
 * request and completed evidence.
 *
 * \param expected_state Exact canonical state admitted before application.
 * \param application_state Matching lease-bound application path projection.
 * \param request Exact upgrade request named by completed evidence.
 * \param evidence Complete immutable application completion evidence.
 * \return One immutable replacement publication request.
 * \throws projection_error when any immutable authority disagrees.
 */
[[nodiscard]] PKGSTATE_APPLY_API state_publication_request
project_completed_application(
    const snapshot& expected_state,
    const pkgapply::lease_bound_state_projection& application_state,
    const pkgapply::upgrade_application_request& request,
    const pkgapply::completed_application_evidence& evidence);

/*!
 * \brief Construct replacement publication with transaction evidence.
 * \param expected_state Exact canonical state admitted before application.
 * \param application_state Matching lease-bound application path projection.
 * \param request Exact upgrade request named by completed evidence.
 * \param evidence Complete immutable application completion evidence.
 * \param transaction_evidence Exact orchestration-owned transaction identity.
 * \return One immutable replacement publication request retaining transaction
 * evidence in both receipt and publication authority.
 * \throws projection_error when any immutable authority disagrees.
 */
[[nodiscard]] PKGSTATE_APPLY_API state_publication_request
project_completed_application(
    const snapshot& expected_state,
    const pkgapply::lease_bound_state_projection& application_state,
    const pkgapply::upgrade_application_request& request,
    const pkgapply::completed_application_evidence& evidence,
    transaction_evidence_identity transaction_evidence);

/*!
 * \brief Construct removal publication from completed native application.
 * \param expected_state Exact canonical state admitted before application.
 * \param application_state Matching lease-bound application path projection.
 * \param request Exact removal request named by completed evidence.
 * \param evidence Complete immutable application completion evidence.
 * \return One immutable removal publication request.
 * \throws projection_error when any immutable authority disagrees.
 */
[[nodiscard]] PKGSTATE_APPLY_API state_publication_request
project_completed_application(
    const snapshot& expected_state,
    const pkgapply::lease_bound_state_projection& application_state,
    const pkgapply::removal_application_request& request,
    const pkgapply::completed_application_evidence& evidence);

/*!
 * \brief Construct removal publication with transaction evidence.
 * \param expected_state Exact canonical state admitted before application.
 * \param application_state Matching lease-bound application path projection.
 * \param request Exact removal request named by completed evidence.
 * \param evidence Complete immutable application completion evidence.
 * \param transaction_evidence Exact orchestration-owned transaction identity.
 * \return One immutable removal publication request retaining transaction
 * evidence on the composed publication request.
 * \throws projection_error when any immutable authority disagrees.
 */
[[nodiscard]] PKGSTATE_APPLY_API state_publication_request
project_completed_application(
    const snapshot& expected_state,
    const pkgapply::lease_bound_state_projection& application_state,
    const pkgapply::removal_application_request& request,
    const pkgapply::completed_application_evidence& evidence,
    transaction_evidence_identity transaction_evidence);

} // namespace pkgstate::apply_adapter
