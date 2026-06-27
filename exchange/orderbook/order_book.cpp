#include "order_book.hpp"

#include <algorithm>

namespace mercury::exchange
{

//=============================================================================
// Private Helper Functions
//=============================================================================

void OrderBook::insert_order(
    BookOrder* order)
{
    order->previous = nullptr;
    order->next = nullptr;

    if (order->side == mercury::core::Side::Buy)
    {
        LimitLevel& level = bids_[order->price];

        level.price = order->price;

        if (level.head == nullptr)
        {
            level.head = order;
            level.tail = order;
        }
        else
        {
            order->previous = level.tail;
            level.tail->next = order;
            level.tail = order;
        }

        level.total_volume += order->quantity;
        ++level.order_count;
    }
    else
    {
        LimitLevel& level = asks_[order->price];

        level.price = order->price;

        if (level.head == nullptr)
        {
            level.head = order;
            level.tail = order;
        }
        else
        {
            order->previous = level.tail;
            level.tail->next = order;
            level.tail = order;
        }

        level.total_volume += order->quantity;
        ++level.order_count;
    }

    order_registry_[order->order_id] = order;
}

void OrderBook::detach_order(
    BookOrder* order)
{
    if (order->side == mercury::core::Side::Buy)
    {
        auto level_it = bids_.find(order->price);

        if (level_it == bids_.end())
        {
            return;
        }

        LimitLevel& level = level_it->second;

        if (order->previous != nullptr)
        {
            order->previous->next = order->next;
        }
        else
        {
            level.head = order->next;
        }

        if (order->next != nullptr)
        {
            order->next->previous = order->previous;
        }
        else
        {
            level.tail = order->previous;
        }

        --level.order_count;

        if (level.order_count == 0)
        {
            bids_.erase(level_it);
        }
    }
    else
    {
        auto level_it = asks_.find(order->price);

        if (level_it == asks_.end())
        {
            return;
        }

        LimitLevel& level = level_it->second;

        if (order->previous != nullptr)
        {
            order->previous->next = order->next;
        }
        else
        {
            level.head = order->next;
        }

        if (order->next != nullptr)
        {
            order->next->previous = order->previous;
        }
        else
        {
            level.tail = order->previous;
        }

        --level.order_count;

        if (level.order_count == 0)
        {
            asks_.erase(level_it);
        }
    }

    order->previous = nullptr;
    order->next = nullptr;
    order->quantity = 0;
}

mercury::core::Price OrderBook::top_bid() const noexcept
{
    if (bids_.empty())
    {
        return 0;
    }

    return bids_.begin()->first;
}

mercury::core::Price OrderBook::top_ask() const noexcept
{
    if (asks_.empty())
    {
        return 0;
    }

    return asks_.begin()->first;
}

MarketSnapshot OrderBook::snapshot() const
{
    MarketSnapshot snapshot;

    std::size_t index = 0;

    for (
        auto it = bids_.begin();
        it != bids_.end() &&
        index < mercury::core::MarketDepthLevels;
        ++it,
        ++index)
    {
        snapshot.bids[index].price = it->first;
        snapshot.bids[index].volume = it->second.total_volume;
    }

    index = 0;

    for (
        auto it = asks_.begin();
        it != asks_.end() &&
        index < mercury::core::MarketDepthLevels;
        ++it,
        ++index)
    {
        snapshot.asks[index].price = it->first;
        snapshot.asks[index].volume = it->second.total_volume;
    }

    return snapshot;
}

//=============================================================================
// Public Member Functions
//=============================================================================

OrderResult OrderBook::execute(
    const mercury::core::Order& order,
    mercury::core::TradeId& next_trade_id)
{
    OrderResult result;

    result.remaining_quantity = order.quantity;

    if (order.side == mercury::core::Side::Buy)
    {
        while (
            result.remaining_quantity > 0 &&
            !asks_.empty())
        {
            auto best_level_it = asks_.begin();

            if (best_level_it->first > order.price)
            {
                break;
            }

            LimitLevel& level = best_level_it->second;

            BookOrder* resting_order = level.head;

            while (
                resting_order != nullptr &&
                result.remaining_quantity > 0)
            {
                BookOrder* next_resting_order = resting_order->next;

                const mercury::core::Quantity matched_quantity =
                    std::min(
                        result.remaining_quantity,
                        resting_order->quantity);

                if (result.trade_count >=result.trades.size())
                {
                    break;
                }

                auto& trade = result.trades[result.trade_count];

                trade.trade_id = next_trade_id++;

                trade.buyer = order.order_id;

                trade.seller = resting_order->order_id;

                trade.symbol = order.symbol;

                trade.price = level.price;

                trade.quantity = matched_quantity;

                trade.timestamp_ns = order.timestamp_ns;

                ++result.trade_count;

                result.remaining_quantity -= matched_quantity;

                resting_order->quantity -= matched_quantity;

                level.total_volume -= matched_quantity;

                if (resting_order->quantity == 0)
                {
                    detach_order(resting_order);

                    order_registry_.erase(resting_order->order_id);

                    order_pool_.deallocate(resting_order);
                }

                resting_order = next_resting_order;
            }
            if (result.trade_count >= result.trades.size())
            {
                break;
            }
        }

        if (result.remaining_quantity > 0)
        {
            BookOrder* resting_order =
                order_pool_.allocate();

            if (resting_order == nullptr)
            {
                result.accepted = false;

                result.status =
                    mercury::core::OrderStatus::Rejected;

                result.reject_reason =
                    mercury::core::RejectReason::MemoryPoolExhausted;

                return result;
            }

            *resting_order =
            {
                .order_id = order.order_id,
                .price = order.price,
                .quantity = result.remaining_quantity,
                .side = order.side,
                .timestamp_ns = order.timestamp_ns
            };

            insert_order(resting_order);
        }

        if (result.remaining_quantity == 0)
        {
            result.status =
                mercury::core::OrderStatus::Filled;
        }
        else if (result.trade_count > 0)
        {
            result.status =
                mercury::core::OrderStatus::PartiallyFilled;
        }

        return result;
    }

        while (
        result.remaining_quantity > 0 &&
        !bids_.empty())
    {
        auto best_level_it = bids_.begin();

        if (best_level_it->first < order.price)
        {
            break;
        }

        LimitLevel& level = best_level_it->second;

        BookOrder* resting_order = level.head;

        while (
            resting_order != nullptr &&
            result.remaining_quantity > 0)
        {
            BookOrder* next_resting_order = resting_order->next;

            const mercury::core::Quantity matched_quantity =
                std::min(
                    result.remaining_quantity,
                    resting_order->quantity);

            if (result.trade_count >= result.trades.size())
            {
                break;
            }

            auto& trade = result.trades[result.trade_count];

            trade.trade_id = next_trade_id++;

            trade.buyer = resting_order->order_id;

            trade.seller = order.order_id;

            trade.symbol = order.symbol;

            trade.price = level.price;

            trade.quantity = matched_quantity;

            // TODO: Replace with exchange execution timestamp.
            trade.timestamp_ns = order.timestamp_ns;

            ++result.trade_count;

            result.remaining_quantity -= matched_quantity;

            resting_order->quantity -= matched_quantity;

            level.total_volume -= matched_quantity;

            if (resting_order->quantity == 0)
            {
                detach_order(resting_order);

                order_registry_.erase(resting_order->order_id);

                order_pool_.deallocate(resting_order);
            }

            resting_order = next_resting_order;
        }
        if (result.trade_count >= result.trades.size())
        {
            break;
        }
    }

    if (result.remaining_quantity > 0)
    {
        BookOrder* resting_order =
            order_pool_.allocate();

        if (resting_order == nullptr)
        {
            result.accepted = false;

            result.status =
                mercury::core::OrderStatus::Rejected;

            result.reject_reason =
                mercury::core::RejectReason::MemoryPoolExhausted;

            return result;
        }

        *resting_order =
        {
            .order_id = order.order_id,
            .price = order.price,
            .quantity = result.remaining_quantity,
            .side = order.side,
            .timestamp_ns = order.timestamp_ns
        };

        insert_order(resting_order);
    }

    if (result.remaining_quantity == 0)
    {
        result.status =
            mercury::core::OrderStatus::Filled;
    }
    else if (result.trade_count > 0)
    {
        result.status =
            mercury::core::OrderStatus::PartiallyFilled;
    }

    return result;
}

bool OrderBook::cancel_order(
    mercury::core::OrderId order_id)
{
    auto order_it = order_registry_.find(order_id);

    if (order_it == order_registry_.end())
    {
        return false;
    }

    BookOrder* order = order_it->second;

    detach_order(order);

    order_registry_.erase(order_it);

    order_pool_.deallocate(order);
    
    return true;
}

} // namespace mercury::exchange