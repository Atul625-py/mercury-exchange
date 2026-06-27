#pragma once

#include <array>

#include "../../core/config/constants.hpp"
#include "../../core/types/trade.hpp"
#include "../../core/types/enums.hpp"

namespace mercury::exchange
{

struct ExecutionReport
{
    mercury::core::OrderStatus status{
        mercury::core::OrderStatus::Accepted
    };

    std::array<
        mercury::core::Trade,
        mercury::core::MaxTradesPerExecution
    > trades{};

    std::size_t trade_count{0};

    std::uint32_t remaining_quantity{0};
};

}