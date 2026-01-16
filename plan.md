# Multithreaded Task Scheduler Plan

## Goals
- Define a clear interface for the thread-pool/task scheduler before delving into implementation details.
- Document milestones and pace where it makes sense (see AGENTS guidance).
- Keep directory structure predictable as the project grows (headers, source, tests, examples).

## Directory Structure
1. `include/` – public headers such as `thread_pool.hpp` and any shared utility interfaces.
2. `src/` – implementation files like `thread_pool.cpp`, queue/stealer helpers, and benchmark drivers.
3. `tests/` – unit/integration tests (e.g., `thread_pool_test.cpp`) once a test framework is chosen.
4. `examples/` – sample programs or benchmark harnesses.
5. `docs/` (optional later) – design notes or profiling reports if the project requires them.

Existing README should capture the build/test instructions once we add tooling, and AGENTS.md should reference any new directories or tracking approach.

## Step-by-step Approach
1. **Interface First** – Sketch a minimal public API (probably in `include/thread_pool.hpp`) describing how clients enqueue tasks, control lifetime, and observe metrics. This helps focus the threading contracts before concrete details.
2. **Basic Scheduler Core** – Implement the core thread pool with worker threads, a central work queue, and basic synchronization primitives. Keep the implementation modular so we can swap in work-stealing components later.
3. **Work-Stealing Layer** – Introduce per-worker dequeues and steal logic, ensuring mutex/atomic usage matches the interface guarantees. Iterate on the queue access paths to avoid excessive contention.
4. **Task Lifetime Management** – Add structures (e.g., reference-counted tasks, explicit ownership wrappers) that keep tasks alive until completion, preventing dangling references.
5. **Benchmarks & Profiling** – Build benchmark harnesses comparing to `std::async`/thread-per-task and collect profiles. Use this to tune dispatch paths and queue interactions.
6. **Tests & Documentation** – Add unit tests under `tests/`, benchmarks under `examples/` or `benchmarks/`, and update `README.md` and `AGENTS.md` with commands and pacing notes.

## Tracking and Pace
- AGENTS.md can note the current phase (interface design, implementation, benchmarks) so the team knows our progress. We'll update it as we transition between major phases.
- Keep plan.md updated if we deviate significantly from this roadmap.

## Next Actions
1. Draft and finalize the public interface in `include/thread_pool.hpp`.
2. Begin implementing the scheduler core in `src/thread_pool.cpp`.
3. Revisit AGENTS.md and README to document new structure/instructions after reaching a stable milestone.

## Current Focus and Timeline
1. Map out the full API and behaviors for `include/tp/blocking_queue.hpp`, describing the locking/notification rules and how `close` wakes `pop()`.
2. Write short rationale notes for each method (in plan.md) so the implementation later has a precise contract.
3. Target the next ~48 hours to finish that documentation before shifting to worker-thread interactions; update AGENTS.md or plan.md if the schedule changes or new directories are added.
