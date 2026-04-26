#include "http_server.h"

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <thread>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

HttpServer::HttpServer(
    boost::asio::io_context& ioc,
    unsigned short port,
    Router& router)
    : ioc_(ioc),
      acceptor_(ioc,
      tcp::endpoint(tcp::v4(), port)),
      router_(router) {}

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

                std::string method =
                    std::string(req.method_string());

                std::string target =
                    std::string(req.target());

                std::string authHeader =
                    std::string(
                        req[http::field::authorization].data(),
                        req[http::field::authorization].size()
                    );

                auto body = router_.handle(
                    method,
                    target,
                    req.body(),
                    authHeader
                );

                http::response<http::string_body> res{
                    http::status::ok,
                    req.version()
                };

                res.set(
                    http::field::content_type,
                    "application/json"
                );

                res.set(
                    http::field::server,
                    "MultiExchangeOMS"
                );

                res.keep_alive(false);

                res.body() = body;
                res.prepare_payload();

                http::write(s, res);

                beast::error_code ec;
                s.shutdown(tcp::socket::shutdown_send, ec);
            }
            catch (...)
            {
            }
        }).detach();
    }
}