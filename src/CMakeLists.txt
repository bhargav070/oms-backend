add_executable(oms
    main.cpp

    core/application.cpp

    api/http_server.cpp
    api/router.cpp

    utils/logger.cpp
)

target_link_libraries(oms PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)