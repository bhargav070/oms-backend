#include "ExchangeFactory.hpp"
#include "../adapters/deribit/DeribitClient.hpp"

std::unique_ptr<IExchange>
ExchangeFactory::create(const std::string& exchange)
{
    if (exchange == "deribit")
    {
        return std::make_unique<DeribitClient>(
            "0TNl3dx4",
            "bQE6syR1WNRF4d6rw1x3DDS_VrYP8EVTR0qfWbRHTSg"
        );
    }

    return nullptr;
}