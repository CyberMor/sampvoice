/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>

// ----------------------------------------------------------------

#define CACHE_LINE 64

// ----------------------------------------------------------------

#define MakeUnique(Type, ...) std::unique_ptr<Type> { new (std::nothrow) Type { __VA_ARGS__ } }
#define MakeShared(Type, ...) std::shared_ptr<Type> { new (std::nothrow) Type { __VA_ARGS__ } }

// ----------------------------------------------------------------

#ifdef _WIN32
#define STDCALL  __stdcall
#define THISCALL __thiscall
#else
#define STDCALL
#define THISCALL
#endif
