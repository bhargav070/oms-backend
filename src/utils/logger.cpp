#include "logger.h"
#include <iostream>

void Logger::info(const std::string& msg) {
    std::cout << "[INFO] " << msg << std::endl;
}