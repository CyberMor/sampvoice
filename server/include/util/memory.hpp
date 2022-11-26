/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <type_traits>
#include <iostream>
#include <cassert>
#include <cstring>
#include <fstream>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#define STDCALL  __stdcall
#define THISCALL __thiscall
#else
#define STDCALL
#define THISCALL
#endif

#define GetArraySize(array) (sizeof(array) / sizeof(*(array)))

namespace Memory
{
    template <class Type>
    struct ObjectContainer {

        ObjectContainer() noexcept = default;
        ~ObjectContainer() noexcept = default;
        ObjectContainer(const ObjectContainer&) = default;
        ObjectContainer(ObjectContainer&&) noexcept = default;
        ObjectContainer& operator=(const ObjectContainer&) = default;
        ObjectContainer& operator=(ObjectContainer&&) noexcept = default;

    public:

        ObjectContainer(const size_t extra)
            : _bytes ( sizeof(Type) + extra )
        {}

        ObjectContainer(const void* addr, const size_t size)
            : _bytes ( size )
        {
            assert(addr != nullptr);
            assert(size >= sizeof(Type));

            std::memcpy(_bytes.data(), addr, size);
        }

    public:

        const Type* operator->() const noexcept
        {
            return reinterpret_cast<const Type*>(_bytes.data());
        }

        Type* operator->() noexcept
        {
            return reinterpret_cast<Type*>(_bytes.data());
        }

    public:

        const Type& operator*() const noexcept
        {
            return *reinterpret_cast<const Type*>(_bytes.data());
        }

        Type& operator*() noexcept
        {
            return *reinterpret_cast<Type*>(_bytes.data());
        }

    public:

        const void* GetData() const noexcept
        {
            return _bytes.data();
        }

        void* GetData() noexcept
        {
            return _bytes.data();
        }

    public:

        size_t GetSize() const noexcept
        {
            return _bytes.size();
        }

    private:

        std::vector<uint8_t> _bytes;

    };

    template <size_t Size>
    struct UnprotectScope {

        static_assert(Size != 0, "'Size' must not be zero");

    public:

        UnprotectScope() noexcept = default;
        ~UnprotectScope() noexcept
        {
            Disable();
        }

        UnprotectScope(const UnprotectScope&) = delete;
        UnprotectScope(UnprotectScope&& object) noexcept
            : _addr    { object._addr }
            , _enabled { object._enabled }
#ifdef _WIN32
            , _protect { object._protect }
#endif
        {
            object._addr    = nullptr;
            object._enabled = false;
        }

        UnprotectScope& operator=(const UnprotectScope&) = delete;
        UnprotectScope& operator=(UnprotectScope&& object) noexcept
        {
            if (&object != this)
            {
                Disable();

                _addr    = object._addr;
                _enabled = object._enabled;
#ifdef _WIN32
                _protect = object._protect;
#endif

                object._addr    = nullptr;
                object._enabled = false;
            }

            return *this;
        }

    public:

        UnprotectScope(void* const addr, const bool enabled = true) noexcept
            : _addr { addr }
        {
            if (enabled) Enable();
        }

    public:

        void Enable() noexcept
        {
            if (_enabled == false)
            {
                assert(_addr != nullptr);
#ifdef _WIN32
                assert(_protect == PAGE_EXECUTE_READWRITE);

                if (VirtualProtect(_addr, Size, _protect, &_protect) != 0)
                {
                    _enabled = true;
                }
#else
                if (const long page_size = sysconf(_SC_PAGESIZE); page_size != -1)
                {
                    const size_t begin = (size_t)(_addr) & ~((size_t)(page_size) - 1);
                    const size_t end   = (size_t)(_addr) + Size;

                    if (mprotect((void*)(begin), end - begin, PROT_READ | PROT_WRITE | PROT_EXEC) == 0)
                    {
                        _enabled = true;
                    }
                }
#endif
            }
        }

        void Disable() noexcept
        {
            if (_enabled == true)
            {
                assert(_addr != nullptr);
#ifdef _WIN32
                if (VirtualProtect(_addr, Size, _protect, &_protect) != 0)
                {
                    _enabled = false;
                }
#endif
            }
        }

    public:

        void* GetAddr() const noexcept
        {
            return _addr;
        }

        constexpr size_t GetSize() const noexcept
        {
            return Size;
        }

    private:

        void*  _addr    = nullptr;

        bool   _enabled = false;
#ifdef _WIN32
        DWORD  _protect = PAGE_EXECUTE_READWRITE;
#endif

    };

    template <size_t Size>
    struct Patch {

        Patch() noexcept = default;
        ~Patch() noexcept
        {
            Disable();
        }

        Patch(const Patch&) = delete;
        Patch(Patch&& object) noexcept
            : _addr      { object._addr }
            , _enabled   { object._enabled }
            , _unprotect { std::move(object._unprotect) }
        {
            std::memcpy(_patch_bytes, object._patch_bytes, Size);
            std::memcpy(_orig_bytes, object._orig_bytes, Size);

            object._addr    = nullptr;
            object._enabled = false;
        }

        Patch& operator=(const Patch&) = delete;
        Patch& operator=(Patch&& object) noexcept
        {
            if (&object != this)
            {
                Disable();

                _addr      = object._addr;
                _enabled   = object._enabled;
                _unprotect = std::move(object._unprotect);

                std::memcpy(_patch_bytes, object._patch_bytes, Size);
                std::memcpy(_orig_bytes, object._orig_bytes, Size);

                object._addr    = nullptr;
                object._enabled = false;
            }

            return *this;
        }

