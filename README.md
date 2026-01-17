# cpp-thread-pool

Modern, standards-forward C++ task scheduler built around a lightweight thread pool, a blocking queue, and an extensible stop token system. The project is intended to demonstrate how to orchestrate async work without overloading the system with one thread per task while keeping the public API simple and safe.

## Overview

`cpp-thread-pool` is designed to offer a predictable task execution engine that:

- keeps a fixed set of worker threads that pull work from a shared queue;
- exposes a minimal future-like interface so callers can enqueue jobs and observe completion;
- exposes controls for graceful shutdown via a dedicated stop token and a queue that can be closed;
- separates public headers (`include/tp/`) from implementation details in `src/`, making it simple to swap or extend components.

The project roadmap (see `plan.md`) focuses on refining the public API first, building the core scheduler, then adding work-stealing, task lifetime management, and benchmarks/tests.

## Key Concepts

- **BlockingQueue&lt;T&gt;** — a multi-producer, multi-consumer queue with blocking and non-blocking pop operations plus a `close()` that wakes waiting workers. Intended to be the central hand-off point between producers and worker threads.
- **Stop token** — a lightweight flag that workers can observe to exit cleanly instead of forcefully terminating threads.
- **Thread pool** — owns a fixed number of worker threads, accepts incoming tasks, and keeps them alive until all work is drained and the pool is shut down.
- **Benchmarks/tests** — skeletal drivers under `bench/` and `tests/` that will be fleshed out as the implementation matures.

## Repository Layout

- `include/tp/` — public headers for the thread pool, queue, stop token, and project configuration.
- `src/` — implementation files that tie the interface pieces together.
- `tests/` — placeholders for unit/integration tests covering task execution, futures, shutdown semantics, and stress testing.
- `bench/` — scaffolding for latency and throughput benchmarks that compare the scheduler against naive threading strategies.
- `plan.md` — living plan that tracks the current phase (interface design, implementation, benchmarks, etc.).
- `AGENTS.md` — agent instructions for collaborators (currently minimal, but intended to note stronger coordination details once the project grows).

## Building

1. Create a build directory: `cmake -S . -B build`.
2. Configure and generate: `cmake --build build`.
3. Run tests once implemented: `ctest --test-dir build`.

The `CMakeLists.txt` file is the build entry point; it currently sets up the structure to wire the headers, sources, tests, and benchmarks into a coherent build.

## Testing & Benchmarks

- Tests live under `tests/` (basic, future behavior, shutdown semantics, and stress suites) and can be run via `ctest` once the implementations and CMake targets are complete.
- Benchmarks (`bench_latency.cpp`, `bench_throughput.cpp`) will eventually exercise task latency and throughput against common scheduling patterns.

## Contribution & Roadmap

The next steps are documented in `plan.md` and focus on:

1. Finalizing the public interfaces (`include/tp/*`) so callers understand how to enqueue tasks and query their results.
2. Implementing the worker loop, task dispatch, and shutdown coordination in `src/`.
3. Adding concrete tests and benchmarks to demonstrate behavior and performance.

Contributions should respect this structure so future expansions (e.g., work stealing, profiling helpers, documentation) stay localized to clearly defined directories.
