#include "OMSService.hpp"

OMSService::OMSService(
    std::shared_ptr<IExchange> exchange)
    : exchange_(std::move(exchange)) {}

Order OMSService::parseOrder(const json& req)
{
    Order o;

    // symbol
    o.symbol = req.at("symbol").get<std::string>();

    // qty
    o.quantity = req.at("qty").get<double>();

    // optional price
    o.price = req.value("price", 0.0);

    // side
    std::string side = req.value("side", "buy");

    if (side == "buy")
        o.side = Side::Buy;
    else if (side == "sell")
        o.side = Side::Sell;
    else
        throw std::runtime_error("Invalid side");

    // type
    std::string type = req.value("type", "limit");

    if (type == "market")
        o.type = OrderType::Market;
    else if (type == "limit")
        o.type = OrderType::Limit;
    else
        throw std::runtime_error("Invalid order type");

    return o;
}

json OMSService::placeOrder(const json& req)
{
    Order o = parseOrder(req);

    auto resp = exchange_->placeOrder(o);

    return json::parse(resp);
}

json OMSService::cancelOrder(const json& req)
{
    auto id = req.at("order_id")
                .get<std::string>();

    auto resp =
        exchange_->cancelOrder(id);

    return json::parse(resp);
}

json OMSService::modifyOrder(const json& req)
{
    auto id = req.at("order_id")
                .get<std::string>();

    Order o = parseOrder(req);

    auto resp =
        exchange_->modifyOrder(id, o);

    return json::parse(resp);
}

json OMSService::health() const
{
    return {
        {"status", "ok"},
        {"service", "oms"},
        {"exchange", exchange_->name()}
    };
}