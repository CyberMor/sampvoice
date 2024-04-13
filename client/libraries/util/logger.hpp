/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <functional>

#include <cassert>
#include <cstdio>
#include <ctime>

#include <d3d9.h>

#include <other/utils.hpp>

// ----------------------------------------------------------------

#define DefineColor(name, r, g, b)                                          \
                                                                            \
    constexpr D3DCOLOR k##name##Color =                                     \
        0xFF << 24 | ((r) & 0xFF) << 16 | ((g) & 0xFF) << 8 | ((b) & 0xFF); \
    constexpr const char k##name##ColorText[]                               \
    {                                                                       \
        utils::to_hex((r) >> 4), utils::to_hex((r) & 0b1111),               \
        utils::to_hex((g) >> 4), utils::to_hex((g) & 0b1111),               \
        utils::to_hex((b) >> 4), utils::to_hex((b) & 0b1111),               \
        '\0'                                                                \
    };

DefineColor(Debug,   0x6B, 0xBA, 0xED);
DefineColor(Success, 0x6B, 0xBF, 0x17);
DefineColor(Error,   0xE6, 0x5F, 0x39);
DefineColor(Info,    0xFF, 0xE0, 0xA5);

#undef DefineColor

// ----------------------------------------------------------------

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
        assert(path != nullptr);

        return (_log_file = (_log_file != nullptr ? std::freopen(path, "a", _log_file) : std::fopen(path, "a"))) != nullptr;
    }

    void Deinitialize() noexcept
    {
        if (_log_file != nullptr)
        {
            std::fclose(_log_file);
            _log_file = nullptr;
        }
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
    void LogToChat(const D3DCOLOR color, const cstr_t message, const Arguments... arguments) noexcept
    {
        assert(message != nullptr);

        if (OnLogToChat != nullptr)
        {
            char message_buffer[144];
            if (const int length = std::snprintf(message_buffer, sizeof(message_buffer), message, arguments...);
                length >= 0 && length < sizeof(message_buffer))
            {
                OnLogToChat(color, message_buffer);
            }
            else
            {
                LogToFile("[inf:logger:logtochat] message is too long to display in chat");
            }
        }
    }

    template <class... Arguments>
    void Log(const D3DCOLOR color, const cstr_t message, const Arguments... arguments) noexcept
    {
        LogToFile(message, arguments...);
        LogToChat(color, message, arguments...);
    }

public:

    std::function<void(D3DCOLOR, cstr_t)> OnLogToChat;

private:

    std::FILE* _log_file = nullptr;

};
