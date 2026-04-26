#pragma once

#include "../service/OMSService.hpp"
#include <string>

class Router
{
public:
    explicit Router(OMSService& oms);

    std::string handle(
        const std::string& method,
        const std::string& target,
        const std::string& body,
        const std::string& authHeader);

private:
    OMSService& oms_;
};