    public:

        Patch(void* const addr, const void* const patch, const bool enabled = true) noexcept
            : _addr      { addr }
            , _unprotect { addr, false }
        {
            assert(addr  != nullptr);
            assert(patch != nullptr);

            std::memcpy(_patch_bytes, patch, Size);
            std::memcpy(_orig_bytes, addr, Size);

            if (enabled) Enable();
        }

    public:

        void Enable() noexcept
        {
            if (_enabled == false)
            {
                _unprotect.Enable();
                std::memcpy(_addr, _patch_bytes, Size);
                _unprotect.Disable();
            }

            _enabled = true;
        }

        void Disable() noexcept
        {
            if (_enabled == true)
            {
                _unprotect.Enable();
                std::memcpy(_addr, _orig_bytes, Size);
                _unprotect.Disable();
            }

            _enabled = false;
        }

    public:

        void* GetAddr() const noexcept
        {
            return _addr;
        }

        constexpr size_t GetSize() const noexcept
        {
            return Size;
        }

    private:

        void*   _addr    = nullptr;

        bool    _enabled = false;

        uint8_t _patch_bytes[Size];
        uint8_t _orig_bytes[Size];

        UnprotectScope<Size> _unprotect;

    };

    struct JumpHook {

        JumpHook() noexcept = default;
        ~JumpHook() noexcept = default;
        JumpHook(const JumpHook&) = delete;
        JumpHook(JumpHook&&) noexcept = default;
        JumpHook& operator=(const JumpHook&) = delete;
        JumpHook& operator=(JumpHook&&) noexcept = default;

    private:

#pragma pack(push, 1)

        struct JumpInstruction {

            JumpInstruction() = delete;
            ~JumpInstruction() noexcept = default;
            JumpInstruction(const JumpInstruction&) noexcept = default;
            JumpInstruction(JumpInstruction&&) noexcept = default;
            JumpInstruction& operator=(const JumpInstruction&) noexcept = default;
            JumpInstruction& operator=(JumpInstruction&&) noexcept = default;

        public:

            JumpInstruction(const int32_t offset) noexcept
                : _offset { offset }
            {}

        private:

            uint8_t _opcode = 0xE9;
            int32_t _offset;

        };

#pragma pack(pop)

    public:

        JumpHook(void* const inject, const void* const hook, const bool enabled = true) noexcept
            : _patch { inject, &JumpInstruction((size_t)(hook) - ((size_t)(inject)
                + sizeof(JumpInstruction))), enabled }
        {
            assert(hook != nullptr);
        }

    public:

        void Enable() noexcept
        {
            _patch.Enable();
        }

        void Disable() noexcept
        {
            _patch.Disable();
        }

    public:

        const Patch<sizeof(JumpInstruction)>& GetPatch() const noexcept
        {
            return _patch;
        }

    private:

        Patch<sizeof(JumpInstruction)> _patch;

    };

    struct Scanner {

        Scanner() noexcept = default;
        ~Scanner() noexcept = default;
        Scanner(const Scanner&) noexcept = default;
        Scanner(Scanner&&) noexcept = default;
        Scanner& operator=(const Scanner&) noexcept = default;
        Scanner& operator=(Scanner&&) noexcept = default;

    public:

        Scanner(const void* const addr, const size_t size) noexcept
            : _addr { addr }
            , _size { size }
        {
            assert(addr != nullptr);
            assert(size != 0);
        }

    public:

        void* Find(const char* const pattern, const char* const mask) const noexcept
        {
            assert(pattern != nullptr);
            assert(mask    != nullptr);

            assert(_addr != nullptr);
            assert(_size != 0);

            const char* current_byte = static_cast<const char*>(_addr);
            const char* const last_byte = reinterpret_cast<char*>((size_t)(_addr) + _size - std::strlen(mask));

            for (size_t i; current_byte < last_byte; ++current_byte)
            {
                for (i = 0; mask[i] != '\0'; ++i) if (mask[i] == 'x' && pattern[i] != current_byte[i]) break;
                if (mask[i] == '\0') break;
            }

            return current_byte != last_byte ? const_cast<char*>(current_byte) : nullptr;
        }

    private:

        const void* _addr = nullptr;
        size_t      _size = 0;

    };

    static bool GetModuleInfo(const void* const addr, void*& module_addr, size_t& module_size) noexcept
    {
#ifdef _WIN32
        MEMORY_BASIC_INFORMATION info;

        if (VirtualQuery(addr, &info, sizeof(info)) == 0)
            return false;

        module_addr = info.AllocationBase;
        if (module_addr == nullptr) return false;

        const auto dos = (IMAGE_DOS_HEADER*)(info.AllocationBase);
        const auto pe = (IMAGE_NT_HEADERS*)((size_t)(dos) + dos->e_lfanew);

        if (pe->Signature != IMAGE_NT_SIGNATURE)
            return false;

        module_size = pe->OptionalHeader.SizeOfImage;
        if (module_size == 0) return false;
#else
        Dl_info info;
        struct stat buf;

        if (dladdr(addr, &info) == 0)
            return false;
        if (stat(info.dli_fname, &buf) == -1)
            return false;

        if ((module_addr = info.dli_fbase) == nullptr)
            return false;
        if ((module_size = buf.st_size) == 0)
            return false;
#endif
        return true;
    }
}
