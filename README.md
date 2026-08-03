# libpkgstate-apply

`libpkgstate-apply` provides lease-bound application-state projection and completed-application admission.

```text
lease-bound canonical state + completed application -> state_publication_request
```

It is a translation boundary, not a second authority. It performs no source discovery, dependency resolution, build or application execution, target mutation, state publication, migration, or compatibility import beyond the exact operation documented in `docs/architecture.md`.

The 3.0 repository was extracted from `libpkgstate` 2.5.1 without rewriting the implementation body.

## Build

```sh
meson setup build -Ddefault_library=shared -Dlink_mode=shared
meson compile -C build
meson test -C build --print-errorlogs
```

Fallback subprojects are intentionally unsupported. Shared and static closures use separate build directories.

The current upstream-generation compatibility gate is recorded in `docs/integration.md`; it must be closed before tagging 3.0.0.

## License

GPL-3.0-or-later. See `COPYING` and `COPYRIGHT`.
