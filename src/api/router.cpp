#include "router.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Router::Router(OMSService& service)
    : service_(service) {}

std::string Router::handle(
    const std::string& method,
    const std::string& target,
    const std::string& body)
{
    try {
        if (method == "GET" &&
            target == "/api/v1/health")
        {
            return service_.health().dump();
        }

        json req =
            body.empty()
            ? json::object()
            : json::parse(body);

        if (method == "POST" &&
            target == "/api/v1/orders/place")
            return service_.placeOrder(req).dump();

        if (method == "POST" &&
            target == "/api/v1/orders/cancel")
            return service_.cancelOrder(req).dump();

        if (method == "POST" &&
            target == "/api/v1/orders/modify")
            return service_.modifyOrder(req).dump();

        return json{
            {"error", "route_not_found"}
        }.dump();
    }
    catch (const std::exception& e) {
        return json{
            {"error", e.what()}
        }.dump();
    }
}