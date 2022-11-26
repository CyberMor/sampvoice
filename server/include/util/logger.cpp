/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "logger.h"

bool Logger::Init(const char* const log_file, const logprintf_t log_func) noexcept
{
    assert(log_file != nullptr && *log_file != '\0');
    assert(log_func != nullptr);

    return _log_file == nullptr && _log_func == nullptr &&
          (_log_file = std::fopen(log_file, "w")) != nullptr &&
          (_log_func = log_func) != nullptr;
}

void Logger::Free() noexcept
{
    std::fclose(_log_file);

    _log_file = nullptr;
    _log_func = nullptr;
}

std::FILE*  Logger::_log_file = nullptr;
logprintf_t Logger::_log_func = nullptr;

std::mutex  Logger::_log_file_mutex;
std::mutex  Logger::_log_console_mutex;
