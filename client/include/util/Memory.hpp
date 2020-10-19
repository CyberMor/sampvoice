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
#include <vector>

#include <Windows.h>

#define RequestArithmeticType(type) static_assert(std::is_arithmetic<type>::value, #type " is not arithmetic type")
#define RequestAddressType(type) static_assert(std::is_arithmetic<type>::value || std::is_pointer<type>::value, #type " is not address type")
#define SizeOfArray(arr) ((sizeof(arr) / sizeof(0[arr])) / ((size_t)(!(sizeof(arr) % sizeof(0[arr])))))

namespace Memory
{
    class ScopeExit {
    public:

        ScopeExit() noexcept = default;
        ScopeExit(const ScopeExit&) = delete;
        ScopeExit(ScopeExit&&) noexcept = default;
        ScopeExit& operator=(const ScopeExit&) = delete;
        ScopeExit& operator=(ScopeExit&&) noexcept = default;

    private:

        using CallbackType = std::function<void()>;

    public:

        explicit ScopeExit(CallbackType callback) noexcept
            : callback(std::move(callback)) {}

        ~ScopeExit() noexcept
        {
            if (this->callback) this->callback();
        }

    public:

        void Release() noexcept
        {
            this->callback = nullptr;
        }

    private:

        CallbackType callback { nullptr };

    };

    template<class ObjectType> class ObjectContainer {
    public:

        ObjectContainer() = default;
        ObjectContainer(const ObjectContainer&) = default;
        ObjectContainer(ObjectContainer&&) noexcept = default;
        ObjectContainer& operator=(const ObjectContainer&) = default;
        ObjectContainer& operator=(ObjectContainer&&) noexcept = default;

    public:

        explicit ObjectContainer(const DWORD addMemSize)
            : bytes(sizeof(ObjectType) + addMemSize) {}

        template<class MemAddrType = LPCVOID, class MemSizeType = DWORD>
        explicit ObjectContainer(const MemAddrType memAddr, const MemSizeType memSize)
            : bytes((DWORD)(memSize))
        {
            RequestAddressType(MemAddrType);
            RequestArithmeticType(MemSizeType);

            assert((LPCVOID)(memAddr));
            assert((DWORD)(memSize));

            assert((DWORD)(memSize) >= sizeof(ObjectType));

            std::memcpy(this->bytes.data(), (LPCVOID)(memAddr), this->bytes.size());
        }

        ~ObjectContainer() noexcept = default;

    public:

        const ObjectType* operator->() const noexcept
        {
            return reinterpret_cast<const ObjectType*>(this->bytes.data());
        }

        ObjectType* operator->() noexcept
        {
            return reinterpret_cast<ObjectType*>(this->bytes.data());
        }

        const ObjectType* operator&() const noexcept
        {
            return reinterpret_cast<const ObjectType*>(this->bytes.data());
        }

        ObjectType* operator&() noexcept
        {
            return reinterpret_cast<ObjectType*>(this->bytes.data());
        }

        LPCVOID GetData() const noexcept
        {
            return static_cast<LPCVOID>(this->bytes.data());
        }

        LPVOID GetData() noexcept
        {
            return static_cast<LPVOID>(this->bytes.data());
        }

        DWORD GetSize() const noexcept
        {
            return static_cast<DWORD>(this->bytes.size());
        }

    private:

        std::vector<BYTE> bytes { sizeof(ObjectType) };

    };

    template<class ObjectType> using ObjectContainerPtr = std::unique_ptr<ObjectContainer<ObjectType>>;

    class UnprotectScope {

        UnprotectScope() = delete;
        UnprotectScope(const UnprotectScope&) = delete;
        UnprotectScope(UnprotectScope&&) = delete;
        UnprotectScope& operator=(const UnprotectScope&) = delete;
        UnprotectScope& operator=(UnprotectScope&&) = delete;

    public:

