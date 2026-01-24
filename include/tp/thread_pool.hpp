#pragma once

#include <future>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>
#include <utility>

#include "tp/blocking_queue.hpp"
#include "tp/stop_token.hpp"

namespace tp {

/// Minimal public surface for the thread pool used in this project.
/// The implementation will drive a set of worker threads that pop tasks
/// from a shared `BlockingQueue` and honor the `StopToken`/`StopSource`.
class ThreadPool {
public:
    explicit ThreadPool(size_t worker_count = std::thread::hardware_concurrency());

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    /// Schedule a callable with arguments. Returns a future when accepted,
    /// or `std::nullopt` if the pool is closed/shutting down.
    template <typename Callable, typename... Args>
    auto submit(Callable&& callable, Args&&... args)
        -> std::optional<std::future<std::invoke_result_t<Callable, Args...>>>;

    /// Initiate an orderly shutdown: stop accepting new tasks, wake all workers,
    /// and join them before returning.
    void shutdown();

    /// Returns `true` once shutdown has been requested.
    bool is_shutdown() const noexcept;

    /// Share a stop token with tasks/helpers so they can react to shutdown.
    StopToken get_stop_token() const noexcept;

    ~ThreadPool();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} 
