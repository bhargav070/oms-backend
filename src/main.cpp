#include "../config/settings.hpp"
#include "../config/env_loader.hpp"
#include "factory/ExchangeFactory.hpp"

#include "auth/SessionManager.hpp"
#include "service/OMSService.hpp"
#include "api/router.h"
#include "api/http_server.h"

#include <boost/asio.hpp>
#include <iostream>

int main()
{
    try
    {
        EnvLoader::load();
        auto cfg = Settings::load();

        boost::asio::io_context ioc;

        SessionManager sessions;
        OMSService oms(sessions);

        Router router(oms);

        HttpServer server(
            ioc,
            cfg.port,
            router
        );

        std::cout
            << "OMS running on port "
            << cfg.port
            << "\n";

        server.run();
    }
    catch (const std::exception& e)
    {
        std::cout
            << "Fatal error: "
            << e.what()
            << "\n";
    }
}