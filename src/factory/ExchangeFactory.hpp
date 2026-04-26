#pragma once

#include "../interfaces/IExchange.hpp"
#include "../models/LoginRequest.hpp"

#include <memory>
#include <string>

class ExchangeFactory
{
public:
    static std::shared_ptr<IExchange>
    create(const LoginRequest& req);
};