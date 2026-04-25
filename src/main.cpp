#include "../config/settings.hpp"
#include "../config/env_loader.hpp"
#include "factory/ExchangeFactory.hpp"

#include "service/OMSService.hpp"
#include "api/router.h"
#include "api/http_server.h"

#include <boost/asio.hpp>
#include <iostream>

int main()
{
    try
    {
        boost::asio::io_context ioc;
        
        std::cout << "Loading config..." << std::endl;
        EnvLoader::load();
        auto cfg = Settings::load();

        std::cout << "Creating exchange..." << std::endl;
        auto exchange =
            ExchangeFactory::create(
                cfg.default_exchange,
                cfg
            );
        
        std::cout << "Logging in..." << std::endl;
        if (!exchange)
        {
            std::cerr << "Failed to create exchange\n";
            return 1;
        }

        if (!exchange->login())
        {
            std::cerr << "Login failed\n";
            return 1;
        }
        std::cout << "Login success" << std::endl;
        
        OMSService oms(exchange);
        Router router(oms);

        HttpServer server(
            ioc,
            cfg.port,
            router
        );

        server.run();

        std::cout << "OMS running on port " << cfg.port << std::endl;

        ioc.run();   // IMPORTANT

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: "
                  << e.what()
                  << "\n";
        return 1;
    }
}