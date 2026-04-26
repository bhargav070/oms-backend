#pragma once

#include "interfaces/IExchange.hpp"
#include "../../../config/settings.hpp"

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <mutex>
#include <chrono>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

class DeribitClient : public IExchange
{
public:
    DeribitClient(
        const std::string& apiKey,
        const std::string& apiSecret,
        bool isTestnet);
        
    std::string name() const override;

    bool login() override;
    bool isAuthenticated() const override;

    std::string placeOrder(const Order& order) override;
    std::string cancelOrder(const std::string& orderId) override;

    std::string modifyOrder(
        const std::string& orderId,
        const Order& order) override;

    std::string getOpenOrders() override;
    std::string getPositions() override;

private:
    std::string host_;
    std::string port_ = "443";

    std::string api_key_;
    std::string api_secret_;

    bool is_testnet_{true};

    std::string access_token_;

    std::chrono::steady_clock::time_point token_expiry_;

    mutable std::mutex auth_mutex_;

private:
    std::string sendRequest(
        const std::string& target,
        const std::string& body,
        bool authRequired);

    bool ensureAuthenticated();
    bool tokenExpired() const;
};