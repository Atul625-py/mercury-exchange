#include "order_book.hpp"
#include <iostream>

OrderBook::~OrderBook() 
{
    // Clear out any remaining allocated pointer nodes to prevent memory leaks
    for (auto& [id, order_ptr] : order_registry_) 
    {
        delete order_ptr;
    }
}

void OrderBook::append_to_level(LimitLevel& level, BookOrder* order) 
{
    // If the queue is completely empty
    if (!level.head) 
    {
        level.head = order;
        level.tail = order;
        order->prev = nullptr;
        order->next = nullptr;
    } else 
    {
        // Enforce FIFO: attach to the tail of the current queue
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
    // Separate the logic cleanly based on the order side

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
        
    } else 
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


Price OrderBook::get_best_bid() const 
{
    if (bid_levels_.empty()) return 0;
    return bid_levels_.begin()->first;
}

Price OrderBook::get_best_ask() const 
{
    if (ask_levels_.empty()) return 0;
    return ask_levels_.begin()->first;
}



std::vector<Trade> OrderBook::limit_order(OrderID id, Side side, Price price, Quantity quantity)
{
    std::vector<Trade> trades;
    uint64_t current_timestamp = 123456789; // Placeholder: we'll feed actual nanoseconds later
    static uint64_t trade_id_counter = 0;

    if (side == Side::BUY)
    {
        // Match against existing sell limit orders (Asks)
        while (quantity > 0 && !ask_levels_.empty())
        {
            auto it = ask_levels_.begin();
            Price best_ask = it->first;
            LimitLevel& level = it->second;

            // Aggressor price is lower than the lowest selling offer -> No match possible
            if (price < best_ask)
            {
                break;
            }

            // Sweep through the FIFO queue at this price level
            BookOrder* current_book_order = level.head;
            while (current_book_order && quantity > 0)
            {
                BookOrder* next_order = current_book_order->next;
                Quantity match_qty = std::min(quantity, current_book_order->quantity);

                // Generate the trade execution record
                trades.push_back(Trade{
                    .trade_id = ++trade_id_counter,
                    .buyer_id = id,
                    .seller_id = current_book_order->order_id,
                    .symbol = Symbol("MOCK"),
                    .price = best_ask,
                    .quantity = match_qty,
                    .timestamp = current_timestamp
                });

                quantity -= match_qty;
                current_book_order->quantity -= match_qty;
                level.total_volume -= match_qty;

                // If the resting book order is completely filled, pop it out
                if (current_book_order->quantity == 0)
                {
                    remove_from_level(current_book_order);
                    order_registry_.erase(current_book_order->order_id);
                    delete current_book_order;
                }

                current_book_order = next_order;
            }
        }

        // If there is leftover quantity, add it to the book as a resting order
        if (quantity > 0)
        {
            BookOrder* new_order = new BookOrder{
                .order_id = id,
                .price = price,
                .quantity = quantity,
                .side = side,
                .timestamp = current_timestamp
            };
            
            order_registry_[id] = new_order;
            
            // If the price level doesn't exist yet, it gets created automatically
            append_to_level(bid_levels_[price], new_order);
            bid_levels_[price].price = price;
        }
    }
    else // side == Side::SELL
    {
        // Match against existing buy limit orders (Bids)
        while (quantity > 0 && !bid_levels_.empty())
        {
            auto it = bid_levels_.begin();
            Price best_bid = it->first;
            LimitLevel& level = it->second;

            if (price > best_bid)
            {
                break;
            }

            BookOrder* current_book_order = level.head;
            while (current_book_order && quantity > 0)
            {
                BookOrder* next_order = current_book_order->next;
                Quantity match_qty = std::min(quantity, current_book_order->quantity);

                trades.push_back(Trade{
                    .trade_id = ++trade_id_counter,
                    .buyer_id = current_book_order->order_id,
                    .seller_id = id,
                    .symbol = Symbol("MOCK"),
                    .price = best_bid,
                    .quantity = match_qty,
                    .timestamp = current_timestamp
                });

                quantity -= match_qty;
                current_book_order->quantity -= match_qty;
                level.total_volume -= match_qty;

                if (current_book_order->quantity == 0)
                {
                    remove_from_level(current_book_order);
                    order_registry_.erase(current_book_order->order_id);
                    delete current_book_order;
                }

                current_book_order = next_order;
            }
        }

        if (quantity > 0)
        {
            BookOrder* new_order = new BookOrder{
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

    return trades;
}


void OrderBook::cancel_order(OrderID id)
{
    auto it = order_registry_.find(id);
    if (it == order_registry_.end())
    {
        return;
    }

    BookOrder* order_to_remove = it->second;
    remove_from_level(order_to_remove);
    order_registry_.erase(it);
    
    delete order_to_remove;
}