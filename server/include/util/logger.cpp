/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "logger.h"

bool Logger::Init(const char* const logFile, const logprintf_t logFunc) noexcept
{
    if (!logFile || !*logFile || !logFunc) return false;

    const std::lock_guard<std::mutex> lockFile { Logger::logFileMutex };
    const std::lock_guard<std::mutex> lockConsole { Logger::logConsoleMutex };

    if (Logger::logFile || Logger::logFunc) return false;

    return (Logger::logFile = std::fopen(logFile, "wt")) &&
           (Logger::logFunc = logFunc);
}

void Logger::Free() noexcept
{
    const std::lock_guard<std::mutex> lockFile { Logger::logFileMutex };
    const std::lock_guard<std::mutex> lockConsole { Logger::logConsoleMutex };

    if (Logger::logFile) std::fclose(Logger::logFile);

    Logger::logFile = nullptr;
    Logger::logFunc = nullptr;
}

FILE* Logger::logFile { nullptr };
logprintf_t Logger::logFunc { nullptr };

std::mutex Logger::logFileMutex;
std::mutex Logger::logConsoleMutex;
