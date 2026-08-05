# History

## 3.0.0 (2026-08-04)

- Extracted `libpkgstate-apply` from the `libpkgstate` 2.5.1 repository.
- Preserved the extracted behavior as repository provenance and retained SONAME generation 3.
- Established a public closure of `libpkgstate >=3.0.0` and
  `libpkgapply >=3.0.0`, with state-build, state-plan, planner, and crypto edges
  private to the implementation and static closure.
- Added public-header, pkg-config, extraction-provenance, architecture, repository, compiler, sanitizer, shared, and static qualification.
- Completed the documented public projection contract under Doxygen warnings-as-errors.
- Renamed the installed manual from the former in-tree adapter name to `libpkgstate-apply.3`.
- Reused `libpkgstate-plan` as the sole durable source-control projection into planner vocabulary.
- Raised the application dependency to the repository-separated `libpkgapply` 3.0 generation.
- Bound the adapter to the corrected repository-separated `libpkgapply` 3.0 dependency graph.
- Removed duplicate source/build dependency edges; incoming state authority is now projected from the exact `libpkgbuild-image` admission retained by the application request.
- Corrected the transaction-evidence export spellings after the state identity ABI reset without widening the reviewed symbol surface.
- Updated source-authority fixtures to the syntax-neutral `libpkgsource` 3.0 sealing contract.
