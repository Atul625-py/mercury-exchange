#include "order_book.hpp"
#include <iostream>
#include <algorithm>

OrderBook::OrderBook()
{
    // Populate the free stack with pointers to our block array indices
    for (size_t i = 0; i < POOL_SIZE; ++i)
    {
        free_nodes_stack_[i] = i;
    }
}

OrderBook::~OrderBook()
{
    // Static allocations clear themselves gracefully
}

OrderBook::BookOrder* OrderBook::allocate_node()
{
    if (pool_index_ == 0)
    {
        throw std::runtime_error("OrderBook Memory Pool completely exhausted!");
    }
    size_t index = free_nodes_stack_[--pool_index_];
    return &order_pool_[index];
}

void OrderBook::deallocate_node(BookOrder* node)
{
    size_t index = node - &order_pool_[0]; // Pointer arithmetic yields array index
    free_nodes_stack_[pool_index_++] = index;
}

void OrderBook::append_to_level(LimitLevel& level, BookOrder* order)
{
    if (!level.head)
    {
        level.head = order;
        level.tail = order;
        order->prev = nullptr;
        order->next = nullptr;
    }
    else
    {
        level.tail->next = order;
        order->prev = level.tail;
        order->next = nullptr;
        level.tail = order;
    }
    level.total_volume += order->quantity;
    level.order_count++;
}

void OrderBook::remove_from_level(BookOrder* order)
{
    if (order->side == Side::BUY)
    {
        auto it = bid_levels_.find(order->price);
        if (it == bid_levels_.end()) return;
        LimitLevel& level = it->second;
        
        if (order->prev) order->prev->next = order->next;
        else level.head = order->next;
        
        if (order->next) order->next->prev = order->prev;
        else level.tail = order->prev;
        
        level.total_volume -= order->quantity;
        level.order_count--;
        if (level.order_count == 0) bid_levels_.erase(it);
    }
    else
    {
        auto it = ask_levels_.find(order->price);
        if (it == ask_levels_.end()) return;
        LimitLevel& level = it->second;
        
        if (order->prev) order->prev->next = order->next;
        else level.head = order->next;
        
        if (order->next) order->next->prev = order->prev;
        else level.tail = order->prev;
        
        level.total_volume -= order->quantity;
        level.order_count--;
        if (level.order_count == 0) ask_levels_.erase(it);
    }
}

size_t OrderBook::limit_order(OrderID id, Side side, Price price, Quantity quantity, TradeArray& trade_buffer)
{
    size_t trade_count = 0;
    uint64_t current_timestamp = 123456789;
    static uint64_t trade_id_counter = 0;

    if (side == Side::BUY)
    {
        while (quantity > 0 && !ask_levels_.empty())
        {
            auto it = ask_levels_.begin();
            Price best_ask = it->first;
            LimitLevel& level = it->second;

            if (price < best_ask) break;

            BookOrder* current_book_order = level.head;
            while (current_book_order && quantity > 0)
            {
                BookOrder* next_order = current_book_order->next;
                Quantity match_qty = std::min(quantity, current_book_order->quantity);

                if (trade_count < MAX_TRADES_PER_ORDER)
                {
                    trade_buffer[trade_count++] = Trade{
                        .trade_id = ++trade_id_counter,
                        .buyer_id = id,
                        .seller_id = current_book_order->order_id,
                        .symbol = Symbol("MOCK"),
                        .price = best_ask,
                        .quantity = match_qty,
                        .timestamp = current_timestamp
                    };
                }

                quantity -= match_qty;
                current_book_order->quantity -= match_qty;
                level.total_volume -= match_qty;

                if (current_book_order->quantity == 0)
                {
                    remove_from_level(current_book_order);
                    order_registry_.erase(current_book_order->order_id);
                    deallocate_node(current_book_order); // Pooled reclaim
                }
                current_book_order = next_order;
            }
        }

        if (quantity > 0)
        {
            BookOrder* new_order = allocate_node(); // Pooled allocation
            *new_order = BookOrder{
                .order_id = id,
                .price = price,
                .quantity = quantity,
                .side = side,
                .timestamp = current_timestamp
            };
            order_registry_[id] = new_order;
            append_to_level(bid_levels_[price], new_order);
            bid_levels_[price].price = price;
        }
    }
    else
    {
        while (quantity > 0 && !bid_levels_.empty())
        {
            auto it = bid_levels_.begin();
            Price best_bid = it->first;
            LimitLevel& level = it->second;

            if (price > best_bid) break;

            BookOrder* current_book_order = level.head;
            while (current_book_order && quantity > 0)
            {
                BookOrder* next_order = current_book_order->next;
                Quantity match_qty = std::min(quantity, current_book_order->quantity);

                if (trade_count < MAX_TRADES_PER_ORDER)
                {
                    trade_buffer[trade_count++] = Trade{
                        .trade_id = ++trade_id_counter,
                        .buyer_id = current_book_order->order_id,
                        .seller_id = id,
                        .symbol = Symbol("MOCK"),
                        .price = best_bid,
                        .quantity = match_qty,
                        .timestamp = current_timestamp
                    };
                }

                quantity -= match_qty;
                current_book_order->quantity -= match_qty;
                level.total_volume -= match_qty;

                if (current_book_order->quantity == 0)
                {
                    remove_from_level(current_book_order);
                    order_registry_.erase(current_book_order->order_id);
                    deallocate_node(current_book_order);
                }
                current_book_order = next_order;
            }
        }

        if (quantity > 0)
        {
            BookOrder* new_order = allocate_node();
            *new_order = BookOrder{
                .order_id = id,
                .price = price,
                .quantity = quantity,
                .side = side,
                .timestamp = current_timestamp
            };
            order_registry_[id] = new_order;
            append_to_level(ask_levels_[price], new_order);
            ask_levels_[price].price = price;
        }
    }
    return trade_count;
}

void OrderBook::cancel_order(OrderID id)
{
    auto it = order_registry_.find(id);
    if (it == order_registry_.end()) return;

    BookOrder* order_to_remove = it->second;
    remove_from_level(order_to_remove);
    order_registry_.erase(it);
    deallocate_node(order_to_remove);
}

Price OrderBook::get_best_bid() const { return bid_levels_.empty() ? 0 : bid_levels_.begin()->first; }
Price OrderBook::get_best_ask() const { return ask_levels_.empty() ? 0 : ask_levels_.begin()->first; }