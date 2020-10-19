/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <ctime>
#include <iostream>
#include <algorithm>
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

class Logger {

    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

public:

    static bool Init(const char* fileName) noexcept;
    static bool Free() noexcept;

    template<class... ARGS>
    static bool LogToFile(const char* message, ARGS... args) noexcept
    {
        const std::scoped_lock lock { Logger::logFileMutex };

        if (Logger::logFile == nullptr)
            return false;

        const auto cTime = std::time(nullptr);
        const auto timeOfDay = std::localtime(&cTime);

        if (timeOfDay == nullptr)
            return false;

        std::fprintf(Logger::logFile, "[%.2d:%.2d:%.2d] : ",
            timeOfDay->tm_hour, timeOfDay->tm_min, timeOfDay->tm_sec);
        std::fprintf(Logger::logFile, message, args...);
        std::fputc('\n', Logger::logFile);
        std::fflush(Logger::logFile);

        return true;
    }

    template<class... ARGS>
    static bool LogToChat(D3DCOLOR color, const char* message, ARGS... args) noexcept
    {
        constexpr auto kMaxLengthMessage = 144;

        std::array<char, kMaxLengthMessage> messageBuffer;

        if (const auto length = std::snprintf(messageBuffer.data(), messageBuffer.size(), message, args...);
            length != std::clamp(length, 0, static_cast<int>(messageBuffer.size() - 1)))
        {
            Logger::LogToFile("[inf:logger:logtochat] : message is too long to display in chat");
            return false;
        }

        {
            const std::scoped_lock lock { Logger::logChatMutex };
            Samp::AddMessageToChat(color, messageBuffer.data());
        }

        return true;
    }

    template<class... ARGS>
    static inline void Log(D3DCOLOR color, const char* message, ARGS... args) noexcept
    {
        Logger::LogToFile(message, args...);
        Logger::LogToChat(color, message, args...);
    }

private:

    static std::FILE* logFile;

    static std::mutex logFileMutex;
    static std::mutex logChatMutex;

};
