/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#include <sys/stat.h>
#endif

#include <system/types.hpp>

struct Scanner {

    Scanner() noexcept = default;
    ~Scanner() noexcept = default;
    Scanner(const Scanner&) noexcept = default;
    Scanner(Scanner&&) noexcept = default;
    Scanner& operator=(const Scanner&) noexcept = default;
    Scanner& operator=(Scanner&&) noexcept = default;

public:

    Scanner(const cptr_t address, const size_t length) noexcept
        : _address { address }
        , _length  { length }
    {}

public:

    bool Valid() const noexcept
    {
        return _address != nullptr && _length != 0;
    }

    bool Invalid() const noexcept
    {
        return _address == nullptr || _length == 0;
    }

public:

    bool Initialize(const cptr_t address, const size_t length) noexcept
    {
        return (_address = address) != nullptr &&
               (_length  = length)  != 0;
    }

    void Deinitialize() noexcept
    {
        _address = nullptr;
        _length  = 0;
    }

public:

    template <size_t Size>
    ptr_t Find(const char(&pattern)[Size], const char(&mask)[Size]) const noexcept
    {
        assert(_address != nullptr);

        if (_length >= Size - 1)
        {
            const auto compare = [&](const char* const sequence) noexcept -> bool
            {
                for (size_t i = 0; i != Size - 1; ++i)
                {
                    if (mask[i] == 'x' && pattern[i] != sequence[i])
                        return false;
                }

                return true;
            };

            const cstr_t border   = (static_cast<cstr_t>(_address) + _length) - (Size - 1);
                  cstr_t iterator =  static_cast<cstr_t>(_address);

            while (iterator != border)
            {
                if (compare(iterator))
                    return const_cast<str_t>(iterator);

                ++iterator;
            }
        }

        return nullptr;
    }

private:

    cptr_t _address = nullptr;
    size_t _length  = 0;

};

inline std::pair<cptr_t, size_t> GetModuleInfo(const cptr_t address) noexcept
{
    std::pair<cptr_t, size_t> result;
#ifdef _WIN32
    if (MEMORY_BASIC_INFORMATION info; VirtualQuery(address, &info, sizeof(info)) != 0)
    {
        if (info.AllocationBase != nullptr)
        {
            const auto dos = static_cast<const IMAGE_DOS_HEADER*>(info.AllocationBase);
            const auto pe = reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<size_t>(dos) + dos->e_lfanew);

            if (pe->Signature == IMAGE_NT_SIGNATURE)
            {
                result.first = info.AllocationBase;
                result.second = pe->OptionalHeader.SizeOfImage;
            }
        }
    }
#else
    if (Dl_info info; dladdr(address, &info) != 0)
    {
        if (struct stat buf; stat(info.dli_fname, &buf) == 0)
        {
            if (info.dli_fbase != nullptr)
            {
                result.first = info.dli_fbase;
                result.second = buf.st_size;
            }
        }
    }
#endif
    return result;
}
