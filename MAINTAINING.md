# Maintaining libpkgstate-apply

Before release, build shared and static configurations separately with GCC and Clang, run ASan and UBSan, compile every public header independently, inspect generated pkg-config metadata, verify SONAME 3, lint the manual, run strict Doxygen, stage-install a consumer, and replay the mailbox into an empty repository.

Changes in either endpoint authority require inspection of the exact corresponding repository body. Missing bodies are an unverified edge, never permission to add guessed compatibility.
