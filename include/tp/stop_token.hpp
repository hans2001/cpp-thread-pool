#pragma once

#include <atomic>
#include <memory>

namespace tp {

namespace detail {
struct StopState {
    std::atomic<bool> requested{false};
};
}

/// A lightweight observer that lets tasks or helpers detect when shutdown has been triggered.
/// The token is copyable/movable and watches the shared stop state owned by a `StopSource`.
class StopToken {
public:
    StopToken() noexcept = default;

    /// Returns true once any source owning the shared stop state calls `request_stop()`.
    bool stop_requested() const noexcept {
        return state_ && state_->requested.load(std::memory_order_acquire);
    }

    /// `false` when the token was default constructed or the source was destroyed.
    bool stop_possible() const noexcept {
        return static_cast<bool>(state_);
    }

private:
    friend class StopSource;
    explicit StopToken(std::shared_ptr<detail::StopState> state) noexcept : state_(std::move(state)) {}

    std::shared_ptr<detail::StopState> state_;
};

/// The initiator of shutdown. Once a source calls `request_stop()`, every shared token observes
/// `stop_requested()` as `true` so workers can stop draining work.
class StopSource {
public:
    StopSource() : state_(std::make_shared<detail::StopState>()) {}

    /// Share a token with callers so they can poll for stop requests.
    StopToken get_token() const noexcept { return StopToken(state_); }

    /// Requests a stop and returns `true` if this call transitioned the state to requested.
    bool request_stop() noexcept {
        if (!state_) return false;
        bool expected = false;
        return state_->requested.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
    }

    /// Convenience mirror of `StopToken::stop_requested()`.
    bool stop_requested() const noexcept {
        return state_ && state_->requested.load(std::memory_order_acquire);
    }

    /// `false` once the source is moved from/destroyed.
    bool stop_possible() const noexcept { return static_cast<bool>(state_); }

private:
    std::shared_ptr<detail::StopState> state_;
};

}
