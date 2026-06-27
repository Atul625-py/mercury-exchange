#pragma once

#include <array>
#include <cstddef>

#include "../../core/config/constants.hpp"
#include "../../core/types/enums.hpp"
#include "../../core/types/trade.hpp"

namespace mercury::exchange
{

//------------------------------------------------------------------------------
// Represents the outcome of processing an incoming order.
//
// Besides reporting the final status of the order, this structure carries every
// trade generated during matching. It is intentionally returned by value since
// its size is fixed and known at compile time.
//------------------------------------------------------------------------------
struct OrderResult
{
    bool accepted{true};

    mercury::core::OrderStatus status
    {
        mercury::core::OrderStatus::Accepted
    };

    mercury::core::RejectReason reject_reason
    {
        mercury::core::RejectReason::None
    };

    std::array<
        mercury::core::Trade,
        mercury::core::MaxTradesPerExecution
    > trades{};

    std::size_t trade_count{0};

    mercury::core::Quantity remaining_quantity{0};
};

} // namespace mercury::exchange