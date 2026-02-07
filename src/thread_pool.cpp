#include "tp/thread_pool.hpp"

#include <cassert>

namespace tp {

ThreadPool::Impl::Impl(size_t worker_count) {
    if (worker_count == 0) {
        worker_count = 1;
    }

    workers.reserve(worker_count);
    for (size_t i = 0; i < worker_count; ++i) {
        workers.emplace_back([this]() {
            while (true) {
                auto task = queue.pop();
                if (!task.has_value()) {
                    break;
                }
                (*task)();
            }
        });
    }
}

ThreadPool::ThreadPool(size_t worker_count)
    : impl_(std::make_unique<Impl>(worker_count)) {}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::shutdown() {
    if (!impl_) return;
    bool expected = false;
    if (!impl_->shutdown.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
        return;
    }

    impl_->stop_source.request_stop();
    impl_->queue.close();

    const auto current_id = std::this_thread::get_id();
    for (auto& worker : impl_->workers) {
        if (!worker.joinable()) {
            continue;
        }
        if (worker.get_id() == current_id) {
            worker.detach();
            continue;
        }
        worker.join();
    }
}

bool ThreadPool::is_shutdown() const noexcept {
    return !impl_ || impl_->shutdown.load(std::memory_order_acquire);
}

StopToken ThreadPool::get_stop_token() const noexcept {
    if (!impl_) {
        return StopToken();
    }
    return impl_->stop_source.get_token();
}

ThreadPool::Stats ThreadPool::stats() const noexcept {
    Stats snapshot;
    if (!impl_) {
        return snapshot;
    }
    snapshot.submitted = impl_->submitted.load(std::memory_order_relaxed);
    snapshot.completed = impl_->completed.load(std::memory_order_relaxed);
    snapshot.rejected = impl_->rejected.load(std::memory_order_relaxed);
    snapshot.queued = impl_->queue.size();
    snapshot.workers = impl_->workers.size();
    return snapshot;
}

}  // namespace tp
