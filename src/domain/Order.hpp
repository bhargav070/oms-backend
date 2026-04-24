#pragma once
#include "Types.hpp"
#include <string>

struct Order {
    std::string symbol;
    Side side;
    OrderType type;
    double price;
    double quantity;
};