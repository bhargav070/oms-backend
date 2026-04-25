#include "ExchangeFactory.hpp"
#include "./adapters/deribit/DeribitClient.hpp"

std::shared_ptr<IExchange>
ExchangeFactory::create(
    const std::string& exchange,
    const AppConfig& cfg)
{
    if (exchange == "deribit")
    {
        return std::make_shared<DeribitClient>(
            cfg.deribit
        );
    }

    return nullptr;
}