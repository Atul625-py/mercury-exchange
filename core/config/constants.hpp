#pragma once

#include <cstddef>

namespace mercury::core
{

inline constexpr std::size_t CacheLineSize = 64;

inline constexpr std::size_t MarketDepthLevels = 5;

inline constexpr std::size_t DefaultOrderPoolSize = 102400;

inline constexpr std::size_t MaxTradesPerExecution = 128;

inline constexpr std::size_t EngineQueueCapacity = 8192;

inline constexpr std::size_t LoggerQueueCapacity = 65536;

inline constexpr std::size_t DefaultWindowWidth = 1280;

inline constexpr std::size_t DefaultWindowHeight = 720;

}