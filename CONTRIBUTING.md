# Contributing

Changes must preserve the authority contract in `docs/architecture.md`:
`lease-bound canonical state + completed application -> state_publication_request`.

Do not move endpoint authority, orchestration, policy selection, discovery,
execution, persistence, migration, retry, or compatibility import into this
repository. A convenience that reacquires or infers missing authority is a
boundary violation even when it reduces caller code.

For a semantic change:

1. inspect the exact current endpoint APIs and implementation bodies;
2. state which invariant or refusal class changes;
3. add focused success and refusal tests;
4. update public declarations, manual, architecture, integration, and ABI
   records together;
5. review public and private dependency closure for shared and static
   consumers;
6. decide version and SONAME consequences explicitly.

Use C++17, POSIX shell for contract programs, GPL-3.0-or-later SPDX headers,
and the checked-in formatting rules. Keep commits single-purpose and suitable
for `git am` review.
