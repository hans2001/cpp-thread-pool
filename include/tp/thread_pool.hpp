#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "tp/blocking_queue.hpp"
#include "tp/stop_token.hpp"

namespace tp {

/// Minimal public surface for the thread pool used in this project.
/// The implementation will drive a set of worker threads that pop tasks
/// from a shared `BlockingQueue` and honor the `StopToken`/`StopSource`.
class ThreadPool {
public:
    struct Stats {
        uint64_t submitted = 0;
        uint64_t completed = 0;
        uint64_t rejected = 0;
        uint64_t queued = 0;
        uint64_t workers = 0;
    };

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

    /// Snapshot basic telemetry counters.
    Stats stats() const noexcept;

    ~ThreadPool();

private:
    struct Impl {
        explicit Impl(size_t worker_count);

        BlockingQueue<std::function<void()>> queue;
        StopSource stop_source;
        std::atomic<bool> shutdown{false};
        std::atomic<uint64_t> submitted{0};
        std::atomic<uint64_t> completed{0};
        std::atomic<uint64_t> rejected{0};
        std::vector<std::thread> workers;
    };
    std::unique_ptr<Impl> impl_;
};

template <typename Callable, typename... Args>
auto ThreadPool::submit(Callable&& callable, Args&&... args)
    -> std::optional<std::future<std::invoke_result_t<Callable, Args...>>> {
    using Result = std::invoke_result_t<Callable, Args...>;

    if (!impl_ || impl_->shutdown.load(std::memory_order_acquire)) {
        if (impl_) {
            impl_->rejected.fetch_add(1, std::memory_order_relaxed);
        }
        return std::nullopt;
    }

    auto task_ptr = std::make_shared<std::packaged_task<Result()>>(
        std::bind(std::forward<Callable>(callable), std::forward<Args>(args)...));
    auto future = task_ptr->get_future();

    impl_->submitted.fetch_add(1, std::memory_order_relaxed);
    auto accepted = impl_->queue.push([task_ptr, impl = impl_.get()]() {
        (*task_ptr)();
        impl->completed.fetch_add(1, std::memory_order_relaxed);
    });
    if (!accepted) {
        impl_->submitted.fetch_sub(1, std::memory_order_relaxed);
        impl_->rejected.fetch_add(1, std::memory_order_relaxed);
        return std::nullopt;
    }

    return future;
}

}