        template<class MemAddrType = LPVOID, class MemSizeType = DWORD>
        explicit UnprotectScope(const MemAddrType memAddr, const MemSizeType memSize, const bool enabled = true) noexcept
            : memAddr((LPVOID)(memAddr)), memSize((DWORD)(memSize))
        {
            RequestAddressType(MemAddrType);
            RequestArithmeticType(MemSizeType);

            assert((LPVOID)(memAddr));
            assert((DWORD)(memSize));

            if (enabled) this->Enable();
        }

        ~UnprotectScope() noexcept
        {
            this->Disable();
        }

    public:

        void Enable() noexcept
        {
            if (this->enabled) return;

            this->enabled = VirtualProtect(
                this->memAddr, this->memSize,
                PAGE_EXECUTE_READWRITE,
                &this->origProtect
            );
        }

        void Disable() noexcept
        {
            if (!this->enabled) return;

            DWORD oldProtect;

            this->enabled = !VirtualProtect(
                this->memAddr, this->memSize,
                this->origProtect, &oldProtect
            );
        }

    public:

        const LPVOID memAddr;
        const DWORD memSize;

    private:

        BOOL enabled { FALSE };

        DWORD origProtect { NULL };

    };

    using UnprotectScopePtr = std::unique_ptr<UnprotectScope>;

    class Patch {

        Patch() = delete;
        Patch(const Patch&) = delete;
        Patch(Patch&&) = delete;
        Patch& operator=(const Patch&) = delete;
        Patch& operator=(Patch&&) = delete;

    public:

        template<class MemAddrType = LPVOID, class PatchAddrType = LPCVOID, class MemSizeType = DWORD>
        explicit Patch(const MemAddrType memAddr, const PatchAddrType patchAddr, const MemSizeType memSize, const bool enabled = true)
            : patchData((DWORD)(memSize)), origData((DWORD)(memSize)), memAddr((LPVOID)(memAddr)), memSize((DWORD)(memSize))
        {
            RequestAddressType(MemAddrType);
            RequestAddressType(PatchAddrType);
            RequestArithmeticType(MemSizeType);

            assert((LPVOID)(memAddr));
            assert((LPCVOID)(patchAddr));
            assert((DWORD)(memSize));

            std::memcpy(this->patchData.data(), (LPCVOID)(patchAddr), this->memSize);
            std::memcpy(this->origData.data(), this->memAddr, this->memSize);

            if (enabled) this->Enable();
        }

        ~Patch() noexcept
        {
            this->Disable();
        }

    public:

        void Enable() noexcept
        {
            if (this->enabled) return;

            {
                const UnprotectScope scope { this->memAddr, this->memSize };
                std::memcpy(this->memAddr, this->patchData.data(), this->memSize);
            }

            this->enabled = TRUE;
        }

        void Disable() noexcept
        {
            if (!this->enabled) return;

            {
                const UnprotectScope scope { this->memAddr, this->memSize };
                std::memcpy(this->memAddr, this->origData.data(), this->memSize);
            }

            this->enabled = FALSE;
        }

    public:

        const LPVOID memAddr;
        const DWORD memSize;

    private:

        BOOL enabled { FALSE };

        std::vector<BYTE> patchData;
        std::vector<BYTE> origData;

    };

    using PatchPtr = std::unique_ptr<Patch>;

    class JumpHook {

        JumpHook() = delete;
        JumpHook(const JumpHook&) = delete;
        JumpHook(JumpHook&&) = delete;
        JumpHook& operator=(const JumpHook&) = delete;
        JumpHook& operator=(JumpHook&&) = delete;

    private:

#pragma pack(push, 1)

        struct JumpInstruction {

            JumpInstruction() = delete;
            JumpInstruction(const JumpInstruction&) = delete;
            JumpInstruction(JumpInstruction&&) = delete;
            JumpInstruction& operator=(const JumpInstruction&) = delete;
            JumpInstruction& operator=(JumpInstruction&&) = delete;

        public:

            explicit JumpInstruction(const INT offset) noexcept
                : offset(offset) {}

            ~JumpInstruction() noexcept = default;

        public:

            const BYTE opcode { 0xE9 };
            const INT offset { 0 };

        };

#pragma pack(pop)

