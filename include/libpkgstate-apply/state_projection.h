// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later

/*!
 * \file state_projection.h
 * \brief Lease-bound projection of canonical installed state into libpkgapply.
 */
#pragma once

#include <libpkgstate-apply/export.h>

#include <cstdint>
#include <stdexcept>
#include <string>

#include <libpkgapply/mutation_lease.h>
#include <libpkgapply/request.h>
#include <libpkgapply/state_projection.h>
#include <libpkgstate/canonical_store.h>
#include <libpkgstate/snapshot.h>

/*! \brief Application-state admission and publication projection. */
namespace pkgstate::apply_adapter {

/*! \brief Canonical evidence schema for one lease-bound state read. */
inline constexpr std::uint16_t
    application_state_projection_evidence_schema_version = 1;

/*! \brief Stable reason that lease-bound state projection was refused. */
enum class application_state_projection_error_code : std::uint8_t {
  request_binding_mismatch = 1,    //!< Request authorities disagree internally.
  lease_not_held = 2,              //!< Caller did not hold the supplied lease.
  lease_lost = 3,                  //!< Lease was lost during the state read.
  lease_target_mismatch = 4,       //!< Lease belongs to another managed target.
  lease_domain_mismatch = 5,       //!< Lease belongs to another exclusion domain.
  target_binding_mismatch = 6,     //!< Store target differs from request target.
  expected_snapshot_mismatch = 7,  //!< Store epoch differs from admitted state.
  ownership_inventory_mismatch = 8,//!< Ownership identity differs from request.
  path_owners_mismatch = 9,        //!< Exact accepted-plan owners disagree.
  identity_translation = 10,       //!< Foreign identity text was invalid.
  path_translation = 11,           //!< Planner and state path vocabularies differ.
  evidence_construction = 12,      //!< Application projection refused evidence.
};

/*! \brief Invalid authority universe for a lease-bound application-state read. */
class PKGSTATE_APPLY_API application_state_projection_error final
    : public std::invalid_argument {
public:
  /*!
   * \brief Construct a typed state-projection failure.
   * \param code Stable refusal category.
   * \param message Human-readable diagnostic text.
   */
  application_state_projection_error(
      application_state_projection_error_code code,
      std::string message);

  /*! \brief Destroy the polymorphic state-projection failure. */
  ~application_state_projection_error() override;

  /*!
   * \brief Return the stable refusal category.
  *  \return The stable refusal category.
   */
  [[nodiscard]] application_state_projection_error_code code() const noexcept;

private:
  application_state_projection_error_code code_;
};

/*!
 * \brief One exact canonical snapshot and its application path projection.
 *
 * The value owns both objects so a caller cannot pair a path-owner projection
 * with another state epoch when constructing an effect driver.
 */
class PKGSTATE_APPLY_API lease_bound_application_state final {
public:
  /*!
   * \brief Return the exact canonical snapshot read under the lease.
  *  \return The exact canonical snapshot read under the lease.
   */
  [[nodiscard]] const snapshot& state() const noexcept;
  /*!
   * \brief Return the matching application-owned path-owner projection.
  *  \return The matching application-owned path-owner projection.
   */
  [[nodiscard]] const pkgapply::lease_bound_state_projection&
  projection() const noexcept;

private:
  /*! \brief Construct through read_application_state(). */
  friend PKGSTATE_APPLY_API lease_bound_application_state
  read_application_state(const pkgapply::package_application_request&,
                         const pkgapply::target_mutation_lease&,
                         const canonical_store&);

  lease_bound_application_state(
      snapshot state,
      pkgapply::lease_bound_state_projection projection);

  snapshot state_;
  pkgapply::lease_bound_state_projection projection_;
};

/*!
 * \brief Read canonical state while one caller-owned target lease is held.
 *
 * The function validates request/lease authority, performs exactly one store
 * read, validates that the returned state is the epoch admitted by the
 * application request, projects the accepted plan's exact path-owner universe,
 * and binds deterministic projection evidence to the live lease acquisition.
 * The lease is checked before and after the read and before return.
 *
 * The function does not acquire a lease, initialize a store, observe the target
 * filesystem, execute application, publish state, reconcile, retry, or repair.
 *
 * \param request Exact operation-specific application request.
 * \param lease Caller-owned target mutation lease.
 * \param store Canonical state store bound to the same target authority.
 * \return Snapshot and matching application projection as one owned value.
 * \throws application_state_projection_error when immutable authorities,
 * lease state, path ownership, or foreign representations disagree.
 * \throws store_error when the canonical store cannot be read.
 */
[[nodiscard]] PKGSTATE_APPLY_API lease_bound_application_state
read_application_state(const pkgapply::package_application_request& request,
                       const pkgapply::target_mutation_lease& lease,
                       const canonical_store& store);

} // namespace pkgstate::apply_adapter
