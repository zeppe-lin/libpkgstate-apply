# Architecture

## Authority flow

Contract shorthand: `lease-bound canonical state + completed application -> state_publication_request`.

```text
caller-held target mutation lease
+ exact package application request
+ canonical store
                    |
                    v
lease-bound application-state projection
                    |
completed application evidence
+ exact operation-specific request
                    |
                    v
state_publication_request
```

`libpkgstate-apply` is the composition boundary between completed filesystem
application and durable state publication. It neither performs the application
nor writes the store.

## Lease-bound read invariants

`read_application_state()` accepts a caller-owned mutation lease. It proves the
lease is live and bound to the request target and exclusion domain, reads the
canonical store exactly once, and verifies the returned snapshot is the epoch
admitted by the request. It projects the accepted plan's exact path universe
and current owners, derives canonical projection evidence, and checks the lease
again before return. The returned value owns both snapshot and projection so a
caller cannot pair facts from different epochs.

The function does not acquire or renew a lease. Lease loss, foreign ownership,
target drift, state drift, incomplete ownership, or identity translation
failure is exported as a typed refusal. Native store failures remain native
`store_error` failures.

## Completed-application admission

`project_completed_application()` checks request, operation, plan, target,
state, ownership, package, and completed-path bindings. For installation and
upgrade, incoming source and build authority are taken only from the exact
`incoming_package_authority` retained by the application request. The adapter
projects that sealed source through `libpkgstate-source` and admits the retained
build result and inspected image through `libpkgstate-build`; it accepts no
parallel caller-supplied authority.

Installation receives one typed initial reason. Upgrade preserves the existing
reason. Removal accepts neither incoming package authority nor a replacement
reason. Optional overloads retain exact transaction evidence in both the
publication request and the installation receipt.

A successful call returns a `state_publication_request`. Compare-and-publish,
storage encoding, generation creation, and publication receipts remain owned by
`libpkgstate`.

## Dependency placement

Installed declarations expose only `libpkgstate` and `libpkgapply` types. The
source-admission, build-admission, planner, build-model, and crypto edges are
implementation-only: private for shared consumers and present in the static
closure.

The repository was seeded from the two 2.5.1 implementation bodies. The root
extraction remains provenance; subsequent changes may refine this boundary
without pretending the historical body is still current.
