#pragma once

#include "../../core/types/trade.hpp"

namespace mercury::infrastructure
{

class TradeLogger
{
public:

    explicit TradeLogger(
        const char* filename);

    ~TradeLogger();

public:

    void consume(
        const mercury::core::Trade& trade);

};

}