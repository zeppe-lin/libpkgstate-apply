# History

## 3.1.2 (2026-08-19)

- Rebind completed-application and lease-bound state projection to the published
  `libpkgapply 4.x` append-only application authority generation.
- Keep `libpkgstate-apply.so.3` and the reviewed export surface after qualifying
  the public `lease_bound_application_state` carrier across application 3.0.1
  and 4.0.0; the embedded projection and complete adapter value preserve their
  x86-64 size and alignment.
- Require `libpkgapply >=4.0.0,<5.0.0` in Meson, pkg-config metadata, hosted
  qualification, documentation, and the shared-product dependency audit.
- Correct the stale shared-boundary audit from historical `libpkgapply.so.2` to
  the actual generation-4 owner dependency.

## 3.1.1 (2026-08-14)

- Bind private build projection to `libpkgstate-build >=3.1.0`, which closes
  through `libpkgstate-source.so.2` and source ABI 4.
- Move test authority to source 4, catalog 4, resolver 4, build 3.0.1,
  source-plan 2, and build-plan 1.1 so qualification cannot reload source 3.
- Require `libpkgapply >=3.0.1`, excluding application 3.0.0 whose
  build-plan interval could still admit the source-3 closure.
- Keep `libpkgstate-apply.so.3`; no public state-apply carrier changes.

## 3.1.0 (2026-08-12)

- Kept state projection strictly present-tense: `read_application_state()`
  observes canonical state only under the caller's current mutation lease.
- Removed restart-time historical projection reconstruction. Durable
  application journals now retain the exact admitted projection body in
  `libpkgapply`; this adapter does not manufacture old evidence from a current
  snapshot and historical lease identity.
- Removed the former historical-read API and its reconstruction-specific refusal
  categories. Missing or invalid historical projection evidence fails at the
  journal owner boundary instead.
- Aligned release qualification with `libpkgresolve` 3.0 and pinned the
  canonical `meson.options` and ATX Markdown source conventions.

## 3.0.0 (2026-08-04)

- Extracted `libpkgstate-apply` from the `libpkgstate` 2.5.1 repository.
- Preserved the extracted behavior as repository provenance and retained SONAME
  generation 3.
- Established a public closure of `libpkgstate >=3.0.0` and
  `libpkgapply >=3.0.0`, with state-build, state-plan, planner, and crypto
  edges private to the implementation and static closure.
- Added public-header, pkg-config, extraction-provenance, architecture,
  repository, compiler, sanitizer, shared, and static qualification.
- Completed the documented public projection contract under Doxygen
  warnings-as-errors.
- Renamed the installed manual from the former in-tree adapter name to
  `libpkgstate-apply.3`.
- Reused `libpkgstate-plan` as the sole durable source-control projection into
  planner vocabulary.
- Raised the application dependency to the repository-separated `libpkgapply`
  3.0 generation.
- Bound the adapter to the corrected repository-separated `libpkgapply` 3.0
  dependency graph.
- Removed duplicate source/build dependency edges; incoming state authority is
  now projected from the exact `libpkgbuild-image` admission retained by the
  application request.
- Corrected the transaction-evidence export spellings after the state identity
  ABI reset without widening the reviewed symbol surface.
- Updated source-authority fixtures to the syntax-neutral `libpkgsource` 3.0
  sealing contract.
- Qualified completed-application publication against the authenticated
  source/catalog and corrected planner generations, and pinned exact
  source-snapshot retention through final state projection.
