#include "SessionManager.hpp"

#include <random>
#include <sstream>

std::string SessionManager::generateToken()
{
    static const char* chars =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 61);

    std::stringstream ss;
    ss << "sess_";

    for (int i = 0; i < 32; ++i)
        ss << chars[dist(gen)];

    return ss.str();
}

std::string
SessionManager::createSession(
    std::shared_ptr<IExchange> exchange)
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::string token = generateToken();
    sessions_[token] = exchange;

    return token;
}

std::shared_ptr<IExchange>
SessionManager::getSession(
    const std::string& token)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(token);

    if (it == sessions_.end())
        return nullptr;

    return it->second;
}

bool SessionManager::removeSession(
    const std::string& token)
{
    std::lock_guard<std::mutex> lock(mutex_);

    return sessions_.erase(token) > 0;
}