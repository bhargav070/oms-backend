#pragma once
#include "../domain/Order.hpp"
#include <string>

class IExchange {
public:
    virtual ~IExchange() = default;

    virtual std::string name() const = 0;

    virtual std::string placeOrder(const Order& order) = 0;
    virtual std::string cancelOrder(const std::string& orderId) = 0;
};