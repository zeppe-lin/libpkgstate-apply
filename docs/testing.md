# Testing

Qualification is layered so a green runtime test cannot hide a damaged package boundary.

The behavior suite exercises successful projection and typed refusal paths. Public-header tests compile the umbrella and each installed header independently. Shared builds compare dynamic exports to `abi/libpkgstate-apply.exports`, verify SONAME `3`, and audit direct `DT_NEEDED` edges. Static builds prove the complete private pkg-config closure.

Source contracts verify architecture placement, release metadata, repository hygiene, CI coverage, style, and root-commit extraction provenance. The provenance contract checks the root extraction against the recorded `libpkgstate` 2.5.1 hashes; it intentionally does not freeze current implementation files.

CI runs GCC and Clang in separate shared and static configurations, one optimized release configuration, and ASan/UBSan configurations. Installation qualification compiles a consumer against staged headers and metadata, checks every installed header, audits the shared boundary or static archive, and verifies installed manual and project documentation.

A release candidate is incomplete until the exact dependency tags used by CI exist and the whole matrix is green from clean build directories.
