/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <map>
#include <string>

class Config {

    Config() = delete;
    ~Config() = delete;
    Config(const Config&) = delete;
    Config(Config&&) = delete;
    Config& operator=(const Config&) = delete;
    Config& operator=(Config&&) = delete;

public:

    static bool Load() noexcept;

public:

    static int AsInteger(const std::string& parameter, int default_value = -1) noexcept;
    static const char* AsString(const std::string& parameter, const char* default_value = nullptr) noexcept;

private:

    static std::map<std::string, std::string> parameters;

};
