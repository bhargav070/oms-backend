#pragma once
#include "../interfaces/IExchange.hpp"

class OMSService {
public:
    std::string placeOrder(IExchange& exchange, const Order& order);
    std::string cancelOrder(IExchange& exchange, const std::string& orderId);
};