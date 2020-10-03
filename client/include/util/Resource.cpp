/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Resource.h"

#include "Logger.h"

Resource::Resource(const HMODULE hModule, const DWORD rId, const char* const rType)
{
    const auto hRsrc = FindResource(hModule, MAKEINTRESOURCE(rId), rType);

    if (!hRsrc)
    {
        Logger::LogToFile("[err:resource] : failed to find resource (code:%u)", GetLastError());
        throw std::exception();
    }

    if (!(this->resource = LoadResource(hModule, hRsrc)))
    {
        Logger::LogToFile("[err:resource] : failed to load resource handle (code:%u)", GetLastError());
        throw std::exception();
    }

    if (!(this->dataPtr = LockResource(this->resource)))
    {
        Logger::LogToFile("[err:resource] : failed to get data pointer (code:%u)", GetLastError());
        throw std::exception();
    }

    if (!(this->dataSize = SizeofResource(hModule, hRsrc)))
    {
        Logger::LogToFile("[err:resource] : failed to get data size (code:%u)", GetLastError());
        throw std::exception();
    }
}

LPVOID Resource::GetDataPtr() const noexcept
{
    return this->dataPtr;
}

DWORD Resource::GetDataSize() const noexcept
{
    return this->dataSize;
}
