#pragma once

#include <functional>

#include "ids.hpp"

namespace std
{

template<>
struct hash<mercury::core::OrderId>
{
    std::size_t operator()(const mercury::core::OrderId& id) const noexcept
    {
        return std::hash<std::uint64_t>{}(id.value);
    }
};

}