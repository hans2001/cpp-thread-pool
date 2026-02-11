# BlockingQueue Notes

`BlockingQueue<T>` is a minimal thread-safe queue with close semantics.

Core behavior:
- `push(T)` enqueues while open; returns `false` after `close()`.
- `pop()` blocks until an item is available or the queue is closed.
- `pop()` returns `std::nullopt` only when closed and empty.
- `close()` wakes all waiters so workers can exit cleanly.

Why it matters:
- Prevents busy-waiting.
- Guarantees shutdown doesn’t hang.
- Provides a clear handoff between producers and workers.
