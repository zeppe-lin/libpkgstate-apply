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

## Release gate

The 3.0 adapter requires the extracted source-authority generation:
`libpkgsource` 3.0.0 and `libpkgsource-plan` 1.0.0. At the time of extraction,
published `libpkgapply` 2.3.0 still declares `libpkgsource-plan >=2.0.0` from
the former in-tree generation. That metadata must be corrected and a
compatible `libpkgapply` release published before `libpkgstate-apply` 3.0.0
can complete its cold dependency build. This repository does not patch or
silently weaken another authority boundary.

Release after `libpkgstate`, `libpkgstate-source`, and `libpkgstate-build`, and
only after the compatible `libpkgapply` tag is available.
