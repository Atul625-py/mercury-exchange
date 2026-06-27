#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>

namespace mercury::core
{

//------------------------------------------------------------------------------
// Fixed-capacity object pool.
//
// The pool pre-allocates storage for all objects during construction,
// guaranteeing deterministic O(1) allocation and deallocation without touching
// the heap.
//
// This allocator is intended for latency-sensitive paths where heap allocation
// would introduce unpredictable pauses.
//------------------------------------------------------------------------------
template <typename T, std::size_t Capacity>
class ObjectPool
{
    static_assert(Capacity > 0);

public:

    ObjectPool()
    {
        for (std::size_t i = 0; i < Capacity; ++i)
        {
            free_stack_[i] = Capacity - i - 1;
        }

        free_top_ = Capacity;
    }

    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;

    ~ObjectPool() = default;

public:

    [[nodiscard]]
    T* allocate() noexcept
    {
        if (free_top_ == 0)
        {
            return nullptr;
        }

        const std::size_t index = free_stack_[--free_top_];

        return &storage_[index];
    }

    void deallocate(T* object) noexcept
    {
        assert(object != nullptr);

        const std::size_t index =
            static_cast<std::size_t>(object - storage_.data());

        free_stack_[free_top_++] = index;
    }

    [[nodiscard]]
    constexpr std::size_t capacity() const noexcept
    {
        return Capacity;
    }

    [[nodiscard]]
    std::size_t available() const noexcept
    {
        return free_top_;
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return free_top_ == 0;
    }

private:

    std::array<T, Capacity> storage_;

    std::array<std::size_t, Capacity> free_stack_;

    std::size_t free_top_{0};
};

}