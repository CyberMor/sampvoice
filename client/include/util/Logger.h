/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <mutex>
#include <iostream>
#include <ctime>

#include <d3d9.h>

#include "Samp.h"

#define kStringErrorColor "e65f39"
#define kStringInfoColor "ffe0a5"
#define kStringSuccessColor "6bbf17"
#define kStringDebugColor "6bbaed"

constexpr DWORD kErrorColor = 0xffe65f39;
constexpr DWORD kInfoColor = 0xffffe0a5;
constexpr DWORD kSuccessColor = 0xff6bbf17;
constexpr DWORD kDebugColor = 0xff6bbaed;

class Logger {

    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

public:

    static bool Init(const char* fileName) noexcept;
    static void Free() noexcept;

    template<class... ARGS>
    static bool LogToFile(const char* const message, const ARGS... args) noexcept
    {
        const std::scoped_lock lock { Logger::logFileMutex };

        if (!Logger::logFile) return false;

        const auto cTime = std::time(nullptr);
        const auto timeOfDay = std::localtime(&cTime);

        if (!timeOfDay) return false;

        std::fprintf(Logger::logFile, "[%.2d:%.2d:%.2d] : ",
            timeOfDay->tm_hour, timeOfDay->tm_min, timeOfDay->tm_sec);
        std::fprintf(Logger::logFile, message, args...);
        std::fputc('\n', Logger::logFile);
        std::fflush(Logger::logFile);

        return true;
    }

    template<class... ARGS>
    static bool LogToChat(const D3DCOLOR color, const char* const message, const ARGS... args) noexcept
    {
        constexpr auto MaxLengthMessageToChat = 144;

        char messageBuffer[MaxLengthMessageToChat];

        if (_snprintf(messageBuffer, sizeof(messageBuffer), message, args...) == -1)
        {
            Logger::LogToFile("[inf:logger:logtochat] : message is too long to display in chat");
            return false;
        }

        {
            const std::scoped_lock lock { Logger::logChatMutex };
            Samp::AddMessageToChat(color, messageBuffer);
        }

        return true;
    }

    template<class... ARGS>
    static inline void Log(const D3DCOLOR color, const char* const message, const ARGS... args) noexcept
    {
        Logger::LogToFile(message, args...);
        Logger::LogToChat(color, message, args...);
    }

private:

    static FILE* logFile;

    static std::mutex logFileMutex;
    static std::mutex logChatMutex;

};
