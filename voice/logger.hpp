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

#include <mutex>

#include <system/types.hpp>

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

    bool Initialize(const cstr_t path) noexcept
    {
        assert(path != nullptr && *path != '\0');

        _log_file = (_log_file != nullptr ? std::freopen(path, "a", _log_file) : std::fopen(path, "a"));
        return _log_file != nullptr;
    }

    void Deinitialize() noexcept
    {
        if (_log_file != nullptr) std::fclose(_log_file);
        _log_file = nullptr;
    }

public:

    template <class... Arguments>
    void Log(const cstr_t message, const Arguments... arguments) noexcept
    {
        assert(message != nullptr);

        char buffer[1024];

        const auto c_time = std::time(nullptr);
        const auto time_of_day = std::localtime(&c_time);
        assert(time_of_day != nullptr);

        std::sprintf(buffer, "[%.02d.%.02d.%.04d](%.02d:%.02d:%.02d) ",
            time_of_day->tm_mday, time_of_day->tm_mon + 1, time_of_day->tm_year + 1900,
            time_of_day->tm_hour, time_of_day->tm_min, time_of_day->tm_sec);

        std::sprintf(buffer + 23, message, arguments...);

        if (_log_file != nullptr)
        {
            _log_lock.lock();

            std::fputs(buffer, _log_file);
            std::fputc('\n', _log_file);
            std::fflush(_log_file);

            _log_lock.unlock();
        }

        std::puts(buffer);
    }

private:

    std::FILE* _log_file = nullptr;
    std::mutex _log_lock;

};
