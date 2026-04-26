#pragma once

#include "../interfaces/IExchange.hpp"

#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>

class SessionManager
{
public:
    std::string createSession(std::shared_ptr<IExchange> exchange);

    std::shared_ptr<IExchange>
    getSession(const std::string& token);

    bool removeSession(const std::string& token);

private:
    std::string generateToken();

private:
    std::unordered_map<
        std::string,
        std::shared_ptr<IExchange>
    > sessions_;

    std::mutex mutex_;
};