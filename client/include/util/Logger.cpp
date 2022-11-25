/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Logger.h"

bool Logger::Init(const char* const path) noexcept
{
    return _log_file == nullptr && (_log_file = std::fopen(path, "wt")) != nullptr;
}

void Logger::Free() noexcept
{
    std::fclose(_log_file);
    _log_file = nullptr;
}

std::FILE* Logger::_log_file = nullptr;

std::mutex Logger::_log_file_mutex;
std::mutex Logger::_log_chat_mutex;
