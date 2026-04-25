#include "env_loader.hpp"

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

static void setEnvVar(const std::string& key, const std::string& value)
{
#ifdef _WIN32
    _putenv_s(key.c_str(), value.c_str());
#else
    setenv(key.c_str(), value.c_str(), 1);
#endif
}

static std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");

    if (start == std::string::npos)
        return "";

    return s.substr(start, end - start + 1);
}

static bool parseFile(const fs::path& file)
{
    std::ifstream in(file);

    if (!in.is_open())
        return false;

    std::string line;

    while (std::getline(in, line))
    {
        line = trim(line);

        if (line.empty()) continue;
        if (line[0] == '#') continue;

        auto pos = line.find('=');

        if (pos == std::string::npos)
            continue;

        std::string key = trim(line.substr(0, pos));
        std::string val = trim(line.substr(pos + 1));

        if (!val.empty() && val.front() == '"')
            val.erase(0, 1);

        if (!val.empty() && val.back() == '"')
            val.pop_back();

        setEnvVar(key, val);
    }

    return true;
}

void EnvLoader::load()
{
    std::vector<fs::path> candidates = {
        ".env",
        "../.env",
        "../../.env",
        "../../../.env"
    };

    for (auto& path : candidates)
    {
        if (parseFile(path))
        {
            std::cout << ".env loaded from: "
                      << fs::absolute(path).string()
                      << "\n";
            return;
        }
    }

    std::cout << "No .env file found. Using system environment.\n";
}