#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>

#include "../exchange/gateway/gateway.hpp"

using namespace mercury;

int main()
{
    constexpr std::size_t OrderCount = 1'000'000;

    exchange::Gateway gateway(
        10000,
        100000000);

    std::mt19937_64 rng(42);

    std::uniform_int_distribution<int> side_dist(0, 1);
    std::uniform_int_distribution<int> price_dist(9950, 10050);
    std::uniform_int_distribution<int> quantity_dist(1, 500);

    std::size_t accepted = 0;
    std::size_t rejected = 0;
    std::size_t trades = 0;

    auto start =
        std::chrono::steady_clock::now();

    for (std::size_t i = 0; i < OrderCount; ++i)
    {
        core::Order order;

        order.order_id =
            core::OrderId{static_cast<std::uint64_t>(i + 1)};

        order.side =
            side_dist(rng)
                ? core::Side::Buy
                : core::Side::Sell;

        order.type =
            core::OrderType::Limit;

        order.price =
            price_dist(rng);

        order.quantity =
            quantity_dist(rng);

        order.timestamp_ns =
            static_cast<std::uint64_t>(i);

        auto result =
            gateway.submit(order);

        if (result.accepted)
            ++accepted;
        else
            ++rejected;

        trades += result.trade_count;
    }

    auto end =
        std::chrono::steady_clock::now();

    const double seconds =
        std::chrono::duration<double>(
            end - start).count();

    const double throughput =
        static_cast<double>(OrderCount) / seconds;

    const double latency_ns =
        seconds * 1e9 / OrderCount;

    std::cout
        << "\n"
        << "=============================================\n"
        << "        Mercury Exchange Benchmark\n"
        << "=============================================\n\n";

    std::cout
        << "Orders Submitted : "
        << OrderCount
        << '\n';

    std::cout
        << "Orders Accepted  : "
        << accepted
        << '\n';

    std::cout
        << "Orders Rejected  : "
        << rejected
        << '\n';

    std::cout
        << "Trades Executed  : "
        << trades
        << '\n';

    std::cout
        << "Elapsed Time     : "
        << std::fixed
        << std::setprecision(3)
        << seconds
        << " s\n";

    std::cout
        << "Throughput       : "
        << throughput / 1e6
        << " M orders/s\n";

    std::cout
        << "Latency          : "
        << latency_ns
        << " ns/order\n";

    std::cout
        << "\n=============================================\n";
}