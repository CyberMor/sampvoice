/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <cstdio>
#include <ctime>

#include <system/types.hpp>

#include <ysf/globals.h>

struct Logger {

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

private:

    Logger() noexcept = default;
    ~Logger() noexcept = default;

public:

    static Logger& Instance() noexcept
    {
        static Logger instance;
        return instance;
    }

public:

    bool Initialize(const cstr_t log_file, const logprintf_t log_func) noexcept
    {
        assert(log_file != nullptr && *log_file != '\0');
        assert(log_func != nullptr);

        _log_file = (_log_file != nullptr ? std::freopen(log_file, "a", _log_file) : std::fopen(log_file, "a"));
        if (_log_file != nullptr) _log_func = log_func;
        return _log_file != nullptr;
    }

    void Deinitialize() noexcept
    {
        if (_log_file != nullptr) std::fclose(_log_file);

        _log_file = nullptr;
        _log_func = nullptr;
    }

public:

    template <class... Arguments>
    void LogToFile(const cstr_t message, const Arguments... arguments) noexcept
    {
        assert(message != nullptr);

        if (_log_file != nullptr)
        {
            const auto c_time = std::time(nullptr);
            const auto time_of_day = std::localtime(&c_time);
            assert(time_of_day != nullptr);

            std::fprintf(_log_file, "[%.02d.%.02d.%.04d](%.02d:%.02d:%.02d) ",
                time_of_day->tm_mday, time_of_day->tm_mon + 1, time_of_day->tm_year + 1900,
                time_of_day->tm_hour, time_of_day->tm_min, time_of_day->tm_sec);
            std::fprintf(_log_file, message, arguments...);
            std::fputc('\n', _log_file);
            std::fflush(_log_file);
        }
    }

    template <class... Arguments>
    void LogToConsole(const cstr_t message, const Arguments... arguments) noexcept
    {
        assert(message != nullptr);

        if (_log_func != nullptr)
        {
            _log_func(message, arguments...);
        }
    }

    template <class... Arguments>
    void Log(const cstr_t message, const Arguments... arguments) noexcept
    {
        LogToFile(message, arguments...);
        LogToConsole(message, arguments...);
    }

private:

    std::FILE*  _log_file = nullptr;
    logprintf_t _log_func = nullptr;

};
