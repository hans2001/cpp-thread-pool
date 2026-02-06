## Thread Pool Project — 1‑Page Study Guide

### 1) What problem does this solve?
Creating a new thread per task is expensive. A **thread pool** keeps a fixed set of worker threads alive and feeds them tasks from a queue. This reduces overhead and makes performance more predictable.

### 2) Core pieces (mental model)
- **ThreadPool**: owns worker threads and the task queue.
- **BlockingQueue**: a thread‑safe queue where tasks wait until a worker pulls them.
- **Task**: a callable unit of work.
- **Future**: lets the caller wait for and get the result of a task.

### 3) How task submission works
1) Caller calls `submit()` with a function.
2) `submit()` wraps it in a `packaged_task` and pushes it into the queue.
3) A worker pops the task and runs it.
4) The future becomes ready when the task finishes.

### 4) Shutdown semantics (safety story)
- `shutdown()` **stops accepting new work**.
- It **closes the queue** and **wakes workers**.
- Workers exit once the queue is drained.
- This avoids deadlocks and ensures a clean exit.

### 5) Telemetry (why it exists)
The pool tracks:
- tasks submitted / completed / rejected
- queue depth
- worker count  
This lets you detect overload (queue growing), measure throughput, and reason about performance.

### 6) What the benchmarks show (high level)
The pool outperforms `std::async` in throughput and latency for many small tasks because it avoids repeated thread creation.

### 6.1) How to explain the benchmark numbers
- **Throughput** = tasks finished per second. Higher is better.  
  In the sample run, the pool finished about **6× more tasks/sec** than `std::async`, which means it handles many small tasks more efficiently.
- **Latency** = how long a task takes from submission to completion. Lower is better.  
  The pool’s average and tail latencies (p50/p95/p99) were **much lower**, meaning most tasks finished faster and the slowest tasks were still better than `std::async`.
- **Why** = the pool **reuses threads**, while `std::async` often creates a new thread per task, which adds overhead.

### 7) What’s *not* implemented yet
- Work stealing (M2)
- Advanced task lifetime management + cancellation (M3)
- Full stress test suite (M4)

### 8) 30‑second explanation (practice)
“I built a fixed‑size C++20 thread pool. Tasks are submitted into a blocking queue, workers pop and run them, and callers get a future for the result. Shutdown closes the queue and drains pending work safely. I also added telemetry counters and benchmarks to measure throughput/latency vs `std::async`.”
