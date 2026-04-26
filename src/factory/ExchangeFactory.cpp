#include "ExchangeFactory.hpp"
#include "../adapters/deribit/DeribitClient.hpp"

std::shared_ptr<IExchange>
ExchangeFactory::create(
    const LoginRequest& req)
{
    if (req.exchange == "deribit")
    {
        return std::make_shared<DeribitClient>(
            req.api_key,
            req.api_secret,
            req.is_testnet
        );
    }

    return nullptr;
}