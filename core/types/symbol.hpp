#pragma once

#include <array>
#include <algorithm>
#include <cstdint>
#include <string_view>

namespace mercury::core
{

class Symbol
{
public:
    static constexpr std::size_t MaxLength = 8;

public:
    constexpr Symbol() = default;

    constexpr explicit Symbol(std::string_view symbol)
    {
        std::copy_n(
            symbol.begin(),
            std::min(symbol.size(), MaxLength),
            data_.begin());
    }

    [[nodiscard]]
    constexpr const auto& data() const noexcept
    {
        return data_;
    }

    constexpr auto operator<=>(const Symbol&) const = default;

private:
    std::array<char, MaxLength> data_{};
};

}