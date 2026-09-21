#include "http_server.h"

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <thread>
#include "../utils/logger.h"

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

static bool isPrivateRoute(const std::string& target)
{
    return target.rfind("/api/v1/auth/exchange", 0) == 0 ||
           target.rfind("/api/v1/account/", 0) == 0 ||
           target.rfind("/api/v1/orders/", 0) == 0 ||
           target.rfind("/api/v1/positions", 0) == 0 ||
           target.rfind("/api/v1/order/", 0) == 0;
}

HttpServer::HttpServer(
    boost::asio::io_context& ioc,
    unsigned short port,
    Router& router)
    : ioc_(ioc),
      acceptor_(ioc, tcp::endpoint(tcp::v4(), port)),
      router_(router)
{
}

void HttpServer::run()
{
    for (;;)
    {
        tcp::socket socket(ioc_);
        acceptor_.accept(socket);

        std::thread(
            [s = std::move(socket), this]() mutable
        {
            try
            {
                beast::flat_buffer buffer;
                http::request<http::string_body> req;

                http::read(s, buffer, req);

                http::response<http::string_body> res{
                    http::status::ok,
                    req.version()
                };

                res.set(http::field::content_type, "application/json");

                // CORS HEADERS
                res.set("Access-Control-Allow-Origin", "*");
                res.set("Access-Control-Allow-Headers", "Content-Type, Authorization");
                res.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");

                // Handle preflight
                if (req.method() == http::verb::options)
                {
                    res.body() = "";
                    res.prepare_payload();
                    http::write(s, res);
                    return;
                }

                std::string authHeader =
                    std::string(req[http::field::authorization]);

                std::string method(req.method_string().data(),
                                req.method_string().size());

                std::string target(req.target().data(),
                                req.target().size());

                const bool privateRoute = isPrivateRoute(target);
                if (privateRoute)
                    Logger::info(
                        std::string("request ") + method + " " + target);

                std::string body = router_.handle(
                    method,
                    target,
                    req.body(),
                    authHeader
                );

                res.body() = body;
                if (body.find("\"error\"") != std::string::npos)
                    res.result(http::status::bad_request);
                if (privateRoute)
                {
                    const std::string status =
                        std::to_string(res.result_int());
                    const bool failed = res.result_int() >= 400;
                    Logger::info(
                        std::string("response ") + method + " " + target +
                        " status=" + status);
                    if (failed)
                        Logger::error(
                            std::string("private request rejected ") +
                            method + " " + target + " reason=" + body);
                }
                res.prepare_payload();

                http::write(s, res);
            }
            catch (const std::exception& e)
            {
                Logger::error(std::string("request failed: ") + e.what());
            }
        }).detach();
    }
}