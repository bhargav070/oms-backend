#include "OMSService.hpp"

#include "../factory/ExchangeFactory.hpp"
#include "../models/LoginRequest.hpp"

OMSService::OMSService(
    SessionManager& sessions)
    : sessions_(sessions)
{
}

std::string OMSService::login(
    const json& req)
{
    LoginRequest r;

    r.exchange   = req.value("exchange", "deribit");
    r.api_key    = req.at("api_key").get<std::string>();
    r.api_secret = req.at("api_secret").get<std::string>();
    r.is_testnet = req.value("is_testnet", true);

    auto exchange =
        ExchangeFactory::create(r);

    if (!exchange)
        return R"({"error":"unsupported_exchange"})";

    if (!exchange->login())
        return R"({"error":"login_failed"})";

    std::string token =
        sessions_.createSession(exchange);

    json res = {
        {"success", true},
        {"session_token", token},
        {"exchange", exchange->name()}
    };

    return res.dump();
}

Order OMSService::parseOrder(
    const json& req)
{
    Order o;

    o.symbol = req.at("symbol");

    std::string side =
        req.value("side", "buy");

    o.side =
        side == "buy"
        ? Side::Buy
        : Side::Sell;

    std::string type =
        req.value("type", "limit");

    o.type =
        type == "market"
        ? OrderType::Market
        : OrderType::Limit;

    o.price =
        req.value("price", 0.0);

    o.quantity =
        req.at("qty");

    return o;
}

std::string OMSService::placeOrder(
    const std::string& token,
    const json& req)
{
    auto ex =
        sessions_.getSession(token);

    if (!ex)
        return R"({"error":"invalid_session"})";

    Order order = parseOrder(req);

    return ex->placeOrder(order);
}

std::string OMSService::cancelOrder(
    const std::string& token,
    const std::string& orderId)
{
    auto ex =
        sessions_.getSession(token);

    if (!ex)
        return R"({"error":"invalid_session"})";

    return ex->cancelOrder(orderId);
}