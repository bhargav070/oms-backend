#pragma once

#include "../auth/SessionManager.hpp"
#include "../domain/Order.hpp"

#include <memory>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class OMSService
{
public:
    explicit OMSService(SessionManager& sessions);

    std::string login(const json& req);

    std::string placeOrder(
        const std::string& token,
        const json& req);

    std::string cancelOrder(
        const std::string& token,
        const std::string& orderId);

private:
    Order parseOrder(const json& req);

private:
    SessionManager& sessions_;
};