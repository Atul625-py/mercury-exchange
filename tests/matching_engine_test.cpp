#include <gtest/gtest.h>

#include "../exchange/matching/matching_engine.hpp"

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
    MatchingEngine,
    AcceptsSingleBuyOrder)
{
    exchange::MatchingEngine engine;

    [[maybe_unused]]
    const auto result =
        engine.submit(
            make_buy(
                1,
                10000,
                100));
}

TEST(
    MatchingEngine,
    GeneratesTradeForCrossingOrders)
{
    exchange::MatchingEngine engine;

    [[maybe_unused]]
    const auto buy =
        engine.submit(
            make_buy(
                1,
                10000,
                100));

    const auto sell =
        engine.submit(
            make_sell(
                2,
                9900,
                100));

    EXPECT_EQ(
        sell.trade_count,
        1u);

    EXPECT_EQ(
        sell.remaining_quantity,
        0u);
}

TEST(
    MatchingEngine,
    TradeIdsIncreaseSequentially)
{
    exchange::MatchingEngine engine;

    [[maybe_unused]]
    const auto buy =
        engine.submit(
            make_buy(
                1,
                10000,
                100));

    const auto first =
        engine.submit(
            make_sell(
                2,
                9900,
                50));

    const auto second =
        engine.submit(
            make_sell(
                3,
                9900,
                50));

    ASSERT_EQ(first.trade_count, 1u);
    ASSERT_EQ(second.trade_count, 1u);

    EXPECT_LT(
        first.trades[0].trade_id.value,
        second.trades[0].trade_id.value);
}

TEST(
    MatchingEngine,
    CancelUnknownOrderReturnsFalse)
{
    exchange::MatchingEngine engine;

    EXPECT_FALSE(
        engine.cancel_order(
            core::OrderId{999}));
}

TEST(
    MatchingEngine,
    SnapshotAfterInsertion)
{
    exchange::MatchingEngine engine;

    [[maybe_unused]]
    const auto result =
        engine.submit(
            make_buy(
                1,
                10000,
                100));

    const auto snapshot =
        engine.snapshot();

    EXPECT_EQ(
        snapshot.bids[0].price,
        10000);

    EXPECT_EQ(
        snapshot.bids[0].volume,
        100);
}