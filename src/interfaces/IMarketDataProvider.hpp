#pragma once

#include "../domain/MarketData.hpp"

#include <string>
#include <vector>

class IMarketDataProvider {
public:
    virtual ~IMarketDataProvider() = default;

    virtual std::string venue() const = 0;

    virtual std::vector<Instrument> instruments(
        const std::string& asset_class) = 0;

    virtual Quote quote(
        const std::string& instrument_id) = 0;

    virtual std::vector<Candle> candles(
        const std::string& instrument_id,
        const std::string& interval,
        std::int64_t from_ms,
        std::int64_t to_ms) = 0;
};
