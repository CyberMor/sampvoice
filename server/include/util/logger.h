/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <cstdio>
#include <ctime>

#include <mutex>

#include <ysf/globals.h>

struct Logger {

    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

public:

    static bool Init(const char* log_file, logprintf_t log_func) noexcept;
    static void Free() noexcept;

public:

    template <class... ARGS>
    static void LogToFile(const char* const message, const ARGS... args) noexcept
    {
        assert(message != nullptr);

        const std::lock_guard lock { _log_file_mutex };

        assert(_log_file != nullptr);

        const auto c_time = std::time(nullptr);
        const auto time_of_day = std::localtime(&c_time);
        assert(time_of_day != nullptr);

        std::fprintf(_log_file, "[%.2d:%.2d:%.2d] : ",
            time_of_day->tm_hour, time_of_day->tm_min, time_of_day->tm_sec);
        std::fprintf(_log_file, message, args...);
        std::fputc('\n', _log_file);
        std::fflush(_log_file);
    }

    template <class... ARGS>
    static void LogToConsole(const char* const message, const ARGS... args) noexcept
    {
        assert(message != nullptr);

        const std::lock_guard lock { _log_console_mutex };

        assert(_log_func != nullptr);

        _log_func(message, args...);
    }

    template <class... ARGS>
    static void Log(const char* const message, const ARGS... args) noexcept
    {
        LogToFile(message, args...);
        LogToConsole(message, args...);
    }

private:

    static std::FILE*  _log_file;
    static logprintf_t _log_func;

    static std::mutex  _log_file_mutex;
    static std::mutex  _log_console_mutex;

};
