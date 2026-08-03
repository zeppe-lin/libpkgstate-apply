# Architecture

## Authority flow

```text
lease-bound canonical state + completed application -> state_publication_request
```

`libpkgstate-apply` owns only this translation. It does not own either endpoint's semantic model, perform ambient discovery, execute effects, or publish state.

The implementation body and focused behavior test are extracted unchanged from `libpkgstate` 2.5.1. Repository extraction changes dependency direction and release ownership, not admission semantics.

## Dependency placement

Public dependencies are those whose types occur in installed headers. Implementation-only dependencies are recorded privately so shared consumers do not inherit them while static closure remains complete.
