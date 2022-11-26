/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <type_traits>
#include <functional>
#include <cassert>
#include <cstring>
#include <vector>

#include <Windows.h>

#define GetArraySize(array) (sizeof(array) / sizeof(*(array)))

namespace Memory
{
    struct ScopeExit {

        ScopeExit() noexcept = default;
        ~ScopeExit() noexcept
        {
            if (_callback != nullptr) _callback();
        }

        ScopeExit(const ScopeExit&) = delete;
        ScopeExit(ScopeExit&&) noexcept = default;
        ScopeExit& operator=(const ScopeExit&) = delete;
        ScopeExit& operator=(ScopeExit&&) noexcept = default;

    private:

        using Callback = std::function<void()>;

    public:

        ScopeExit(Callback&& callback) noexcept
            : _callback { std::move(callback) }
        {}

    public:

        void Release() noexcept
        {
            _callback = nullptr;
        }

    private:

        Callback _callback = nullptr;

    };

    template <class Type>
    struct ObjectContainer {

        ObjectContainer() noexcept = default;
        ~ObjectContainer() noexcept = default;
        ObjectContainer(const ObjectContainer&) = default;
        ObjectContainer(ObjectContainer&&) noexcept = default;
        ObjectContainer& operator=(const ObjectContainer&) = default;
        ObjectContainer& operator=(ObjectContainer&&) noexcept = default;

    public:

        ObjectContainer(const DWORD extra_size)
            : _bytes ( sizeof(Type) + extra_size )
        {}

        ObjectContainer(const LPCVOID addr, const DWORD size)
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

        const Type* operator&() const noexcept
        {
            return reinterpret_cast<const Type*>(_bytes.data());
        }

        Type* operator&() noexcept
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

        LPCVOID GetData() const noexcept
        {
            return static_cast<LPCVOID>(_bytes.data());
        }

        LPVOID GetData() noexcept
        {
            return static_cast<LPVOID>(_bytes.data());
        }

    public:

        DWORD GetSize() const noexcept
        {
            return static_cast<DWORD>(_bytes.size());
        }

    private:

        std::vector<BYTE> _bytes;

    };

    struct UnprotectScope {

        UnprotectScope() noexcept = default;
        ~UnprotectScope() noexcept
        {
            Disable();
        }

        UnprotectScope(const UnprotectScope&) = delete;
        UnprotectScope(UnprotectScope&& object) noexcept
            : _addr    { object._addr }
            , _size    { object._size }
            , _enabled { object._enabled }
            , _protect { object._protect }
        {
            object._addr    = nullptr;
            object._size    = 0;
            object._enabled = FALSE;
            object._protect = 0;
        }

        UnprotectScope& operator=(const UnprotectScope&) = delete;
        UnprotectScope& operator=(UnprotectScope&& object) noexcept
        {
            if (&object != this)
            {
                Disable();

                _addr    = object._addr;
                _size    = object._size;
                _enabled = object._enabled;
                _protect = object._protect;

                object._addr    = nullptr;
                object._size    = 0;
                object._enabled = FALSE;
                object._protect = 0;
            }

            return *this;
        }

    public:

        UnprotectScope(const LPVOID addr, const DWORD size, const bool enabled = true) noexcept
            : _addr { addr }
            , _size { size }
        {
            if (enabled) Enable();
        }

    public:

        void Enable() noexcept
        {
            if (_enabled == FALSE)
            {
                assert(_addr != nullptr);
                assert(_size != 0);

                assert(_protect == PAGE_EXECUTE_READWRITE);

                if (VirtualProtect(_addr, _size, _protect, &_protect) != FALSE)
                {
                    _enabled = TRUE;
                }
            }
        }

        void Disable() noexcept
        {
            if (_enabled == TRUE)
            {
                assert(_addr != nullptr);
                assert(_size != 0);

                if (VirtualProtect(_addr, _size, _protect, &_protect) != FALSE)
                {
                    _enabled = FALSE;
                }
            }
        }

    public:

        LPVOID GetAddr() const noexcept
        {
            assert(_addr != nullptr);
            return _addr;
        }