    public:

        template<class InjectAddrType = LPVOID, class HookAddrType = LPCVOID>
        explicit JumpHook(const InjectAddrType injectAddr, const HookAddrType hookAddr, const bool enabled = true)
            : patch(injectAddr, &JumpInstruction(((DWORD)(hookAddr)) - (((DWORD)(injectAddr)) +
                sizeof(JumpInstruction))), sizeof(JumpInstruction), enabled)
        {
            RequestAddressType(InjectAddrType);
            RequestAddressType(HookAddrType);

            assert((LPVOID)(injectAddr));
            assert((LPCVOID)(hookAddr));
        }

        ~JumpHook() noexcept = default;

    public:

        inline void Enable() noexcept
        {
            this->patch.Enable();
        }

        inline void Disable() noexcept
        {
            this->patch.Disable();
        }

        const Patch& GetPatch() const noexcept
        {
            return this->patch;
        }

    private:

        Patch patch;

    };

    using JumpHookPtr = std::unique_ptr<JumpHook>;

    class CallHook {

        CallHook() = delete;
        CallHook(const CallHook&) = delete;
        CallHook(CallHook&&) = delete;
        CallHook& operator=(const CallHook&) = delete;
        CallHook& operator=(CallHook&&) = delete;

    private:

#pragma pack(push, 1)

        struct CallInstruction {

            CallInstruction() = delete;
            CallInstruction(const CallInstruction&) = delete;
            CallInstruction(CallInstruction&&) = delete;
            CallInstruction& operator=(const CallInstruction&) = delete;
            CallInstruction& operator=(CallInstruction&&) = delete;

        public:

            explicit CallInstruction(const INT offset) noexcept
                : offset(offset) {}

            ~CallInstruction() noexcept = default;

        public:

            const BYTE opcode { 0xE8 };
            const INT offset { 0 };

        };

#pragma pack(pop)

    public:

        template<class InjectAddrType = LPVOID, class HookAddrType = LPCVOID>
        explicit CallHook(const InjectAddrType injectAddr, const HookAddrType hookAddr, const bool enabled = true)
            : callFuncAddr((LPVOID)((((DWORD)(injectAddr)) + sizeof(CallInstruction)) + ((CallInstruction*)(injectAddr))->offset))
            , patch(injectAddr, &CallInstruction(((DWORD)(hookAddr)) - (((DWORD)(injectAddr)) + sizeof(CallInstruction))),
                sizeof(CallInstruction), enabled)
        {
            RequestAddressType(InjectAddrType);
            RequestAddressType(HookAddrType);

            assert((LPVOID)(injectAddr));
            assert((LPCVOID)(hookAddr));
        }

        ~CallHook() noexcept = default;

    public:

        inline void Enable() noexcept
        {
            this->patch.Enable();
        }

        inline void Disable() noexcept
        {
            this->patch.Disable();
        }

        const Patch& GetPatch() const noexcept
        {
            return this->patch;
        }

    public:

        const LPVOID callFuncAddr;

    private:

        Patch patch;

    };

    using CallHookPtr = std::unique_ptr<CallHook>;

    class DllFuncHook {

        DllFuncHook() = delete;
        DllFuncHook(const DllFuncHook&) = delete;
        DllFuncHook(DllFuncHook&&) = delete;
        DllFuncHook& operator=(const DllFuncHook&) = delete;
        DllFuncHook& operator=(DllFuncHook&&) = delete;

    public:

        template<class HookAddrType = LPCVOID>
        explicit DllFuncHook(const char* moduleName, const char* functionName,
                             const HookAddrType hookAddr, const bool enabled = true)
            : hook(GetProcAddress(GetModuleHandle(moduleName), functionName), hookAddr, enabled)
        {
            RequestAddressType(HookAddrType);

            assert((LPCVOID)(hookAddr));
        }

        ~DllFuncHook() noexcept = default;

    public:

        inline void Enable() noexcept
        {
            this->hook.Enable();
        }

        inline void Disable() noexcept
        {
            this->hook.Disable();
        }

