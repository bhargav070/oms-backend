#pragma once

#include "../domain/Order.hpp"
#include <string>

class IExchange {
public:
    virtual ~IExchange() = default;

    virtual std::string name() const = 0;

    virtual bool login() = 0;
    virtual bool isAuthenticated() const = 0;

    virtual std::string placeOrder(
        const Order& order) = 0;

    virtual std::string cancelOrder(
        const std::string& orderId) = 0;

    virtual std::string modifyOrder(
        const std::string& orderId,
        const Order& updated) = 0;

    virtual std::string getOpenOrders() = 0;
    virtual std::string getPositions() = 0;
};