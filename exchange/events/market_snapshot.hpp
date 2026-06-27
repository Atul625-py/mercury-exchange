#pragma once

#include <array>

#include "../../core/config/constants.hpp"

namespace mercury::exchange
{

struct Level
{
    std::uint64_t price{0};

    std::uint32_t volume{0};
};

struct MarketSnapshot
{
    std::array<
        Level,
        mercury::core::MarketDepthLevels
    > bids;

    std::array<
        Level,
        mercury::core::MarketDepthLevels
    > asks;
};

}