/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Config.h"

#include <cstdio>

bool Config::Load() noexcept
{
    const auto cfg_file = std::fopen("server.cfg", "r");
    if (cfg_file == nullptr) return false;

    char cfg_line[512];

    while (std::fgets(cfg_line, sizeof(cfg_line), cfg_file) != nullptr)
    {
        const char* cfg_iter = cfg_line;

        while (*cfg_iter == ' ' || *cfg_iter == '\t')
            ++cfg_iter;

        const auto parameter_begin = cfg_iter;

        while (*cfg_iter != ' ' && *cfg_iter != '\t' && *cfg_iter != '=' && *cfg_iter != '\n' && *cfg_iter != '\0')
            ++cfg_iter;

        const auto parameter_end = cfg_iter;

        while (*cfg_iter == ' ' || *cfg_iter == '\t')
            ++cfg_iter;

        if (*cfg_iter != '=')
        {
            std::fclose(cfg_file);
            return false;
        }

        ++cfg_iter;

        while (*cfg_iter == ' ' || *cfg_iter == '\t')
            ++cfg_iter;

        const auto value_begin = cfg_iter;

        while (*cfg_iter != ' ' && *cfg_iter != '\t' && *cfg_iter != '\n' && *cfg_iter != '\0')
            ++cfg_iter;

        const auto value_end = cfg_iter;

        Config::parameters.emplace(std::make_pair(std::string(parameter_begin, parameter_end - parameter_begin),
            std::string(value_begin, value_end - value_begin)));
    }

    const bool result = std::ferror(cfg_file) == 0;

    std::fclose(cfg_file);

    return result;
}

int Config::AsInteger(const std::string& parameter, int default_value) noexcept
{
    if (const auto iterator = Config::parameters.find(parameter); iterator != Config::parameters.end())
    {
        if (const auto value = std::atoi(iterator->second.c_str()); value != 0 || *iterator->second.c_str() == '0')
        {
            return value;
        }
    }

    return default_value;
}

const char* Config::AsString(const std::string& parameter, const char* default_value) noexcept
{
    if (const auto iterator = Config::parameters.find(parameter); iterator != Config::parameters.end())
    {
        return iterator->second.c_str();
    }

    return default_value;
}

std::map<std::string, std::string> Config::parameters;
