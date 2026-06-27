#pragma once

#include <array>
#include <atomic>
#include <cstddef>

#include "../../core/config/constants.hpp"

namespace mercury::infrastructure
{

template<typename T, std::size_t Capacity>
class LockFreeQueue
{
    static_assert(
        (Capacity & (Capacity - 1)) == 0,
        "Capacity must be a power of two.");

public:

    LockFreeQueue() = default;

    LockFreeQueue(const LockFreeQueue&) = delete;

    LockFreeQueue& operator=(const LockFreeQueue&) = delete;

public:

    [[nodiscard]]
    bool enqueue(const T& item) noexcept
    {
        const std::size_t tail =
            tail_.load(std::memory_order_relaxed);

        const std::size_t head =
            head_.load(std::memory_order_acquire);

        if ((tail - head) == Capacity)
        {
            return false;
        }

        buffer_[tail & (Capacity - 1)] = item;

        tail_.store(
            tail + 1,
            std::memory_order_release);

        return true;
    }

    [[nodiscard]]
    bool dequeue(T& item) noexcept
    {
        const std::size_t head =
            head_.load(std::memory_order_relaxed);

        const std::size_t tail =
            tail_.load(std::memory_order_acquire);

        if (head == tail)
        {
            return false;
        }

        item = buffer_[head & (Capacity - 1)];

        head_.store(
            head + 1,
            std::memory_order_release);

        return true;
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return head_.load(std::memory_order_acquire)
            == tail_.load(std::memory_order_acquire);
    }

private:

    std::array<T, Capacity> buffer_{};

    alignas(mercury::core::CacheLineSize)
    std::atomic<std::size_t> head_{0};

    alignas(mercury::core::CacheLineSize)
    std::atomic<std::size_t> tail_{0};
};

}