#include <gtest/gtest.h>

#include "../exchange/orderbook/order_book.hpp"

using namespace mercury;

namespace
{

core::Order make_buy(
    std::uint64_t id,
    core::Price price,
    core::Quantity quantity)
{
    return
    {
        .order_id = core::OrderId{id},
        .symbol = core::Symbol("AAPL"),
        .side = core::Side::Buy,
        .type = core::OrderType::Limit,
        .price = price,
        .quantity = quantity,
        .timestamp_ns = id
    };
}

core::Order make_sell(
    std::uint64_t id,
    core::Price price,
    core::Quantity quantity)
{
    return
    {
        .order_id = core::OrderId{id},
        .symbol = core::Symbol("AAPL"),
        .side = core::Side::Sell,
        .type = core::OrderType::Limit,
        .price = price,
        .quantity = quantity,
        .timestamp_ns = id
    };
}

} // namespace

TEST(
    OrderBook,
    EmptyBookHasNoBestPrices)
{
    exchange::OrderBook book;

    EXPECT_EQ(book.top_bid(), 0);
    EXPECT_EQ(book.top_ask(), 0);
}

TEST(
    OrderBook,
    BuyOrderCreatesBestBid)
{
    exchange::OrderBook book;

    core::TradeId next_trade_id{1};

    [[maybe_unused]]
    const auto result =
        book.execute(
            make_buy(
                1,
                10000,
                100),
            next_trade_id);

    EXPECT_EQ(book.top_bid(), 10000);
    EXPECT_EQ(book.top_ask(), 0);
}

TEST(
    OrderBook,
    SellOrderCreatesBestAsk)
{
    exchange::OrderBook book;

    core::TradeId next_trade_id{1};

    [[maybe_unused]]
    const auto result =
        book.execute(
            make_sell(
                1,
                10100,
                100),
            next_trade_id);

    EXPECT_EQ(book.top_bid(), 0);
    EXPECT_EQ(book.top_ask(), 10100);
}

TEST(
    OrderBook,
    CrossingOrdersGenerateTrade)
{
    exchange::OrderBook book;

    core::TradeId next_trade_id{1};

    [[maybe_unused]]
    const auto buy =
        book.execute(
            make_buy(
                1,
                10000,
                100),
            next_trade_id);

    const auto sell =
        book.execute(
            make_sell(
                2,
                9900,
                100),
            next_trade_id);

    EXPECT_EQ(sell.trade_count, 1u);
    EXPECT_EQ(sell.remaining_quantity, 0u);

    EXPECT_EQ(book.top_bid(), 0);
    EXPECT_EQ(book.top_ask(), 0);
}

TEST(
    OrderBook,
    CancelUnknownOrderReturnsFalse)
{
    exchange::OrderBook book;

    EXPECT_FALSE(
        book.cancel_order(
            core::OrderId{999}));
}