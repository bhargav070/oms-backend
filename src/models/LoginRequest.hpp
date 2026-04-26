#pragma once
#include <string>

struct LoginRequest
{
    std::string exchange;
    std::string api_key;
    std::string api_secret;
    bool is_testnet{true};
};