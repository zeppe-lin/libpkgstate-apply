# libpkgstate-apply

libpkgstate-apply provides lease-bound state projection and
completed-application admission.

```text
lease-bound canonical state + completed application -> state_publication_request
```

## Authority

This repository owns the composition seam between request-bound `libpkgapply`
evidence and state-owned publication authority. It is a translation boundary,
not another semantic owner. Its input and output models remain authoritative in
their respective repositories.

The state read accepts a caller-owned target lease and reports only current
canonical truth observed under that lease. Historical application projection
evidence is owned durably by the `libpkgapply` application journal; this adapter
never re-derives an old projection from current state plus historical identity.
The publication adapter then accepts the exact operation request and completed
application evidence. A successful projection retains the exact
expected state epoch, path-owner projection, request-bound build-image
authority, complete object consequences, optional transaction evidence, and one
immutable publication request.

The adapter performs no discovery, parsing, dependency resolution, build
execution, archive inspection, target mutation, state publication, migration,
retry policy, or compatibility import unless the operation is explicitly part
of the contract above. It exports refusal rather than guessing. It refuses
lease, request, plan, target, state, ownership, package, path, incoming
authority, identity, or publication construction mismatch.

See `docs/architecture.md` for invariants and `docs/integration.md` for
placement in the package-management graph.

## Dependency boundary

Public installed closure: `libpkgstate >=3.0.0` and `libpkgapply >=3.0.0`.

Private implementation closure: `libpkgstate-build >=3.0.0`,
`libpkgstate-plan >=3.0.0`, `libpkgplan >=0.3.0`, and `libcrypto`.

Fallback subprojects are intentionally unsupported. Shared consumers receive
only public requirements; static consumers receive the complete private closure
through pkg-config.

## Build

```sh
meson setup build-shared \
  -Ddefault_library=shared \
  -Dlink_mode=shared
meson compile -C build-shared
meson test -C build-shared --print-errorlogs

meson setup build-static \
  -Ddefault_library=static \
  -Dlink_mode=static
meson compile -C build-static
meson test -C build-static --print-errorlogs
```

Shared and static artifacts must come from separate build directories.
`default_library=both` is rejected because one dependency closure cannot
truthfully represent both linkage modes.

## Release lineage

The 3.0 repository was extracted from `libpkgstate` 2.5.1. The repository root
preserves extraction provenance; later commits may evolve the independent
product without rewriting that history. The library preserves SONAME generation
3.

Release after the state core, state build and plan bridges, and the
repository-separated `libpkgapply` 3.0 generation.

## Documentation

- `docs/architecture.md` — authority and refusal invariants;
- `docs/integration.md` — composition and release order;
- `docs/testing.md` — qualification matrix;
- `docs/abi.md` — ABI and pkg-config policy;
- `man/libpkgstate-apply.3.scdoc` — installed `libpkgstate-apply.3` interface
  manual;
- `MAINTAINING.md` — release gate.

## License

GPL-3.0-or-later. See `COPYING` and `COPYRIGHT`.
