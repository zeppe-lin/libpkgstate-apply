# Code style

The implementation is C++17. Public APIs use typed values and typed error codes; diagnostic strings are not control flow. Authority is passed explicitly and immutable results are returned by value. Ambient filesystem, process, environment, clock, or host state must not be consulted unless that mechanism is the declared purpose of the boundary.

Formatting follows `.clang-format` and `.editorconfig`. Continuation indentation uses spaces, never tabs. Public declarations carry the repository export annotation and complete Doxygen contracts. Shell qualification programs are POSIX `sh`, fail closed, quote paths, and keep generated build trees outside source inspection.

A boundary change starts from exact endpoint bodies. Missing implementation or API bodies are an unverified dependency, not permission to synthesize compatibility behavior.
