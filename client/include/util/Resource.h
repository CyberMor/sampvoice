/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>

struct Resource {

    Resource() noexcept = default;
    ~Resource() noexcept = default;
    Resource(const Resource&) noexcept = default;
    Resource(Resource&&) noexcept = default;
    Resource& operator=(const Resource&) noexcept = default;
    Resource& operator=(Resource&&) noexcept = default;

public:

    Resource(HMODULE module, DWORD id, LPCSTR type) noexcept

public:

    bool Valid() const noexcept;

public:

    LPVOID GetData() const noexcept;
    DWORD GetSize() const noexcept;

private:

    LPVOID _data = nullptr;
    DWORD  _size = 0;

};
