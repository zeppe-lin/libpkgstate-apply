# History

## 3.0.0 (2026-08-04)

- Extracted `libpkgstate-apply` from the `libpkgstate` 2.5.1 repository.
- Preserved the existing adapter behavior and SONAME generation 3.
- Established an independent dependency closure: libpkgstate >=3.0.0; libpkgapply >=2.3.0; libpkgstate-source >=3.0.0; libpkgstate-build >=3.0.0; libpkgplan >=0.3.0.
- Added public-header, pkg-config, extraction-provenance, architecture, repository, compiler, sanitizer, shared, and static qualification.
