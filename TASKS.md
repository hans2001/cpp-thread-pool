# cpp-thread-pool Tasks

Use this as the source of truth for per-milestone work. Check items as they land.

<<<<<<< HEAD
-## M0: Public API + Contracts
=======
## M0: Public API + Contracts
>>>>>>> main
- [x] [P0] Define `BlockingQueue<T>` public API in `include/tp/blocking_queue.hpp`.
- [x] [P0] Document close/pop wake rules and blocking behavior.
- [x] [P0] Define `StopToken` + `StopSource` public API in `include/tp/stop_token.hpp`.
- [x] [P0] Define `ThreadPool` public API in `include/tp/thread_pool.hpp`.
- [x] [P1] Define future/promise surface for task submission (if needed).
- [x] [P1] Add minimal examples showing submit + shutdown flow in `examples/`.

## M1: Core Scheduler
- [x] [P0] Implement core worker loop in `src/thread_pool.cpp`.
- [x] [P0] Implement queue integration with `BlockingQueue`.
- [x] [P0] Implement `ThreadPool::submit` with basic task wrapper.
- [x] [P0] Implement shutdown/close semantics (join, drain, cancel policy).
- [x] [P1] Add a basic end-to-end example that executes multiple tasks.
- [x] [P1] Add basic telemetry counters (submit/complete/reject/queue depth).
- [x] [P2] Add parallel sum example using futures.

## M2: Work Stealing
- [ ] [P0] Add per-worker deque structure and interface in `include/tp/`.
- [ ] [P0] Implement steal logic and fallback to central queue.
- [ ] [P1] Add simple contention metrics for queue/steal paths.
- [ ] [P1] Update benchmarks to compare M1 vs. M2 scheduling.

## M3: Task Lifetime Management
- [ ] [P0] Define task ownership model (ref-count or shared state).
- [ ] [P0] Implement task wrapper with safe completion/future linkage.
- [ ] [P1] Add cancellation/stop behavior for in-flight tasks.
- [ ] [P1] Add tests for lifecycle edge cases (shutdown + pending tasks).

## M4: Tests + Benchmarks
- [ ] [P0] Choose a test framework and wire it into CMake.
- [ ] [P0] Implement unit tests for queue, stop token, thread pool.
- [ ] [P1] Implement stress tests for enqueue/drain/shutdown.
- [x] [P1] Fill in latency and throughput benchmarks with stable configs.
- [x] [P1] Add assert-based sanity tests for queue and thread pool basics.
- [x] [P2] Add sanitizer build/run helper script.
- [x] [P2] Run benchmarks and record sample results.

## M5: Documentation + Release Prep
- [x] [P0] Update `README.md` with usage examples and guarantees.
- [x] [P1] Add design notes in `docs/` if needed.
- [x] [P1] Add study guide and interview Q&A.
- [ ] [P1] Add a release checklist (targets, compilers, platforms).