        const JumpHook& GetHook() const noexcept
        {
            return this->hook;
        }

    private:

        JumpHook hook;

    };

    using DllFuncHookPtr = std::unique_ptr<DllFuncHook>;

    class Scanner {
    public:

        Scanner() = delete;
        Scanner(const Scanner&) noexcept = default;
        Scanner(Scanner&&) noexcept = default;
        Scanner& operator=(const Scanner&) noexcept = default;
        Scanner& operator=(Scanner&&) noexcept = default;

    public:

        template<class MemAddrType = LPCVOID, class MemSizeType = DWORD>
        explicit Scanner(const MemAddrType memAddr, const MemSizeType memSize) noexcept
            : memAddr((LPCVOID)(memAddr)), memSize((DWORD)(memSize))
        {
            RequestAddressType(MemAddrType);
            RequestArithmeticType(MemSizeType);

            assert((LPCVOID)(memAddr));
            assert((DWORD)(memSize));
        }

        ~Scanner() noexcept = default;

    public:

        LPVOID Find(PCCH pattern, PCCH mask) const noexcept
        {
            PCCH currentByte = (PCCH)(this->memAddr);
            PCCH lastByte = (PCCH)((DWORD)(this->memAddr) +
                this->memSize - std::strlen(mask));

            for (DWORD i; currentByte < lastByte; ++currentByte)
            {
                for (i = 0; mask[i]; ++i) if (mask[i] == 'x' &&
                    pattern[i] != currentByte[i]) break;

                if (!mask[i]) break;
            }

            return currentByte != lastByte ? (LPVOID)(currentByte) : nullptr;
        }

    private:

        LPCVOID memAddr { nullptr };
        DWORD memSize { NULL };

    };

    using ScannerPtr = std::unique_ptr<Scanner>;

    template<class MemAddrType = LPCVOID, class ModuleAddrType = LPCVOID, class ModuleSizeType = DWORD>
    static bool GetModuleInfo(const MemAddrType memAddr, ModuleAddrType& moduleAddr, ModuleSizeType& moduleSize) noexcept
    {
        RequestAddressType(MemAddrType);
        RequestAddressType(ModuleAddrType);
        RequestArithmeticType(ModuleSizeType);

        MEMORY_BASIC_INFORMATION info {};

        if (VirtualQuery((LPCVOID)(memAddr), &info, sizeof(info)) == 0)
            return false;

        if ((moduleAddr = (ModuleAddrType)(info.AllocationBase)) == nullptr)
            return false;

        const auto dos = (IMAGE_DOS_HEADER*)(info.AllocationBase);
        const auto pe = (IMAGE_NT_HEADERS*)(((DWORD)(dos)) + dos->e_lfanew);

        if (pe->Signature != IMAGE_NT_SIGNATURE)
            return false;

        if ((moduleSize = (ModuleSizeType)(pe->OptionalHeader.SizeOfImage)) == 0)
            return false;

        return true;
    }

    template<class MemAddrType = LPVOID, class MemSizeType = DWORD>
    static inline void FillWithNops(const MemAddrType memAddr, const MemSizeType memSize) noexcept
    {
        RequestAddressType(MemAddrType);
        RequestArithmeticType(MemSizeType);

        assert((LPVOID)(memAddr));
        assert((DWORD)(memSize));

        {
            const UnprotectScope scope { memAddr, memSize };
            std::memset((LPVOID)(memAddr), 0x90, (DWORD)(memSize));
        }
    }
}

#define MakeObjectContainer(ObjectType) std::make_unique<Memory::ObjectContainer<ObjectType>>
#define MakeUnprotectScope              std::make_unique<Memory::UnprotectScope>
#define MakePatch                       std::make_unique<Memory::Patch>
#define MakeJumpHook                    std::make_unique<Memory::JumpHook>
#define MakeCallHook                    std::make_unique<Memory::CallHook>
#define MakeDllFuncHook                 std::make_unique<Memory::DllFuncHook>
#define MakeScanner                     std::make_unique<Memory::Scanner>
