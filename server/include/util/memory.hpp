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
#define STDCALL __stdcall
#define THISCALL __thiscall
#else
#define STDCALL
#define THISCALL
#endif

#define RequireArithmeticType(type) static_assert(std::is_arithmetic<type>::value, #type " is not arithmetic type")
#define RequireAddressType(type) static_assert(std::is_arithmetic<type>::value || std::is_pointer<type>::value, #type " is not address type")
#define RequireVarHasType(var, type) static_assert(std::is_same<decltype(var), type>::value, #var " type does not correspond to " #type)
#define SizeOfArray(arr) ((sizeof(arr) / sizeof(0[arr])) / ((size_t)(!(sizeof(arr) % sizeof(0[arr])))))

namespace Memory
{
    using addr_t = void*;
    using size_t = std::size_t;
    using byte_t = unsigned char;

    template<class ObjectType> class ObjectContainer {
    public:

        ObjectContainer() = default;
        ObjectContainer(const ObjectContainer&) = default;
        ObjectContainer(ObjectContainer&&) noexcept = default;
        ObjectContainer& operator=(const ObjectContainer&) = default;
        ObjectContainer& operator=(ObjectContainer&&) noexcept = default;

    public:

        explicit ObjectContainer(const size_t addMemSize)
            : bytes(sizeof(ObjectType) + addMemSize) {}

        template<class MemAddrType = addr_t, class MemSizeType = size_t>
        explicit ObjectContainer(const MemAddrType memAddr, const MemSizeType memSize)
            : bytes((size_t)(memSize))
        {
            RequireAddressType(MemAddrType);
            RequireArithmeticType(MemSizeType);

            assert((addr_t)(memAddr));
            assert((size_t)(memSize));

            assert((size_t)(memSize) >= sizeof(ObjectType));

            std::memcpy(this->bytes.data(), (addr_t)(memAddr), this->bytes.size());
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

        const addr_t GetData() const noexcept
        {
            return reinterpret_cast<const addr_t>(this->bytes.data());
        }

        addr_t GetData() noexcept
        {
            return reinterpret_cast<addr_t>(this->bytes.data());
        }

        size_t GetSize() const noexcept
        {
            return static_cast<size_t>(this->bytes.size());
        }

    private:

        std::vector<byte_t> bytes { sizeof(ObjectType) };

    };

    template<class ObjectType> using ObjectContainerPtr = std::unique_ptr<ObjectContainer<ObjectType>>;

    class UnprotectScope {

        UnprotectScope() = delete;
        UnprotectScope(const UnprotectScope&) = delete;
        UnprotectScope(UnprotectScope&&) = delete;
        UnprotectScope& operator=(const UnprotectScope&) = delete;
        UnprotectScope& operator=(UnprotectScope&&) = delete;

    public:

        template<class MemAddrType = addr_t, class MemSizeType = size_t>
        explicit UnprotectScope(const MemAddrType memAddr, const MemSizeType memSize, const bool enabled = true) noexcept
            : memAddr((addr_t)(memAddr)), memSize((size_t)(memSize))
        {
            RequireAddressType(MemAddrType);
            RequireArithmeticType(MemSizeType);

            assert((addr_t)(memAddr));
            assert((size_t)(memSize));

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

#ifdef _WIN32
            this->enabled = VirtualProtect(this->memAddr, this->memSize, PAGE_EXECUTE_READWRITE, &this->origProtect);
#else
            const long pageSize = sysconf(_SC_PAGESIZE);
            if (pageSize == -1) return;

            const long alignMemAddr = (long)(this->memAddr) & ~(pageSize - 1);
            const long pageCount = ((long)(this->memAddr) + (long)(this->memSize) - alignMemAddr) / pageSize + 1;

            this->enabled = !mprotect((void*)(alignMemAddr), pageCount * pageSize, PROT_READ | PROT_EXEC | PROT_WRITE);
#endif
        }

        void Disable() noexcept
        {
            if (!this->enabled) return;

#ifdef _WIN32
            DWORD oldProtect;

            this->enabled = !VirtualProtect(this->memAddr, this->memSize, this->origProtect, &oldProtect);
#else
            const long pageSize = sysconf(_SC_PAGESIZE);
            if (pageSize == -1) return;

            const long alignMemAddr = (long)(this->memAddr) & ~(pageSize - 1);
            const long pageCount = ((long)(this->memAddr) + (long)(this->memSize) - alignMemAddr) / pageSize + 1;

            this->enabled = mprotect((void*)(alignMemAddr), pageCount * pageSize, PROT_READ | PROT_EXEC);
#endif
        }

    public:

        const addr_t memAddr;
        const size_t memSize;

    private:

        bool enabled { false };

#ifdef _WIN32
        DWORD origProtect { NULL };
#endif

    };

    using UnprotectScopePtr = std::unique_ptr<UnprotectScope>;

    class Patch {

        Patch() = delete;
        Patch(const Patch&) = delete;
        Patch(Patch&&) = delete;
        Patch& operator=(const Patch&) = delete;
        Patch& operator=(Patch&&) = delete;

    public:

        template<class MemAddrType = addr_t, class PatchAddrType = addr_t, class MemSizeType = size_t>
        explicit Patch(const MemAddrType memAddr, const PatchAddrType patchAddr, const MemSizeType memSize, const bool enabled = true)
            : memAddr((addr_t)(memAddr)), memSize((size_t)(memSize)), patchData((size_t)(memSize)), origData((size_t)(memSize))
        {
            RequireAddressType(MemAddrType);
            RequireAddressType(PatchAddrType);
            RequireArithmeticType(MemSizeType);

            assert((addr_t)(memAddr));
            assert((addr_t)(patchAddr));
            assert((size_t)(memSize));

            std::memcpy(this->patchData.data(), (addr_t)(patchAddr), this->memSize);
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

            this->enabled = true;
        }

        void Disable() noexcept
        {
            if (!this->enabled) return;

            {
                const UnprotectScope scope { this->memAddr, this->memSize };
                std::memcpy(this->memAddr, this->origData.data(), this->memSize);
            }

            this->enabled = false;
        }

    public:

        const addr_t memAddr;
        const size_t memSize;

    private:

        bool enabled { false };

        std::vector<byte_t> patchData;
        std::vector<byte_t> origData;

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

            explicit JumpInstruction(int offset) noexcept
                : offset(offset) {}

            ~JumpInstruction() noexcept = default;

        public:

            const byte_t opcode { 0xE9 };
            const int offset { 0 };

        };

#pragma pack(pop)

    public:

        template<class InjectAddrType = addr_t, class HookAddrType = addr_t>
        explicit JumpHook(const InjectAddrType injectAddr, const HookAddrType hookAddr, const bool enabled = true)
            : patch(injectAddr, &JumpInstruction(((size_t)(hookAddr)) - (((size_t)(injectAddr)) +
                sizeof(JumpInstruction))), sizeof(JumpInstruction), enabled)
        {
            RequireAddressType(InjectAddrType);
            RequireAddressType(HookAddrType);

            assert((addr_t)(injectAddr));
            assert((addr_t)(hookAddr));
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

    class Scanner {
    public:

        Scanner() noexcept = default;
        Scanner(const Scanner&) noexcept = default;
        Scanner(Scanner&&) noexcept = default;
        Scanner& operator=(const Scanner&) noexcept = default;
        Scanner& operator=(Scanner&&) noexcept = default;

    public:

        template<class MemAddrType = addr_t, class MemSizeType = size_t>
        explicit Scanner(const MemAddrType memAddr, const MemSizeType memSize) noexcept
            : memAddr((addr_t)(memAddr)), memSize((size_t)(memSize))
        {
            RequireAddressType(MemAddrType);
            RequireArithmeticType(MemSizeType);

            assert((addr_t)(memAddr));
            assert((size_t)(memSize));
        }

        ~Scanner() noexcept = default;

    public:

        addr_t Find(const char* const pattern, const char* const mask) const noexcept
        {
            if (!this->memAddr || !this->memSize) return nullptr;

            const char* currentByte = static_cast<char*>(this->memAddr);
            const char* const lastByte = reinterpret_cast<char*>((size_t)(this->memAddr)
                + this->memSize - std::strlen(mask));

            for (size_t i; currentByte < lastByte; ++currentByte)
            {
                for (i = 0; mask[i]; ++i) if (mask[i] == 'x' &&
                    pattern[i] != currentByte[i]) break;

                if (!mask[i]) break;
            }

            return currentByte != lastByte ? (addr_t)(currentByte) : nullptr;
        }

    private:

        addr_t memAddr { nullptr };
        size_t memSize { NULL };

    };

    using ScannerPtr = std::unique_ptr<Scanner>;

    template<class MemAddrType = addr_t, class ModuleAddrType = addr_t, class ModuleSizeType = size_t>
    static bool GetModuleInfo(const MemAddrType memAddr, ModuleAddrType& moduleAddr, ModuleSizeType& moduleSize) noexcept
    {
        RequireAddressType(MemAddrType);
        RequireAddressType(ModuleAddrType);
        RequireArithmeticType(ModuleSizeType);

#ifdef _WIN32
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
#else
        Dl_info info {};
        struct stat buf {};

        if (dladdr((addr_t)(memAddr), &info) == 0)
            return false;

        if (stat(info.dli_fname, &buf) == -1)
            return false;

        if ((moduleAddr = (ModuleAddrType)(info.dli_fbase)) == nullptr)
            return false;

        if ((moduleSize = (ModuleSizeType)(buf.st_size)) == 0)
            return false;
#endif

        return true;
    }

    static inline float qsqrt(const float number) noexcept
    {
        float result;

#ifdef _WIN32
        __asm {
            mov eax, number
            sub eax, 0x3f800000
            sar eax, 1
            add eax, 0x3f800000
            mov result, eax
        }
#else
        __asm__ __volatile__ (
            "subl $0x3f800000, %1\n\t"
            "sarl $1, %1\n\t"
            "addl $0x3f800000, %1"
            : "=a"(result)
            : "a"(number)
        );
#endif

        return result;
    }
}

#define MakeObjectContainer(ObjectType) std::make_unique<Memory::ObjectContainer<ObjectType>>
#define MakeUnprotectScope              std::make_unique<Memory::UnprotectScope>
#define MakePatch                       std::make_unique<Memory::Patch>
#define MakeJumpHook                    std::make_unique<Memory::JumpHook>
#define MakeScanner                     std::make_unique<Memory::Scanner>
