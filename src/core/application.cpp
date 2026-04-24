#include "application.h"
#include "../api/http_server.h"
#include "../utils/logger.h"

void Application::run() {
    Logger::info("Starting OMS Backend...");

    HttpServer server(8080);
    server.start();
}