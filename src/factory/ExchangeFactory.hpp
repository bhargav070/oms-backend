#pragma once

#include "interfaces/IExchange.hpp"
#include "../../config/settings.hpp"

#include <memory>
#include <string>

class ExchangeFactory
{
public:
    static std::shared_ptr<IExchange>
    create(
        const std::string& exchange,
        const AppConfig& cfg
    );
};