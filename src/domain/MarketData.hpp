#pragma once

#include <cstdint>
#include <string>

struct Instrument {
    std::string id;
    std::string venue;
    std::string symbol;
    std::string asset_class;
    std::string quote_currency;
    double tick_size{0.0};
    double lot_size{0.0};
};

struct Quote {
    std::string instrument_id;
    std::int64_t timestamp_ms{0};
    double bid{0.0};
    double ask{0.0};
    double last{0.0};
    double bid_quantity{0.0};
    double ask_quantity{0.0};
};

struct Candle {
    std::string instrument_id;
    std::string interval;
    std::int64_t open_time_ms{0};
    double open{0.0};
    double high{0.0};
    double low{0.0};
    double close{0.0};
    double volume{0.0};
};
