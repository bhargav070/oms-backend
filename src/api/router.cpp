#include "router.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

Router::Router(OMSService& oms)
    : oms_(oms)
{
}

std::string Router::handle(
    const std::string& method,
    const std::string& target,
    const std::string& body,
    const std::string& auth)
{
    if (method == "POST" &&
        target == "/api/v1/auth/login")
    {
        return oms_.login(
            json::parse(body));
    }

    std::string token;

    if (auth.rfind("Bearer ", 0) == 0)
        token = auth.substr(7);

    if (method == "POST" &&
        target == "/api/v1/orders")
    {
        return oms_.placeOrder(
            token,
            json::parse(body));
    }

    if (method == "POST" &&
        target == "/api/v1/orders/cancel")
    {
        auto j = json::parse(body);

        return oms_.cancelOrder(
            token,
            j.at("order_id"));
    }

    if (target == "/api/v1/health")
    {
        return R"({"status":"ok"})";
    }

    return R"({"error":"route_not_found"})";
}