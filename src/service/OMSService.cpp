#include "OMSService.hpp"

#include "../factory/ExchangeFactory.hpp"
#include "../adapters/deribit/DeribitClient.hpp"
#include "../models/LoginRequest.hpp"

OMSService::OMSService(
        SessionManager& sessions,
        const AppConfig& config)
        : sessions_(sessions),
            config_(config)
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

std::string OMSService::connectConfiguredAccount()
{
    LoginRequest request;
    request.exchange = config_.default_exchange;
    request.api_key = config_.deribit.api_key;
    request.api_secret = config_.deribit.api_secret;
    request.is_testnet = config_.deribit.testnet;

    auto exchange = ExchangeFactory::create(request);
    if (!exchange || !exchange->login())
        return R"({"error":"configured_account_login_failed"})";

    return json{
        {"success", true},
        {"session_token", sessions_.createSession(exchange)},
        {"exchange", exchange->name()},
        {"environment", config_.deribit.testnet ? "testnet" : "mainnet"}
    }.dump();
}

std::string OMSService::accountSummary(
    const std::string& token,
    const std::string& currency)
{
    auto exchange = sessions_.getSession(token);
    if (!exchange)
        return R"({"error":"invalid_session"})";
    return exchange->getAccountSummary(currency);
}

std::string OMSService::openOrders(
    const std::string& token)
{
    auto exchange = sessions_.getSession(token);
    if (!exchange)
        return R"({"error":"invalid_session"})";
    return exchange->getOpenOrders();
}

std::string OMSService::positions(
    const std::string& token)
{
    auto exchange = sessions_.getSession(token);
    if (!exchange)
        return R"({"error":"invalid_session"})";
    return exchange->getPositions();
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
        req.contains("quantity")
        ? req.at("quantity")
        : req.at("qty");

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

    const auto response = ex->placeOrder(order);
    const auto parsed = json::parse(response, nullptr, false);
    if (!parsed.is_discarded())
    {
        if (parsed.contains("result") && parsed["result"].contains("order"))
        {
            auto event = parsed["result"]["order"];
            event["created_at"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
            recordOrderEvent(event);
        }
        else if (parsed.contains("error"))
        {
            const auto error = parsed["error"];
            json event = {
                {"instrument_name", order.symbol},
                {"direction", order.side == Side::Buy ? "buy" : "sell"},
                {"amount", order.quantity},
                {"price", order.price},
                {"order_state", "rejected"},
                {"reject_reason", error.value("data", json::object()).value("reason", error.value("message", "Order rejected"))},
                {"error_code", error.value("code", 0)},
                {"created_at", std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()).count()}
            };
            recordOrderEvent(event);
        }
    }
    return response;
}

std::string OMSService::cancelOrder(
    const std::string& token,
    const std::string& orderId)
{
    auto ex =
        sessions_.getSession(token);

    if (!ex)
        return R"({"error":"invalid_session"})";

    const auto response = ex->cancelOrder(orderId);
    const auto parsed = json::parse(response, nullptr, false);
    if (!parsed.is_discarded() && parsed.contains("result"))
    {
        auto event = parsed["result"];
        event["order_id"] = orderId;
        event["order_state"] = "cancelled";
        event["created_at"] = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        recordOrderEvent(event);
    }
    return response;
}

void OMSService::recordOrderEvent(const json& event)
{
    std::lock_guard<std::mutex> lock(order_history_mutex_);
    order_history_.push_back(event);
    if (order_history_.size() > 200)
        order_history_.erase(order_history_.begin());
}

std::string OMSService::orderHistory(const std::string& token)
{
    auto exchange = sessions_.getSession(token);
    if (!exchange)
        return R"({"error":{"message":"Invalid session","code":"INVALID_SESSION"}})";

    json history = json::array();
    const auto remote = json::parse(exchange->getOrderHistory(), nullptr, false);
    if (!remote.is_discarded() && remote.contains("result") && remote["result"].contains("order_history"))
        history = remote["result"]["order_history"];

    {
        std::lock_guard<std::mutex> lock(order_history_mutex_);
        for (const auto& event : order_history_)
            history.push_back(event);
    }

    return json{{"jsonrpc", "2.0"}, {"result", {{"order_history", history}}}}.dump();
}

std::string OMSService::marketTicker(
    const std::string& instrument)
{
    DeribitClient client(
        config_.deribit.api_key,
        config_.deribit.api_secret,
        config_.deribit.testnet);

    return client.getTicker(instrument);
}

std::string OMSService::marketCandles(
    const std::string& instrument,
    const std::string& resolution,
    std::int64_t startMs,
    std::int64_t endMs)
{
    DeribitClient client(
        config_.deribit.api_key,
        config_.deribit.api_secret,
        config_.deribit.testnet);

    return client.getCandles(
        instrument,
        resolution,
        startMs,
        endMs);
}