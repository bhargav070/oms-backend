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
    virtual std::string getOrderHistory() = 0;
    virtual std::string getPositions() = 0;

    virtual std::string getAccountSummary(
        const std::string& currency) = 0;

    virtual std::string getTicker(
        const std::string& instrument) = 0;

    virtual std::string getCandles(
        const std::string& instrument,
        const std::string& resolution,
        std::int64_t startMs,
        std::int64_t endMs) = 0;
};