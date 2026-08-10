# ABI policy

`libpkgstate-apply` is an independently released C++17 shared and static
library. Release 3.0.0 preserves SONAME generation `3` from the formerly
in-tree product.

The reviewed ELF export set is stored in `abi/libpkgstate-apply.exports`.
Shared builds use hidden visibility and a generated version script; unreviewed
implementation symbols must not escape. Any export addition, removal, signature
change, exception hierarchy change, or public value-layout change requires an
explicit ABI decision before release.

The pkg-config file is part of the installed contract. Public requirements are
only those needed to compile installed declarations: `libpkgstate >=3.0.0` and
`libpkgapply >=3.0.0`. Private requirements are `libpkgstate-build >=3.0.0`,
`libpkgstate-plan >=3.0.0`, `libpkgplan >=0.3.0`, and `libcrypto`. Private
requirements must not leak into ordinary shared-consumer flags, but they must
appear in the static closure.

Repository version and SONAME generation are separate decisions. A
repository-major extraction does not by itself require a new SONAME when the
installed binary contract is preserved.
