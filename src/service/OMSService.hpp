#pragma once

#include "../auth/SessionManager.hpp"
#include "../domain/Order.hpp"
#include "../../config/settings.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class OMSService
{
public:
    OMSService(
        SessionManager& sessions,
        const AppConfig& config);

    std::string login(const json& req);

    std::string connectConfiguredAccount();

    std::string accountSummary(
        const std::string& token,
        const std::string& currency);

    std::string openOrders(
        const std::string& token);

    std::string orderHistory(
        const std::string& token);

    std::string positions(
        const std::string& token);

    std::string placeOrder(
        const std::string& token,
        const json& req);

    std::string cancelOrder(
        const std::string& token,
        const std::string& orderId);

    std::string marketTicker(
        const std::string& instrument);

    std::string marketCandles(
        const std::string& instrument,
        const std::string& resolution,
        std::int64_t startMs,
        std::int64_t endMs);

private:
    Order parseOrder(const json& req);

    void recordOrderEvent(const json& event);

private:
    SessionManager& sessions_;
    AppConfig config_;
    std::mutex order_history_mutex_;
    std::vector<json> order_history_;
};