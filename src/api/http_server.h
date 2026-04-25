#pragma once

#include "router.h"
#include <boost/asio.hpp>

class HttpServer {
public:
    HttpServer(
        boost::asio::io_context& ioc,
        unsigned short port,
        Router& router
    );

    void run();

private:
    boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    Router& router_;
};