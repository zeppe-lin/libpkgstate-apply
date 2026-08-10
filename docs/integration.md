# Integration

`libpkgstate-apply` belongs at the composition point after application has
completed and before `libpkgstate` publication. It consumes the exact
operation request and completed evidence retained by `libpkgapply`; it does
not inspect mutable build directories, reconstruct incoming authority from
planner facts, acquire target leases, execute effects, or write the state
store.

The installed public API exposes only `libpkgstate` and `libpkgapply` types.
`libpkgstate-build`, `libpkgstate-plan`, `libpkgplan`, and `libcrypto` are
implementation dependencies. `libpkgstate-build` consumes the exact
`libpkgbuild-image` admission already retained by the application request and
owns the transitive source/build/image closure. This repository does not
redeclare or repeat those edges. They remain private for shared consumers and
are retained in the static link closure.

## Release order

The adapter uses `libpkgstate-plan` for the canonical durable-source to planner
control projection. It must not recreate that vocabulary locally.

Release after `libpkgstate`, `libpkgstate-source`, `libpkgstate-build`,
`libpkgstate-plan`, and the compatible `libpkgapply` 3.0 generation. Current
qualification uses the authenticated source/catalog and corrected planner
generations while retaining the proven public planner floor of 0.3.0.
