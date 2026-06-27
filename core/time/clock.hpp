#pragma once

#include <chrono>
#include <cstdint>

namespace mercury::core
{

class Clock
{
public:

    [[nodiscard]]
    static std::uint64_t now_ns() noexcept
    {
        return std::chrono::duration_cast<
            std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count();
    }
};

}