/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>

#include <Windows.h>

class Resource {

    Resource() = delete;
    Resource(const Resource&) = delete;
    Resource(Resource&&) = delete;
    Resource& operator=(const Resource&) = delete;
    Resource& operator=(Resource&&) = delete;

public:

    explicit Resource(HMODULE hModule, DWORD rId, const char* rType);

    ~Resource() noexcept = default;

public:

    LPVOID GetDataPtr() const noexcept;
    DWORD GetDataSize() const noexcept;

private:

    HGLOBAL resource { nullptr };
    LPVOID dataPtr { nullptr };
    DWORD dataSize { 0 };

};

using ResourcePtr = std::shared_ptr<Resource>;
#define MakeResource std::make_shared<Resource>
