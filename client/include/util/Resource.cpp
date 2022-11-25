/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Resource.h"

#include <cassert>

#include "Logger.h"

Resource::Resource(const HMODULE module, const DWORD id, const LPCSTR type) noexcept
{
    const auto rsrc = FindResource(module, MAKEINTRESOURCE(id), type);
    if (rsrc == nullptr)
    {
        Logger::LogToFile("[err:resource] : failed to find resource (code:%u)", GetLastError());
        return;
    }

    const auto global = LoadResource(module, rsrc);
    if (global == nullptr)
    {
        Logger::LogToFile("[err:resource] : failed to load resource (code:%u)", GetLastError());
        return;
    }

    _data = LockResource(global);
    if (_data == nullptr)
    {
        Logger::LogToFile("[err:resource] : failed to get data pointer (code:%u)", GetLastError());
        return;
    }

    _size = SizeofResource(module, rsrc);
    if (_size == 0)
    {
        Logger::LogToFile("[err:resource] : failed to get data size (code:%u)", GetLastError());
        _data = nullptr;
        return;
    }
}

bool Resource::Valid() const noexcept
{
    return _data != nullptr && _size != 0;
}

LPVOID Resource::GetData() const noexcept
{
    assert(_data != nullptr);
    return _data;
}

DWORD Resource::GetSize() const noexcept
{
    assert(_size != 0);
    return _size;
}
