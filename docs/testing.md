# Testing

Qualification is layered so one successful application/state projection cannot
hide a damaged evidence seam, publication rule, package boundary, or test
harness.

The `unit` suite exercises adapter-owned typed errors and schema/value
semantics that require no package graph or canonical state store. The
`integration` suite uses real neighboring public APIs across planning,
completed application, lease-bound canonical state, state-build provenance, and
state publication. It separately qualifies installation, upgrade, removal, all
completed object classes, completed-evidence completeness, request/state/target
binding, package-state refusal, planner policy outcomes, lease-bound state
projection, state-projection refusals, and projection of the emitted
publication request back through `libpkgstate`.

The adapter is not a second planner or application validator. Planning owns
path decisions and incoming ownership intent; `libpkgapply` owns the validity
of completed application evidence; `libpkgstate-apply` owns admission of that
completed evidence against the retained plan and exact lease-bound prior state,
and translation into durable state publication authority. Integration tests pin
both sides of these seams rather than duplicating neighbor policy.

Completed-object tests cover regular, directory, symlink, FIFO, character
device, block device, socket, and other objects. They prove required mode,
owner, group, timestamp, content, symlink, and device facts are translated
exactly. Partial or fact-incomplete completed evidence is refused. A complete
regular object may retain an unknown hard-link relation and publish no
hard-link anchor; a known relation publishes the exact anchor.

Policy-sensitive tests cross a real planner decision into completed application
and durable publication. Retaining an already-compatible target object while
claiming ownership publishes `retained_existing`; staging rejected incoming
content while declining ownership does not invent an installed manifest entry.

Lease-bound state projection is tested independently from publication. The
canonical store is read exactly once under a caller-held mutation lease, and
the lease is revalidated around the read and before return. Target binding,
expected snapshot, ownership identity, and every required path-owner vector
must match.  Native store errors pass through as store errors rather than being
laundered into adapter vocabulary.

The `header` suite compiles the umbrella and each installed header
independently.  The `contract` suite owns architecture, documentation, release,
repository, extraction, CI, test-layout, style, ABI, and generated pkg-config
checks. Shared builds compare dynamic exports to
`abi/libpkgstate-apply.exports`, verify SONAME `3`, and audit direct
`DT_NEEDED` edges. Static builds prove the complete private pkg-config closure.

Fixtures contain deterministic neighboring authority construction only. Shared
assertion helpers live under `tests/support`; behavioral assertions remain in
individual unit or integration programs. Fixtures must not reimplement planner,
application, or state-publication policy.

Source contracts verify architecture placement, release metadata, repository
hygiene, CI coverage, style, test topology, and root-commit extraction
provenance. The provenance contract checks the root extraction against the
recorded `libpkgstate` 2.5.1 hashes; it intentionally does not freeze current
implementation files.

CI runs GCC and Clang in separate shared and static configurations, one
optimized release configuration, and ASan/UBSan configurations. Installation
qualification compiles a consumer against staged headers and metadata, checks
every installed header, audits the shared boundary or static archive, and
verifies installed manual and project documentation. The staged consumer
constructs both public adapter error models so static linkage must extract both
implementation translation units and resolve the complete private state-build,
state-plan, planner, and crypto closure; an address-only consumer is not
accepted.

The documentation contract receives the include roots of the production
dependencies resolved by Meson before Clang parses public headers. Ambient
system-installed zoo headers are not accepted as dependency closure.

A release candidate is incomplete until the exact dependency tags used by CI
exist and the whole matrix is green from clean build directories.
