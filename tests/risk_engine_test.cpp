#include <gtest/gtest.h>

#include "../exchange/risk/risk_engine.hpp"

using namespace mercury;

namespace
{

core::Order make_order()
{
    return
    {
        .order_id = core::OrderId{1},
        .symbol = core::Symbol("AAPL"),
        .side = core::Side::Buy,
        .type = core::OrderType::Limit,
        .price = 10000,
        .quantity = 100,
        .timestamp_ns = 1
    };
}

} // namespace

TEST(
    RiskEngine,
    AcceptsValidOrder)
{
    exchange::RiskEngine engine(
        1000,
        100000000);

    EXPECT_TRUE(
        engine.validate(
            make_order()));
}

TEST(
    RiskEngine,
    RejectsZeroPrice)
{
    exchange::RiskEngine engine(
        1000,
        100000000);

    auto order = make_order();

    order.price = 0;

    EXPECT_FALSE(
        engine.validate(
            order));
}

TEST(
    RiskEngine,
    RejectsZeroQuantity)
{
    exchange::RiskEngine engine(
        1000,
        100000000);

    auto order = make_order();

    order.quantity = 0;

    EXPECT_FALSE(
        engine.validate(
            order));
}

TEST(
    RiskEngine,
    RejectsOversizedOrder)
{
    exchange::RiskEngine engine(
        100,
        100000000);

    auto order = make_order();

    order.quantity = 101;

    EXPECT_FALSE(
        engine.validate(
            order));
}

TEST(
    RiskEngine,
    RejectsExposureExceeded)
{
    exchange::RiskEngine engine(
        100000,
        10000);

    auto order = make_order();

    EXPECT_FALSE(
        engine.validate(
            order));
}