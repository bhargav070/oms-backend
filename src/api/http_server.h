#pragma once

class HttpServer {
public:
    explicit HttpServer(int port);
    void start();

private:
    int port_;
};