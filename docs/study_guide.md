## Thread Pool Project — Detailed Study Guide (Follow Exactly)

### Goal
By the end, you should be able to explain the design, walk through the code line by line, and answer interview questions about correctness, shutdown, and performance.

### Prereqs
You should be comfortable with these terms. If not, open `docs/thread_pool_notes.md` first.
`thread pool`, `blocking queue`, `condition_variable`, `mutex`, `future`, `packaged_task`, `atomic`, `memory ordering`

### Step 1: Build the mental model (10–15 min)
1. Read `README.md` top to bottom.
2. Summarize in 3 sentences:
3. The problem it solves.
4. The core flow (submit → queue → worker → future).
5. The shutdown guarantee (close, drain, join, reject).

### Step 2: Understand the BlockingQueue (20–30 min)
1. Open `include/tp/blocking_queue.hpp`.
2. For each method, answer these questions:
3. What shared state does it touch?
4. What lock does it take?
5. When can it block?
6. What does it return in the closed + empty case?
7. Why does `close()` call `notify_all()`?
8. Write a 2–3 sentence explanation of the queue’s semantics in your own words.

### Step 3: Understand task submission (20–30 min)
1. Open `include/tp/thread_pool.hpp`.
2. Focus on `ThreadPool::submit()`:
3. Identify how the return type is computed (`std::invoke_result_t`).
4. Explain why `std::packaged_task` is used.
5. Trace how a callable and args become a `std::function<void()>`.
6. Explain what happens when the pool is already shut down.
7. Write a 2–3 sentence explanation of `submit()` in your own words.

### Step 4: Understand the worker loop and shutdown (20–30 min)
1. Open `src/thread_pool.cpp`.
2. Read `ThreadPool::Impl::Impl`:
3. Explain how workers are created.
4. Explain when a worker exits the loop.
5. Read `ThreadPool::shutdown()`:
6. Explain why it uses `compare_exchange_strong`.
7. Explain how it prevents new tasks and wakes workers.
8. Explain why it joins threads and why self‑join is avoided.
9. Write a 3–4 sentence explanation of shutdown in your own words.

### Step 5: Validate behavior with tests (20–30 min)
1. Read `tests/test_basic.cpp` to see queue behavior.
2. Read `tests/test_future.cpp` to see future usage and rejection after shutdown.
3. Read `tests/test_shutdown.cpp` to see drain behavior.
4. For each test, answer:
5. What property is the test proving?
6. What would break if the property failed?

### Step 6: Understand the performance story (10–15 min)
1. Read `bench/bench_throughput.cpp`.
2. Read `bench/bench_latency.cpp`.
3. Explain why `std::async` is slower here.
4. Explain the difference between throughput and latency in one sentence each.

### Step 7: Practice the interview explanation (10–15 min)
1. Use `docs/interview_qa.md`.
2. Say the 30‑second explanation out loud.
3. Answer Q3, Q6, Q12, and Q18 without reading the answers.

### Step 8: Write your own summary (10 min)
Write a 1‑page summary that includes:
1. The mental model.
2. The shutdown contract.
3. The data structures and why they were chosen.
4. The biggest tradeoff.
5. The benchmark takeaway.

### Step 9: Stretch questions (optional)
1. How would you add work‑stealing?
2. How would you add task cancellation?
3. What’s the most likely contention point and why?

### If you get stuck
Open `docs/thread_pool_notes.md` and restate the relevant concept in your own words, then return to the code.
