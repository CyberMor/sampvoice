/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Resource.h"

#include "Logger.h"

Resource::Resource(const HMODULE hModule, const DWORD rId, const LPCSTR rType)
{
    const auto hRsrc = FindResource(hModule, MAKEINTRESOURCE(rId), rType);
    if (hRsrc == nullptr)
    {
        Logger::LogToFile("[err:resource] : failed to find resource (code:%u)", GetLastError());
        throw std::exception();
    }

    const auto hGlobal = LoadResource(hModule, hRsrc);
    if (hGlobal == nullptr)
    {
        Logger::LogToFile("[err:resource] : failed to load resource (code:%u)", GetLastError());
        throw std::exception();
    }

    this->dataPtr = LockResource(hGlobal);
    if (this->dataPtr == nullptr)
    {
        Logger::LogToFile("[err:resource] : failed to get data pointer (code:%u)", GetLastError());
        throw std::exception();
    }

    this->dataSize = SizeofResource(hModule, hRsrc);
    if (this->dataSize == 0)
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
