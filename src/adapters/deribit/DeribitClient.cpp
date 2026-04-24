#include "DeribitClient.hpp"
#include <nlohmann/json.hpp>
#include <openssl/ssl.h>
#include <openssl/err.h>

using json = nlohmann::json;

DeribitClient::DeribitClient(
    const std::string& clientId,
    const std::string& clientSecret)
    : client_id_(clientId),
      client_secret_(clientSecret)
{
}

std::string DeribitClient::name() const {
    return "Deribit";
}

bool DeribitClient::tokenExpired() const {
    return std::chrono::steady_clock::now() >= token_expiry_;
}

bool DeribitClient::isAuthenticated() const {
    return !access_token_.empty() && !tokenExpired();
}

bool DeribitClient::ensureAuthenticated() {
    std::lock_guard<std::mutex> lock(auth_mutex_);

    if (isAuthenticated())
        return true;

    return login();
}

bool DeribitClient::login()
{
    json body = {
        {"jsonrpc","2.0"},
        {"id",1},
        {"method","public/auth"},
        {"params",{
            {"grant_type","client_credentials"},
            {"client_id",client_id_},
            {"client_secret",client_secret_}
        }}
    };

    std::string resp =
        sendRequest("/api/v2", body.dump(), false);

    auto j = json::parse(resp, nullptr, false);

    if (j.is_discarded() || !j.contains("result"))
        return false;

    access_token_ = j["result"]["access_token"];

    int expires =
        j["result"].value("expires_in", 3600);

    token_expiry_ =
        std::chrono::steady_clock::now() +
        std::chrono::seconds(expires - 30);

    return true;
}

std::string DeribitClient::sendRequest(
    const std::string& target,
    const std::string& body,
    bool authRequired)
{
    try {
        net::io_context ioc;

        ssl::context ctx(ssl::context::tls_client);
        ctx.set_verify_mode(ssl::verify_none);

        tcp::resolver resolver(ioc);
        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

        SSL_set_tlsext_host_name(
            stream.native_handle(),
            host_.c_str());

        auto results =
            resolver.resolve(host_, port_);

        beast::get_lowest_layer(stream).connect(results);

        stream.handshake(ssl::stream_base::client);

        http::request<http::string_body> req{
            http::verb::post,
            target,
            11
        };

        req.set(http::field::host, host_);
        req.set(http::field::content_type, "application/json");

        if (authRequired)
            req.set(http::field::authorization,
                "Bearer " + access_token_);

        req.body() = body;
        req.prepare_payload();

        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;

        http::read(stream, buffer, res);

        beast::error_code ec;
        stream.shutdown(ec);

        return res.body();
    }
    catch (std::exception& e) {
        return std::string("{\"error\":\"") + e.what() + "\"}";
    }
}

std::string DeribitClient::placeOrder(const Order& order)
{
    if (!ensureAuthenticated())
        return R"({"error":"login_failed"})";

    json body = {
        {"jsonrpc","2.0"},
        {"id",2},
        {"method",
            order.side == Side::Buy
            ? "private/buy"
            : "private/sell"},
        {"params",{
            {"instrument_name",order.symbol},
            {"amount",order.quantity},
            {"type",
                order.type == OrderType::Market
                ? "market"
                : "limit"},
            {"price",order.price}
        }}
    };

    return sendRequest("/api/v2", body.dump(), true);
}

std::string DeribitClient::cancelOrder(
    const std::string& orderId)
{
    if (!ensureAuthenticated())
        return R"({"error":"login_failed"})";

    json body = {
        {"jsonrpc","2.0"},
        {"id",3},
        {"method","private/cancel"},
        {"params",{
            {"order_id",orderId}
        }}
    };

    return sendRequest("/api/v2", body.dump(), true);
}