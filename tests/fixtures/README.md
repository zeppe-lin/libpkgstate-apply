# Fixtures

`authority.h` builds deterministic source, catalog, resolution, build/image, and
installed-state authority through the neighboring public APIs. It contains data
construction, not adapter policy.

`application.h` builds planner requests, application requests, completed object
facts, path consequences, and lease-bound projection inputs from that authority.
It does not project durable state.

`state.h` supplies recording lease/store probes and complete install, upgrade,
and removal fixture graphs used by the integration programs.

Behavioral assertions stay in `tests/integration`; shared assertion primitives
stay in `tests/support`.
