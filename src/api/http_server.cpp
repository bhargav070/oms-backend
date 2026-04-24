#include "http_server.h"
#include "router.h"
#include <iostream>

HttpServer::HttpServer(int port) : port_(port) {}

void HttpServer::start() {
    std::cout << "HTTP Server running on port " << port_ << std::endl;

    while (true) {
        // real Beast server comes next phase
    }
}