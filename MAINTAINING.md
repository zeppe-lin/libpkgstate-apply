# Maintaining libpkgstate-apply

The release gate is the repository contract, not merely a successful compilation.

Before tagging:

1. confirm the authority and non-authority statements still match the implementation;
2. build clean GCC and Clang shared and static configurations separately;
3. run the optimized release and ASan/UBSan configurations;
4. compile every installed public header independently;
5. compare exports with `abi/libpkgstate-apply.exports` and verify SONAME `3`;
6. inspect pkg-config public requirements and static private closure;
7. inspect shared `DT_NEEDED` edges for accidental authority coupling;
8. run strict Doxygen and manual lint;
9. stage-install and compile the installed consumer;
10. verify installed documentation and repository hygiene;
11. replay the release mailbox into a clean repository and compare Git trees.

Release after the state core, state build and plan bridges, and the repository-separated `libpkgapply` 3.0 generation.

Changes in an adjacent owner require inspection of that exact repository body. Do not paper over an unavailable or incompatible generation with fallbacks, guessed APIs, or broad version floors.
