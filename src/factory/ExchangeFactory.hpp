#pragma once

#include "../interfaces/IExchange.hpp"
#include <memory>
#include <string>

class ExchangeFactory {
public:
    static std::unique_ptr<IExchange>
    create(const std::string& exchange);
};