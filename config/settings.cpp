#include "settings.hpp"
#include <cstdlib>
#include <stdexcept>

static std::string envOrThrow(const char* key) {
    const char* val = std::getenv(key);
    if (!val) throw std::runtime_error(std::string("Missing env: ") + key);
    return val;
}

static std::string envOrDefault(const char* key, const char* def) {
    const char* val = std::getenv(key);
    return val ? val : def;
}

AppConfig Settings::load() {
    AppConfig cfg;

    cfg.port = std::stoi(envOrDefault("APP_PORT", "8080"));

    cfg.deribit.api_key = envOrThrow("DERIBIT_API_KEY");
    cfg.deribit.api_secret = envOrThrow("DERIBIT_API_SECRET");
    cfg.deribit.host = envOrDefault("DERIBIT_HOST", "test.deribit.com");
    cfg.deribit.testnet =
        envOrDefault("DERIBIT_TESTNET", "true") == std::string("true");
    cfg.default_exchange = envOrDefault("DEFAULT_EXCHANGE", "deribit"); 
    return cfg;
}