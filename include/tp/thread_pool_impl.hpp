#pragma once

#include "tp/thread_pool.hpp"

namespace tp {

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

}  // namespace tp
