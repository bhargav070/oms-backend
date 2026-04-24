#pragma once

#include "../../interfaces/IExchange.hpp"

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

class DeribitClient : public IExchange {
public:
    DeribitClient(
        const std::string& clientId,
        const std::string& clientSecret);

    std::string name() const override;

    bool login() override;
    bool isAuthenticated() const override;

    std::string placeOrder(const Order& order) override;
    std::string cancelOrder(const std::string& orderId) override;

private:
    std::string host_ = "test.deribit.com";
    std::string port_ = "443";

    std::string client_id_;
    std::string client_secret_;

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