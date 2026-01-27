# cpp-thread-pool Roadmap

## Timeline Assumptions
- Timeline is relative to the start of active work on the milestone.
- Adjust dates in this file if a window shifts.

## Milestones

### M0: Public API + Contracts (Week 1)
- Deliverables: header-level APIs for queue, stop token, thread pool, and futures.
- Exit criteria: documented threading and shutdown guarantees; no missing API decisions.

### M1: Core Scheduler (Weeks 2-3)
- Deliverables: worker loop, shared queue, submit path, graceful shutdown.
- Exit criteria: basic end-to-end example works and drains tasks safely.

### M2: Work Stealing (Week 4)
- Deliverables: per-worker deque + stealing logic; metrics hooks for contention.
- Exit criteria: contention hot spots reduced in benchmarks vs. M1.

### M3: Task Lifetime Management (Week 5)
- Deliverables: task ownership wrapper, reference counting, cancellation rules.
- Exit criteria: no dangling task references; tests cover lifecycle edge cases.

### M4: Tests + Benchmarks (Week 6)
- Deliverables: unit/integration tests; latency/throughput benchmarks.
- Exit criteria: CI-ready test suite; baseline perf numbers recorded.

### M5: Documentation + Release Prep (Week 7)
- Deliverables: README updates, usage examples, design notes.
- Exit criteria: docs match behavior; release checklist ready.
