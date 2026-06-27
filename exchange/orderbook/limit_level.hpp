#pragma once

#include "book_order.hpp"

namespace mercury::exchange
{

//------------------------------------------------------------------------------
// Represents a single price level within the order book.
//
// Each price level owns an intrusive FIFO linked list of BookOrder nodes.
// Incoming orders that cannot be matched immediately are appended to the tail,
// preserving exchange-standard price-time priority.
//
// The aggregated volume is maintained incrementally, allowing market depth
// queries without traversing every resting order.
//------------------------------------------------------------------------------
struct LimitLevel
{
    mercury::core::Price price{0};

    mercury::core::Quantity total_volume{0};

    std::uint32_t order_count{0};

    BookOrder* head{nullptr};

    BookOrder* tail{nullptr};
};

} // namespace mercury::exchange