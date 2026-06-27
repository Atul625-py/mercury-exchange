#pragma once

#include <cstdint>
#include <string_view>

enum class Side : uint8_t 
{
    BUY = 0,
    SELL = 1
};

enum class OrderType : uint8_t 
{
    LIMIT = 0,
    MARKET = 1
};

using OrderID = uint64_t;
using Price = uint64_t;    // Fixed-point math (e.g., Rs. 100.50 represented as 10050)
using Quantity = uint32_t;

// Standard fixed-width ticker format for memory efficiency (e.g., "AAPL")
struct Symbol 
{
    char data[8]{};
    
    constexpr Symbol() = default;
    constexpr Symbol(std::string_view sv) 
    {
        for (size_t i = 0; i < 8 && i < sv.size(); ++i) 
        {

            data[i] = sv[i];
        }
    }
};

struct Order 
{
    OrderID order_id;
    Symbol symbol;
    Side side;
    OrderType type;
    Price price;
    Quantity quantity;
    uint64_t timestamp; // Nanoseconds since epoch
};

struct Trade 
{
    uint64_t trade_id;
    OrderID buyer_id;
    OrderID seller_id;
    Symbol symbol;
    Price price;
    Quantity quantity;
    uint64_t timestamp;
};