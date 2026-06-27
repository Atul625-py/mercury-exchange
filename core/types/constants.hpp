#pragma once

#include <cstddef>
#include <cstdint>

namespace mercury::core
{

inline constexpr std::size_t CacheLineSize = 64;

inline constexpr std::size_t DefaultOrderPoolSize = 102400;

inline constexpr std::size_t MaxTradesPerExecution = 128;

inline constexpr std::size_t DefaultEngineQueueCapacity = 8192;

inline constexpr std::size_t DefaultLoggerQueueCapacity = 65536;

inline constexpr std::size_t MarketDepthLevels = 5;

}