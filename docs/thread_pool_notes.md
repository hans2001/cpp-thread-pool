## Project Architecture And API

### Thread Pool
What it does: keeps a fixed set of worker threads alive and reuses them for tasks.
```cpp
tp::ThreadPool pool(4);
```

### Blocking Queue
What it does: `pop()` sleeps until work arrives or the queue is closed.
```cpp
auto item = queue.pop();
```

### Closeable Queue
What it does: after `close()`, `push()` rejects new work and `pop()` returns `std::nullopt` when empty.
```cpp
queue.close();
if (!queue.push(1)) { /* rejected */ }
```

### Orderly Shutdown
What it does: stop accepting work, close queue, wake workers, drain tasks, join threads.
```cpp
pool.shutdown();
```

### Drain vs Reject
What it does: tasks already queued are finished, new submissions are rejected.
```cpp
auto fut = pool.submit([]{});
if (!fut) { /* rejected */ }
```

### Telemetry Counters
What it does: track submitted/completed/rejected tasks, queue depth, workers.
```cpp
auto stats = pool.stats();
```
 
### RAII
What it does: resources are released in destructors automatically.
```cpp
ThreadPool::~ThreadPool() { shutdown(); }
```

## Task Submission Pipeline

### Callable
What it does: anything you can run like a function.
```cpp
auto f = [] { return 42; };
f();
```

### Lambdas
What it does: inline callable with optional captures.
```cpp
auto f = [x = 1] { return x + 1; };
```

### `std::function<void()>`
What it does: holds any callable with signature `void()`.
```cpp
std::function<void()> fn = []{};
```

### Type Erasure
What it does: hides the callable’s concrete type in `std::function`.
```cpp
std::function<int()> f = [] { return 1; };
```

### Templates
What it does: lets `submit()` accept any callable and arguments.
```cpp
template <typename Callable, typename... Args>
auto submit(Callable&& c, Args&&... args);
```

### Perfect Forwarding (`std::forward`)
What it does: preserves lvalue/rvalue-ness of arguments.
```cpp
std::forward<Callable>(callable);
```

### `std::invoke_result_t`
What it does: computes return type of a callable at compile time.
```cpp
using R = std::invoke_result_t<Callable, Args...>;
```

### `std::packaged_task<R()>`
What it does: connects a callable to a `future`.
```cpp
std::packaged_task<int()> task([]{ return 7; });
auto fut = task.get_future();
task();
```

### `std::future<R>`
What it does: waits for and retrieves a result from another thread.
```cpp
int v = fut.get();
```

### `std::optional<T>`
What it does: explicit “value or no value.”
```cpp
std::optional<int> v;
```

### `std::nullopt`
What it does: represents “no value.”
```cpp
return std::nullopt;
```

## Threading And Synchronization

### `std::thread`
What it does: runs a callable in a new thread.
```cpp
std::thread t([]{});
t.join();
```

### `join()`
What it does: waits for a thread to finish.
```cpp
t.join();
```

### `std::mutex`
What it does: protects shared data from concurrent access.
```cpp
std::mutex m;
```

### `std::lock_guard`
What it does: RAII lock/unlock for a mutex.
```cpp
std::lock_guard<std::mutex> lock(m);
```

### `std::unique_lock`
What it does: lock you can unlock/relock; required by `condition_variable`.
```cpp
std::unique_lock<std::mutex> lock(m);
```

### `std::condition_variable`
What it does: waits until a condition becomes true.
```cpp
cv.wait(lock, [&]{ return ready; });
```

### Spurious Wakeup
What it does: `wait` may wake even if condition is false, so predicate rechecks.
```cpp
cv.wait(lock, [&]{ return ready; });
```

### Busy Waiting
What it does: wastes CPU by spinning.
```cpp
while (!ready) { /* burn CPU */ }
```

## Atomics And Memory Model

### `std::atomic<T>`
What it does: safe concurrent variable access.
```cpp
std::atomic<int> counter{0};
```

### `fetch_add` / `fetch_sub`
What it does: atomic increment/decrement.
```cpp
counter.fetch_add(1);
```

### `compare_exchange_strong`
What it does: atomic compare-and-swap.
```cpp
bool expected = false;
flag.compare_exchange_strong(expected, true);
```

### Memory Ordering
What it does: controls visibility of writes between threads.
```cpp
flag.store(true, std::memory_order_release);
flag.load(std::memory_order_acquire);
```

### `memory_order_relaxed`
What it does: atomicity only (used for counters).
```cpp
counter.fetch_add(1, std::memory_order_relaxed);
```

### `memory_order_acquire` / `memory_order_release`
What it does: ensures shutdown visibility across threads.
```cpp
flag.store(true, std::memory_order_release);
```

## Ownership

### Move Semantics (`std::move`)
What it does: transfers ownership without copying.
```cpp
T b = std::move(a);
```

### `std::shared_ptr`
What it does: shared ownership of an object.
```cpp
auto p = std::make_shared<int>(3);
```
