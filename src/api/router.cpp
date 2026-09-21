#include "router.h"

#include <nlohmann/json.hpp>
#include <chrono>

using json = nlohmann::json;

static std::string queryValue(
    const std::string& target,
    const std::string& key,
    const std::string& fallback)
{
    const auto queryStart = target.find('?');
    if (queryStart == std::string::npos)
        return fallback;

    const std::string query = target.substr(queryStart + 1);
    const std::string prefix = key + "=";
    std::size_t start = 0;

    while (start < query.size())
    {
        const auto end = query.find('&', start);
        const auto part = query.substr(start, end - start);
        if (part.rfind(prefix, 0) == 0)
            return part.substr(prefix.size());
        if (end == std::string::npos)
            break;
        start = end + 1;
    }

    return fallback;
}

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
    if (method == "GET" &&
        target.rfind("/api/v1/market/ticker", 0) == 0)
    {
        return oms_.marketTicker(
            queryValue(target, "instrument", "BTC-PERPETUAL"));
    }

    if (method == "GET" &&
        target.rfind("/api/v1/market/candles", 0) == 0)
    {
        const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        const auto endMs = std::stoll(queryValue(target, "to", std::to_string(now)));
        const auto startMs = std::stoll(queryValue(
            target,
            "from",
            std::to_string(endMs - 24LL * 60LL * 60LL * 1000LL)));

        return oms_.marketCandles(
            queryValue(target, "instrument", "BTC-PERPETUAL"),
            queryValue(target, "resolution", "15"),
            startMs,
            endMs);
    }

    if (method == "POST" &&
        target == "/api/v1/auth/login")
    {
        return oms_.login(
            json::parse(body));
    }

    if (method == "POST" &&
        target == "/api/v1/auth/exchange")
    {
        return oms_.connectConfiguredAccount();
    }

    std::string token;

    if (auth.rfind("Bearer ", 0) == 0)
        token = auth.substr(7);

    if (method == "POST" &&
        target == "/api/v1/order/place")
    {
        return oms_.placeOrder(
            token,
            json::parse(body));
    }

    if (method == "GET" &&
        target.rfind("/api/v1/account/summary", 0) == 0)
        return oms_.accountSummary(token, queryValue(target, "currency", "BTC"));

    if (method == "GET" && target == "/api/v1/orders/open")
        return oms_.openOrders(token);

    if (method == "GET" && target == "/api/v1/orders/history")
        return oms_.orderHistory(token);

    if (method == "GET" && target == "/api/v1/positions")
        return oms_.positions(token);

    if (method == "POST" &&
        target == "/api/v1/order/cancel")
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