        DWORD GetSize() const noexcept
        {
            assert(_size != 0);
            return _size;
        }

    private:

        LPVOID _addr = nullptr;
        DWORD  _size = 0;

    private:

        BOOL   _enabled = FALSE;
        DWORD  _protect = PAGE_EXECUTE_READWRITE;

    };

    template <DWORD Size>
    struct Patch {

        Patch() noexcept = default;
        ~Patch() noexcept
        {
            Disable();
        }

        Patch(const Patch&) = delete;
        Patch(Patch&& object) noexcept
            : _addr    { object._addr }
            , _enabled { object._enabled }
        {
            std::memcpy(_patch_data, object._patch_data, Size);
            std::memcpy(_orig_data, object._orig_data, Size);

            object._addr    = nullptr;
            object._enabled = FALSE;
        }

        Patch& operator=(const Patch&) = delete;
        Patch& operator=(Patch&& object) noexcept
        {
            if (&object != this)
            {
                Disable();

                _addr    = object._addr;
                _enabled = object._enabled;

                std::memcpy(_patch_data, object._patch_data, Size);
                std::memcpy(_orig_data, object._orig_data, Size);

                object._addr    = nullptr;
                object._enabled = FALSE;
            }

            return *this;
        }

    public:

        Patch(const LPVOID addr, const LPCVOID patch, const bool enabled = true) noexcept
            : _addr { addr }
        {
            assert(addr  != nullptr);
            assert(patch != nullptr);

            std::memcpy(_patch_data, patch, Size);
            std::memcpy(_orig_data, addr, Size);

            if (enabled) Enable();
        }

    public:

        void Enable() noexcept
        {
            if (_enabled == FALSE)
            {
                assert(_addr != nullptr);

                const UnprotectScope scope { _addr, Size };
                std::memcpy(_addr, _patch_data, Size);
            }

            _enabled = TRUE;
        }

        void Disable() noexcept
        {
            if (_enabled == TRUE)
            {
                assert(_addr != nullptr);

                const UnprotectScope scope { _addr, Size };
                std::memcpy(_addr, _orig_data, Size);
            }

            _enabled = FALSE;
        }

    public:

        LPVOID GetAddr() const noexcept
        {
            assert(_addr != nullptr);
            return _addr;
        }

        constexpr DWORD GetSize() const noexcept
        {
            return Size;
        }

    private:

        LPVOID _addr = nullptr;

        BOOL   _enabled = FALSE;

        BYTE   _patch_data[Size];
        BYTE   _orig_data[Size];

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

            JumpInstruction(const INT32 offset) noexcept
                : _offset { offset }
            {}

        private:

            BYTE  _opcode = 0xE9;
            INT32 _offset;

        };

#pragma pack(pop)

    public:

