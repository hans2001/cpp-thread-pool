#pragma once
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>

namespace tp {

template <typename T>
class BlockingQueue {
public:
    BlockingQueue() = default;
    ~BlockingQueue() = default;

    BlockingQueue(const BlockingQueue&) = delete;
    BlockingQueue& operator=(const BlockingQueue&) = delete;

    bool closed() const { 
      std::lock_guard<std::mutex> lock(mtx_);
      return closed_;
    }

    bool push(T value) { 
      std::lock_guard<std::mutex> lock(mtx_);
      if (closed_) return false;
      queue_.push_back(std::move(value));
      cv_.notify_one();
      return true;
    }

    std::optional<T> pop() {
      std::unique_lock<std::mutex> lock(mtx_);
      cv_.wait(lock, [this] { return closed_ || !queue_.empty(); }); 
      if (queue_.empty()) {
        return std::nullopt;  
      }
      T front = std::move(queue_.front());
      queue_.pop_front();
      return front;
    }

    std::optional<T> try_pop() {
      std::lock_guard<std::mutex> lock(mtx_);
      if (queue_.empty()) return std::nullopt;
      T front = std::move(queue_.front());
      queue_.pop_front();  
      return front;
    }

    void close() { 
      std::lock_guard<std::mutex> lock(mtx_);
      closed_ = true;
      cv_.notify_all();
    }
  
    bool empty() const {
      std::lock_guard<std::mutex> lock(mtx_);
      return queue_.empty();
    }

    size_t size( ) const {
      std::lock_guard<std::mutex> lock(mtx_);
      return queue_.size();
    }

private:
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    std::deque<T> queue_;
    bool closed_ = false;
};

}  
