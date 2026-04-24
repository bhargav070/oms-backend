#include "OMSService.hpp"

std::string OMSService::placeOrder(IExchange& exchange, const Order& order) {
    return exchange.placeOrder(order);
}

std::string OMSService::cancelOrder(IExchange& exchange, const std::string& orderId) {
    return exchange.cancelOrder(orderId);
}