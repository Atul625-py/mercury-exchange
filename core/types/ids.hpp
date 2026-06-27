#pragma once

#include <compare>
#include <cstdint>
#include <functional>

namespace mercury::core
{

struct OrderId
{
    std::uint64_t value{0};

    constexpr auto operator<=>(const OrderId&) const = default;
};

struct TradeId
{
    std::uint64_t value{0};

    constexpr auto operator<=>(const TradeId&) const = default;

    constexpr TradeId& operator++() noexcept
    {
        ++value;
        return *this;
    }

    constexpr TradeId operator++(int) noexcept
    {
        TradeId copy = *this;
        ++(*this);
        return copy;
    }
};

} // namespace mercury::core

//------------------------------------------------------------------------------
// Hash support for unordered containers.
//------------------------------------------------------------------------------

template<>
struct std::hash<mercury::core::OrderId>
{
    std::size_t operator()(
        const mercury::core::OrderId& id) const noexcept
    {
        return std::hash<std::uint64_t>{}(id.value);
    }
};

template<>
struct std::hash<mercury::core::TradeId>
{
    std::size_t operator()(
        const mercury::core::TradeId& id) const noexcept
    {
        return std::hash<std::uint64_t>{}(id.value);
    }
};