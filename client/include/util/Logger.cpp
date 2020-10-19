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

    if (Logger::logFile != nullptr)
        return false;

    Logger::logFile = std::fopen(fileName, "wt");

    if (Logger::logFile == nullptr)
        return false;

    return true;
}

bool Logger::Free() noexcept
{
    const std::scoped_lock lock { Logger::logFileMutex };

    if (std::fclose(Logger::logFile) == EOF)
        return false;

    Logger::logFile = nullptr;

    return true;
}

std::FILE* Logger::logFile { nullptr };

std::mutex Logger::logFileMutex;
std::mutex Logger::logChatMutex;
