#pragma once
#include <string>

struct DeribitConfig {
    std::string api_key;
    std::string api_secret;
    std::string host;
    bool testnet;
};

struct AppConfig {
    int port;
    std::string default_exchange;
    DeribitConfig deribit;
};

class Settings {
public:
    static AppConfig load();
};