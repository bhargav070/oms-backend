#include "router.h"

std::string Router::route(const std::string& path) {
    if (path == "/health")
        return R"({"status":"ok"})";

    return R"({"error":"not found"})";
}