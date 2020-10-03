/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Logger.h"

bool Logger::Init(const char* const fileName) noexcept
{
    const std::scoped_lock lock { Logger::logFileMutex };

    return !Logger::logFile && (Logger::logFile = fopen(fileName, "wt"));
}

void Logger::Free() noexcept
{
    const std::scoped_lock lock { Logger::logFileMutex };

    fclose(Logger::logFile);
    Logger::logFile = nullptr;
}

FILE* Logger::logFile { nullptr };

std::mutex Logger::logFileMutex;
std::mutex Logger::logChatMutex;
