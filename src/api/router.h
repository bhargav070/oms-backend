#pragma once

#include "../service/OMSService.hpp"
#include <string>

class Router {
public:
    explicit Router(OMSService& service);

    std::string handle(
        const std::string& method,
        const std::string& target,
        const std::string& body
    );

private:
    OMSService& service_;
};