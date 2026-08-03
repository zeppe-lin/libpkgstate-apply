# Testing

The behavior suite is the extracted `apply_adapter_test.cpp` qualification from `libpkgstate` 2.5.1. Additional contracts compile the installed umbrella header, verify project version and SONAME, inspect pkg-config closure, reject forbidden authority dependencies, validate repository hygiene, and check the extracted body against a recorded SHA-256 manifest.

Shared and static builds are separate. CI runs GCC and Clang plus ASan/UBSan.
