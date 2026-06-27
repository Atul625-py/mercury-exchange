#pragma once

#include <cstdint>

#include "enums.hpp"
#include "ids.hpp"
#include "numeric_types.hpp"
#include "symbol.hpp"

namespace mercury::core
{

//------------------------------------------------------------------------------
// Represents an order submitted to the exchange.
//
// Orders are immutable request objects that travel through the exchange
// pipeline:
//
// Gateway
//     ↓
// Matching Engine
//     ↓
// Order Book
//
// The structure intentionally contains no behaviour and remains a plain data
// object for maximum cache efficiency and trivial copying.
//------------------------------------------------------------------------------
struct Order
{
    OrderId order_id{};

    Symbol symbol{};

    Side side{Side::Buy};

    OrderType type{OrderType::Limit};

    Price price{0};

    Quantity quantity{0};

    // Timestamp captured when the order enters the exchange.
    std::uint64_t timestamp_ns{0};
};

} // namespace mercury::core