        JumpHook(const LPVOID inject_addr, const LPCVOID hook_addr, const bool enabled = true) noexcept
            : _patch { inject_addr, &JumpInstruction((DWORD)(hook_addr) -
                ((DWORD)(inject_addr) + sizeof(JumpInstruction))), enabled }
        {
            assert(hook_addr != nullptr);
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

    struct CallHook {

        CallHook() noexcept = default;
        ~CallHook() noexcept = default;
        CallHook(const CallHook&) = delete;
        CallHook(CallHook&&) noexcept = default;
        CallHook& operator=(const CallHook&) = delete;
        CallHook& operator=(CallHook&&) noexcept = default;

    private:

#pragma pack(push, 1)

        struct CallInstruction {

            CallInstruction() = delete;
            ~CallInstruction() noexcept = default;
            CallInstruction(const CallInstruction&) noexcept = default;
            CallInstruction(CallInstruction&&) noexcept = default;
            CallInstruction& operator=(const CallInstruction&) noexcept = default;
            CallInstruction& operator=(CallInstruction&&) noexcept = default;

        public:

            CallInstruction(const INT32 offset) noexcept
                : _offset { offset }
            {}

        private:

            BYTE  _opcode = 0xE8;
            INT32 _offset;

        };

#pragma pack(pop)

    public:

        CallHook(const LPVOID inject_addr, const LPCVOID hook_addr, const bool enabled = true) noexcept
            : _call_func_addr { (LPVOID)(((DWORD)(inject_addr) + sizeof(CallInstruction)) +
                ((CallInstruction*)(inject_addr))->offset) }
            , _patch { inject_addr, &CallInstruction((DWORD)(hook_addr) -
                ((DWORD)(inject_addr) + sizeof(CallInstruction))), enabled }
        {
            assert(hook_addr != nullptr);
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

        LPVOID GetCallFuncAddr() const noexcept
        {
            assert(_call_func_addr != nullptr);
            return _call_func_addr;
        }

        const Patch<sizeof(CallInstruction)>& GetPatch() const noexcept
        {
            return _patch;
        }

    private:

        LPVOID _call_func_addr = nullptr;
        Patch<sizeof(CallInstruction)> _patch;

    };

    struct DllFuncHook {

        DllFuncHook() noexcept = default;
        ~DllFuncHook() noexcept = default;
        DllFuncHook(const DllFuncHook&) = delete;
        DllFuncHook(DllFuncHook&&) noexcept = default;
        DllFuncHook& operator=(const DllFuncHook&) = delete;
        DllFuncHook& operator=(DllFuncHook&&) noexcept = default;

    public:

        DllFuncHook(const char* const module, const char* const function, const LPCVOID hook_addr, const bool enabled = true) noexcept
            : _hook { GetProcAddress(GetModuleHandle(module), function), hook_addr, enabled }
        {}

    public:

        void Enable() noexcept
        {
            _hook.Enable();
        }

        void Disable() noexcept
        {
            _hook.Disable();
        }

    public:

        const JumpHook& GetHook() const noexcept
        {
            return _hook;
        }

    private:

        JumpHook _hook;

    };

    struct Scanner {

        Scanner() noexcept = default;
        ~Scanner() noexcept = default;
        Scanner(const Scanner&) noexcept = default;
        Scanner(Scanner&&) noexcept = default;
        Scanner& operator=(const Scanner&) noexcept = default;
        Scanner& operator=(Scanner&&) noexcept = default;

    public:

        Scanner(const LPCVOID addr, const DWORD size) noexcept
            : _addr { addr }
            , _size { size }
        {
            assert(addr != nullptr);
            assert(size != 0);
        }

    public:

        LPVOID Find(PCCH pattern, PCCH mask) const noexcept
        {
            assert(pattern != nullptr);
            assert(mask    != nullptr);

            PCCH current_byte = (PCCH)(_addr);
            PCCH last_byte = (PCCH)((DWORD)(_addr) + _size - std::strlen(mask));

            for (DWORD i; current_byte < last_byte; ++current_byte)
            {
                for (i = 0; mask[i]; ++i) if (mask[i] == 'x' && pattern[i] != current_byte[i]) break;
                if (mask[i] == '\0') break;
            }

            return current_byte != last_byte ? (LPVOID)(current_byte) : nullptr;
        }

    private:

        LPCVOID _addr = nullptr;
        DWORD   _size = 0;

    };

    static bool GetModuleInfo(const LPCVOID addr, LPCVOID& module_addr, DWORD& module_size) noexcept
    {
        MEMORY_BASIC_INFORMATION info;

        if (VirtualQuery(addr, &info, sizeof(info)) == 0) return false;

        module_addr = info.AllocationBase;
        if (module_addr == nullptr) return false;

        const auto dos = (IMAGE_DOS_HEADER*)(info.AllocationBase);
        const auto pe = (IMAGE_NT_HEADERS*)((DWORD)(dos) + dos->e_lfanew);

        if (pe->Signature != IMAGE_NT_SIGNATURE) return false;

        module_size = pe->OptionalHeader.SizeOfImage;
        if (module_size == 0) return false;

        return true;
    }

    static void FillWithNops(const LPVOID addr, const DWORD size) noexcept
    {
        const UnprotectScope scope { addr, size };
        std::memset(addr, 0x90, size);
    }
}
