add_executable(oms
    main.cpp
    ../config/settings.cpp
    ../config/env_loader.cpp
    service/OMSService.cpp
    adapters/deribit/DeribitClient.cpp
    factory/ExchangeFactory.cpp
    api/http_server.cpp
    api/router.cpp
    auth/SessionManager.cpp
)

target_include_directories(oms PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(oms PRIVATE
    Boost::headers
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
)