#pragma once

#include "../interfaces/IExchange.hpp"
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class OMSService {
public:
    explicit OMSService(std::shared_ptr<IExchange> exchange);

    json placeOrder(const json& req);
    json cancelOrder(const json& req);
    json modifyOrder(const json& req);
    json health() const;

private:
    std::shared_ptr<IExchange> exchange_;

    Order parseOrder(const json& req);
};