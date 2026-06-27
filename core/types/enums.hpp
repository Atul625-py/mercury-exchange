#pragma once

#include <cstdint>

namespace mercury::core
{

//------------------------------------------------------------------------------
// Specifies whether an order adds or removes liquidity from the bid or ask side
// of the order book.
//------------------------------------------------------------------------------
enum class Side : std::uint8_t
{
    Buy = 0,
    Sell = 1
};

//------------------------------------------------------------------------------
// Defines the execution semantics of an incoming order.
//------------------------------------------------------------------------------
enum class OrderType : std::uint8_t
{
    Limit = 0,
    Market = 1
};

//------------------------------------------------------------------------------
// Represents the lifecycle state of an order inside the exchange.
//
// This will later be used by ExecutionReport and Market Data messages.
//------------------------------------------------------------------------------
enum class OrderStatus : std::uint8_t
{
    Accepted,
    PartiallyFilled,
    Filled,
    Cancelled,
    Rejected
};

//------------------------------------------------------------------------------
// Describes why an order failed pre-trade validation.
//
// Keeping rejection reasons explicit greatly simplifies debugging, testing,
// and future client APIs.
//------------------------------------------------------------------------------
enum class RejectReason : std::uint8_t
{
    None,

    InvalidPrice,
    InvalidQuantity,

    OrderTooLarge,
    ExposureExceeded,

    UnknownSymbol,
    DuplicateOrderId,

    MemoryPoolExhausted
};

} // namespace mercury::core