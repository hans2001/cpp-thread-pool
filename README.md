# cpp-thread-pool

Simple, fast, and predictable C++20 thread-pool scheduler with clear shutdown semantics and a minimal public API.

## Why It Matters
- Replace thread-per-task with a fixed worker pool to reduce overhead and keep CPU usage stable.
- Provide a clean submission API and a safe shutdown story for production-grade async work.
- Offer a focused, modern C++20 example that is easy to read, extend, and benchmark.

## At a Glance
- Predictable concurrency model with a fixed worker pool and clear shutdown path.
- Minimal C++20 API focused on safety, maintainability, and blocking-queue semantics.
- Roadmap-driven implementation with tests/benchmarks scaffolded for validation.

## Project Status
- Current phase: M0 (Public API + Contracts).
- Public headers live in `include/tp/` with API contracts being finalized.
- Core implementation and examples are in progress.
- See `ROADMAP.md`, `TASKS.md`, and `plan.md` for the active timeline.

## Planned Usage (Draft API)
The API below is a target shape and may change while M0 is finalized.

```cpp
#include <tp/thread_pool.hpp>

int main() {
    tp::thread_pool pool(4);

    pool.enqueue_detach([](int v) { /* work */ }, 42);

    auto fut = pool.enqueue([](int v) { return v * 2; }, 21);
    auto value = fut.get();

    pool.wait_for_tasks();
}
```

## Repository Layout
- `include/tp/` — public headers (API contracts and types).
- `src/` — implementation (worker loop, queue integration, shutdown).
- `tests/` — unit/integration tests (scaffolded).
- `bench/` — benchmark drivers (scaffolded).
- `examples/` — usage examples (in progress).

## Building (Scaffold)
The CMake setup is currently a placeholder and will be finalized alongside the core implementation. Build guidance will be updated in M1.

## Roadmap
High-level milestones and timelines live in `ROADMAP.md`, with detailed, prioritized tasks in `TASKS.md`.
