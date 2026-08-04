# Integration

`libpkgstate-apply` belongs at the composition point after application has
completed and before `libpkgstate` publication. It consumes the exact
operation request and completed evidence retained by `libpkgapply`; it does
not inspect mutable build directories, reconstruct incoming authority from
planner facts, acquire target leases, execute effects, or write the state
store.

The installed public API exposes only `libpkgstate` and `libpkgapply` types.
`libpkgstate-source`, `libpkgstate-build`, `libpkgplan`, `libpkgbuild`, and
`libcrypto` are implementation dependencies. They remain private for shared
consumers and are retained in the static link closure.

## Release order

The adapter uses `libpkgstate-plan` for the canonical durable-source to planner
control projection. It must not recreate that vocabulary locally.

Release after `libpkgstate`, `libpkgstate-source`, `libpkgstate-build`,
`libpkgstate-plan`, and the compatible `libpkgapply` 3.0 generation.
