## Design Decisions & Tradeoffs (Plain English)

### 1) Shared Queue vs. Work‑Stealing (why we started simple)
**Choice:** One shared blocking queue.  
**Why:** It’s easier to implement and reason about correctness (fewer moving parts).  
**Tradeoff:** Under uneven workloads, a single queue can create contention and imbalance.  
**Plan:** Add work‑stealing later after measuring real contention.

### 2) Shutdown Policy: Drain vs. Cancel
**Choice:** Drain tasks already queued; reject new tasks.  
**Why:** It prevents work loss and makes shutdown predictable.  
**Tradeoff:** Shutdown can take longer if many tasks are queued.  
**Explanation:** This is safer for most applications and easier to explain and test.

### 3) Futures for Results
**Choice:** Return `std::future` from `submit()`.  
**Why:** It’s a standard, easy‑to‑use way for callers to wait for results.  
**Tradeoff:** Futures add some overhead and require a shared state.  
**Benefit:** Simple, familiar API for users.

### 4) Telemetry Counters (why they exist)
**Choice:** Track submitted/completed/rejected tasks and queue depth.  
**Why:** These are the minimum metrics that tell you if the pool is overloaded or efficient.  
**Tradeoff:** Slight overhead from atomic increments.  
**Benefit:** You can make data‑driven tuning decisions.

### 5) BlockingQueue Close Semantics
**Choice:** `close()` wakes all waiters; `pop()` returns `nullopt` only when closed+empty.  
**Why:** This makes shutdown safe and prevents deadlocks.  
**Tradeoff:** After shutdown, `push()` silently rejects work.  
**Benefit:** Predictable shutdown behavior for users.

### 6) Single Owner of Workers
**Choice:** ThreadPool owns and joins workers.  
**Why:** Clear ownership means no leaked threads.  
**Tradeoff:** The pool controls lifecycle; users can’t “detach” workers.  
**Benefit:** Simple, safe lifecycle management.

### 7) `submit()` Returns Optional Future
**Choice:** `submit()` returns `std::optional<std::future<...>>`.  
**Why:** It clearly signals rejection when the pool is shutting down.  
**Tradeoff:** Callers must check `has_value()` before using the future.  
**Benefit:** Explicit error handling and cleaner shutdown behavior.

### 8) Queue Closure Drops New Work
**Choice:** After `close()`, `push()` returns false and drops new tasks.  
**Why:** It prevents tasks from getting stuck in a queue that workers won’t service.  
**Tradeoff:** Late submissions are lost.  
**Benefit:** Predictable shutdown and no hidden backlog.

### 9) Atomic Counters for Telemetry
**Choice:** Use atomics for submitted/completed/rejected counters.  
**Why:** Low overhead and safe from data races.  
**Tradeoff:** Counter values are approximate during heavy contention.  
**Benefit:** Lightweight observability without global locks.

### 10) No Priority Scheduling (Yet)
**Choice:** FIFO queue without priorities.  
**Why:** Simpler to implement and reason about.  
**Tradeoff:** Urgent tasks can’t jump the line.  
**Benefit:** Predictable, fair ordering.

### 11) No Work Stealing (Yet)
**Choice:** One shared queue in M1.  
**Why:** Lower complexity and easier correctness.  
**Tradeoff:** Potential contention and load imbalance.  
**Benefit:** Solid baseline before optimization.

### 12) Benchmarks vs. Real Workloads
**Choice:** Use synthetic CPU‑bound tasks for benchmarks.  
**Why:** Easy to control and compare results.  
**Tradeoff:** Doesn’t capture all real‑world behaviors.  
**Benefit:** Clear baseline for performance validation.

### 13) Exceptions Stay in Futures
**Choice:** Let task exceptions propagate through `std::future::get()`.  
**Why:** It matches standard C++ async patterns and keeps workers stable.  
**Tradeoff:** Callers must remember to `get()` and handle exceptions.  
**Benefit:** Clear, standard error propagation without crashing worker threads.

### 14) Fixed Worker Count
**Choice:** Create a fixed number of workers on construction.  
**Why:** Stable resource usage and predictable throughput.  
**Tradeoff:** No dynamic resizing under load.  
**Benefit:** Simpler mental model and easier benchmarking.

### 15) Stop Token Is Optional, Not Enforced
**Choice:** Provide a stop token but do not force tasks to check it.  
**Why:** Tasks may be non‑interruptible or have their own stop logic.  
**Tradeoff:** Tasks can run to completion even during shutdown.  
**Benefit:** Keeps the core pool simple and avoids surprising behavior.

### Risks & Mitigations
- **Risk:** Single shared queue becomes a bottleneck under heavy contention.  
  **Mitigation:** Add per‑worker queues + work‑stealing (planned M2).
- **Risk:** Shutdown waits too long if many tasks are queued.  
  **Mitigation:** Add optional cancellation policy or drain timeout (planned M3).
- **Risk:** Telemetry overhead under extreme load.  
  **Mitigation:** Keep counters lightweight and optional for release builds.
- **Risk:** Synthetic benchmarks don’t represent production workloads.  
  **Mitigation:** Add workload‑specific benchmarks once real usage is known.
- **Risk:** Callers ignore futures and miss exceptions.  
  **Mitigation:** Document usage; add optional logging for unobserved failures.
