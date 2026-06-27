#include <gtest/gtest.h>

#include "../exchange/gateway/gateway.hpp"

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
    Gateway,
    AcceptsValidOrder)
{
    exchange::Gateway gateway(
        1000,
        100000000);

    const auto result =
        gateway.submit(
            make_buy(
                1,
                10000,
                100));

    EXPECT_TRUE(result.accepted);
}

TEST(
    Gateway,
    RejectsOversizedOrder)
{
    exchange::Gateway gateway(
        100,
        100000000);

    const auto result =
        gateway.submit(
            make_buy(
                1,
                10000,
                101));

    EXPECT_FALSE(result.accepted);
}

TEST(
    Gateway,
    ExecutesCrossingOrders)
{
    exchange::Gateway gateway(
        1000,
        100000000);

    [[maybe_unused]]
    const auto buy =
        gateway.submit(
            make_buy(
                1,
                10000,
                100));

    const auto sell =
        gateway.submit(
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
    Gateway,
    CancelUnknownOrderReturnsFalse)
{
    exchange::Gateway gateway(
        1000,
        100000000);

    EXPECT_FALSE(
        gateway.cancel_order(
            core::OrderId{42}));
}

TEST(
    Gateway,
    SnapshotContainsRestingOrder)
{
    exchange::Gateway gateway(
        1000,
        100000000);

    [[maybe_unused]]
    const auto result =
        gateway.submit(
            make_buy(
                1,
                10000,
                100));

    const auto snapshot =
        gateway.snapshot();

    EXPECT_EQ(
        snapshot.bids[0].price,
        10000);

    EXPECT_EQ(
        snapshot.bids[0].volume,
        100);
}