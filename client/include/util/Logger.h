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
#include <array>

#include <d3d9.h>

#include "Samp.h"

#define DefineColor(name, rgb) \
constexpr auto k##name##Color = 0xff##rgb##u; \
constexpr auto k##name##ColorStr = #rgb;

DefineColor(Debug,   6bbaed);
DefineColor(Success, 6bbf17);
DefineColor(Error,   e65f39);
DefineColor(Info,    ffe0a5);

#undef DefineColor

struct Logger {

    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

public:

    static bool Init(const char* path) noexcept;
    static void Free() noexcept;

public:

    template <class... ARGS>
    static bool LogToFile(const char* const message, const ARGS... args) noexcept
    {
        assert(_log_file != nullptr);

        const std::lock_guard lock { _log_file_mutex };

        const auto c_time = std::time(nullptr);
        const auto time_of_day = std::localtime(&c_time);
        assert(time_of_day != nullptr);

        std::fprintf(_log_file, "[%.2d:%.2d:%.2d] : ",
            time_of_day->tm_hour, time_of_day->tm_min, time_of_day->tm_sec);
        std::fprintf(_log_file, message, args...);
        std::fputc('\n', _log_file);
        std::fflush(_log_file);

        return true;
    }

    template <class... ARGS>
    static bool LogToChat(const D3DCOLOR color, const char* const message, const ARGS... args) noexcept
    {
        constexpr size_t kMaxLengthMessage = 144;
        std::array<char, kMaxLengthMessage> message_buffer;

        if (const auto length = std::snprintf(message_buffer.data(), message_buffer.size(), message, args...);
            length < 0 || length >= static_cast<int>(message_buffer.size()))
        {
            LogToFile("[inf:logger:logtochat] : message is too long to display in chat");
            return false;
        }

        {
            const std::lock_guard lock { _log_chat_mutex };
            Samp::AddMessageToChat(color, message_buffer.data());
        }

        return true;
    }

    template <class... ARGS>
    static void Log(const D3DCOLOR color, const char* const message, const ARGS... args) noexcept
    {
        LogToFile(message, args...);
        LogToChat(color, message, args...);
    }

private:

    static std::FILE* _log_file;

    static std::mutex _log_file_mutex;
    static std::mutex _log_chat_mutex;

};
