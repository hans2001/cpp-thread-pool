## Mock Interview Q&A — Thread Pool Project

### Q1) What problem does your thread pool solve?
**A:** Creating a new thread for every task is expensive. A fixed thread pool reuses worker threads, reducing overhead and improving predictability under load.

### Q2) How does task submission work?
**A:** `submit()` wraps a callable in a `packaged_task`, enqueues it into a blocking queue, and returns a future. A worker thread pops and runs it, which fulfills the future.

### Q3) How does shutdown work, and why is it safe?
**A:** `shutdown()` stops accepting new tasks, closes the queue, wakes workers, and waits for them to finish. Workers exit once the queue is drained, so we don’t abandon queued tasks.

### Q4) How do you avoid data races?
**A:** The queue is protected by a mutex/condition variable. Shared counters use atomics. Workers only touch their task after it’s been safely popped from the queue.

### Q5) What telemetry do you collect and why?
**A:** Submitted/completed/rejected task counts plus queue depth and worker count. It helps detect back‑pressure and evaluate throughput changes after optimizations.

### Q6) Why is your pool faster than `std::async` in benchmarks?
**A:** `std::async` often spawns new threads, which is costly for many small tasks. The pool amortizes thread creation and reduces overhead.

### Q7) What happens if a task throws?
**A:** The exception is captured by the `std::future`. It surfaces when the caller calls `get()`; the worker thread keeps running.

### Q8) What’s the biggest limitation right now?
**A:** No work‑stealing yet. Under uneven workloads, some threads may be idle while others are busy. Work‑stealing is planned next.

### Q9) If you had more time, what would you implement?
**A:** Work‑stealing (M2), stronger task lifetime/cancellation rules (M3), and stress tests/benchmarks at scale (M4).

### Q10) How does this relate to streaming systems?
**A:** Streaming pipelines are latency‑sensitive and bursty. A predictable scheduler with telemetry for queue depth and throughput is foundational for keeping latency under control.

### Q11) What design tradeoff did you make?
**A:** Started with a single shared queue for simplicity and correctness. It’s easier to reason about, then optimize with work‑stealing once data shows contention.

### Q12) What happens if `submit()` is called after shutdown?
**A:** The pool rejects the task and returns `std::nullopt`. This prevents new work from entering once shutdown begins.

### Q13) How do you avoid deadlocks in the queue?
**A:** `pop()` waits on a condition variable with a clear predicate, and `close()` wakes all waiters. Workers exit when the queue is closed and empty.

### Q14) What’s your strategy for back‑pressure?
**A:** We expose queue depth and rejected counts. If queue depth grows, the system is overloaded and should scale or reduce input rate.

### Q15) Why use a blocking queue instead of busy‑waiting?
**A:** Blocking avoids wasting CPU while waiting for work and keeps performance predictable under low load.

### Q16) How would you add timeouts or cancellation?
**A:** Extend the task wrapper with a shared state and cancellation flag, then have workers skip canceled tasks. This is part of the planned M3 work.

### Q17) How do you decide the number of worker threads?
**A:** Start with hardware concurrency for CPU‑bound work, then measure. For mixed I/O, a higher count may help. Telemetry guides tuning.

### Q18) How would you test concurrency correctness?
**A:** Use targeted tests for shutdown and submission, plus sanitizers (ASan/TSan) to catch memory and race issues.

### Q19) What’s the biggest risk in the current design?
**A:** A single shared queue can become a contention point under heavy load. That’s why work‑stealing is the next step.

### Q20) What part of this maps to streaming systems?
**A:** The scheduler/queueing layer is foundational for a streaming pipeline; telemetry and predictable shutdown are critical for reliability and latency control.

### Q21) How would you explain this to a non‑technical person?
**A:** It’s a team of workers sharing a to‑do list; work gets done faster because workers don’t need to be hired for every task.

### Q22) How do you ensure memory visibility between threads?
**A:** Queue operations are protected by a mutex/condition variable, which establishes happens‑before relationships. Futures also synchronize when `get()` returns.

### Q23) Why not detach worker threads?
**A:** Joining in the pool ensures clean shutdown and no leaked threads or undefined lifetimes.

### Q24) What’s the fairness model?
**A:** FIFO queue with no priorities. It’s predictable and simple; priorities can be added later if needed.
