# cpp-thread-pool

## BlockingQueue guarantees

- `push(T)` returns `true` while the queue is open and enqueues the value (moves it when possible); after `close()` it returns `false` and silently drops new items.
- `pop()` blocks until the queue has an item or `close()` is called. If closed and empty, it returns `std::nullopt`. Once an item is available, it moves it out and returns it.
- `try_pop()` never blocks: it returns the front item if available, otherwise `std::nullopt`, so it can be used in polling loops.
- `close()` sets the shutdown flag and calls `notify_all()` so every waiting `pop()` wakes, re-checks the predicate, and exits once the queue is drained.

### Close/pop wake rules

- `close()` runs under the mutex, flips `closed_`, and calls `cv_.notify_all()`, so every blocked `pop()` wakes exactly once with a consistent state view.
- A woken `pop()` checks `queue_.empty()`; if the queue drained because the pool shut down, it returns `std::nullopt`, otherwise it moves the front element.
- `push()` drops values once `closed_` is visible, so there are no surprises after shutdown starts.
- `try_pop()` and the query helpers `empty()`/`size()` also lock the mutex, so they can safely observe the queue during close without races.

Simple, fast, and predictable C++20 thread-pool scheduler with clear shutdown semantics and a minimal public API.

## Why It Matters
- Replace thread-per-task with a fixed worker pool to reduce overhead and keep CPU usage stable.
- Provide a clean submission API and a safe shutdown story for production-grade async work.
- Offer a focused, modern C++20 example that is easy to read, extend, and benchmark.

## At a Glance
- Predictable concurrency model with a fixed worker pool and clear shutdown path.
- Minimal C++20 API focused on safety, maintainability, and blocking-queue semantics.
- Benchmarks and examples included to validate throughput/latency and usage patterns.

## Project Status
- Current phase: M1 (Core Scheduler).
- Core implementation, tests, examples, and benchmarks are available.
- See `ROADMAP.md` and `TASKS.md` for the active timeline.

## Usage
Basic usage with futures:

```cpp
#include "tp/thread_pool.hpp"

int main() {
    tp::ThreadPool pool(4);

    auto fut = pool.submit([](int v) { return v * 2; }, 21);
    if (fut.has_value()) {
        auto value = fut->get();
    }

    pool.shutdown();
}
```

### Shutdown Semantics
- `shutdown()` stops accepting new tasks, wakes workers, and waits for queued tasks to drain.
- Submitting after shutdown returns `std::nullopt`.

### Telemetry
The pool tracks lightweight counters for:
- tasks submitted/completed/rejected
- current queue depth
- worker count

Use `ThreadPool::stats()` to snapshot counters for visibility and tuning.

## Repository Layout
- `include/tp/` — public headers (API contracts and types).
- `src/` — implementation (worker loop, queue integration, shutdown).
- `tests/` — sanity tests for queue + thread pool behavior.
- `bench/` — throughput/latency benchmarks.
- `examples/` — usage examples (`hello.cpp`, `parallel_sum.cpp`).

## Building
```bash
cmake -S . -B build
cmake --build build -j
```

## Benchmarks
```bash
./build/bench_throughput 20000 5000 4
./build/bench_latency 10000 2000 4
```

### Sample Results (Feb 11, 2026)
Results depend on hardware and load. These are sample numbers from the dev machine:
- Throughput: thread pool ~524k tasks/sec vs. `std::async` ~24.3k tasks/sec (**~21.5× faster**).
- Latency (us): thread pool avg ~19.1, p50 ~18.2, p95 ~64.6, p99 ~119.7.
- Latency (us): `std::async` avg ~43.9, p50 ~41.4, p95 ~54.5, p99 ~98.7.

## Roadmap
High-level milestones and timelines live in `ROADMAP.md`, with detailed, prioritized tasks in `TASKS.md`.
