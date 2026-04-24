add_executable(oms
    main.cpp
    service/OMSService.cpp
    adapters/deribit/DeribitClient.cpp
)

target_include_directories(oms PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(oms PRIVATE
    Boost::headers
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
)