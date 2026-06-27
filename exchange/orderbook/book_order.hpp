#pragma once

#include "../../core/types/order.hpp"

namespace mercury::exchange
{

//------------------------------------------------------------------------------
// Represents a resting order inside the limit order book.
//
// Unlike core::Order, which models an incoming client request, BookOrder is
// the internal representation maintained by the OrderBook after any remaining
// quantity has been added as resting liquidity.
//
// Orders at the same price level are linked together using an intrusive
// doubly-linked list, allowing:
//
// • O(1) insertion at the tail
// • O(1) cancellation
// • Strict FIFO price-time priority
//
// Instances are allocated exclusively from ObjectPool<BookOrder>, ensuring
// deterministic allocation latency without heap allocation.
//------------------------------------------------------------------------------
struct BookOrder
{
    mercury::core::OrderId order_id{};

    mercury::core::Price price{0};

    mercury::core::Quantity quantity{0};

    mercury::core::Side side{mercury::core::Side::Buy};

    std::uint64_t timestamp_ns{0};

    BookOrder* previous{nullptr};

    BookOrder* next{nullptr};
};

} // namespace mercury::exchange