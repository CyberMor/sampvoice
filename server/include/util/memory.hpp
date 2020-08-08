/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <vector>
#include <fstream>
#include <type_traits>
#include <iostream>

#include <assert.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#define STDCALL __stdcall
#define THISCALL __thiscall

#else

#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#define STDCALL
#define THISCALL

#endif

#define RequestArithmeticType(type) static_assert(std::is_arithmetic<type>::value, #type " is not arithmetic type")
#define RequestAddressType(type) static_assert(std::is_arithmetic<type>::value || std::is_pointer<type>::value, #type " is not address type")

namespace Memory {

	using addr_t = void*;
	using byte_t = unsigned char;

	template<class ObjectType> class ObjectContainer {
	private:

		std::vector<byte_t> bytes;

	public:

		ObjectContainer() : bytes(sizeof(ObjectType)) {}
		ObjectContainer(const size_t addMemSize) : bytes(sizeof(ObjectType) + addMemSize) {}

		template<class MemAddrType = addr_t, class MemSizeType = size_t>
		ObjectContainer(const MemAddrType memAddr, const MemSizeType memSize) : bytes((size_t)(memSize)) {

			RequestAddressType(MemAddrType);
			RequestArithmeticType(MemSizeType);

			assert((addr_t)(memAddr));
			assert((size_t)(memSize));

			assert((size_t)(memSize) >= sizeof(ObjectType));

			memcpy(this->bytes.data(), (addr_t)(memAddr), this->bytes.size());

		}

		ObjectType* operator->() const {

			return (ObjectType*)(this->bytes.data());

		}

		ObjectType* operator&() const {

			return (ObjectType*)(this->bytes.data());

		}

		addr_t GetData() const {

			return (addr_t)(this->bytes.data());

		}

		size_t GetSize() const {

			return (size_t)(this->bytes.size());

		}

	};

	template<class ObjectType> using ObjectContainerPtr = std::shared_ptr<ObjectContainer<ObjectType>>;

	class UnprotectScope {
	private:

		bool enabled = false;

#ifdef _WIN32
		DWORD origProtect = NULL;
#endif

	public:

		const addr_t memAddr;
		const size_t memSize;

	public:

		UnprotectScope() = delete;
		UnprotectScope(const UnprotectScope& object) = delete;
		UnprotectScope(UnprotectScope&& object) = delete;

		UnprotectScope& operator=(const UnprotectScope& object) = delete;
		UnprotectScope& operator=(UnprotectScope&& object) = delete;

		template<class MemAddrType = addr_t, class MemSizeType = size_t>
		UnprotectScope(const MemAddrType memAddr, const MemSizeType memSize, const bool enabled = true) :
			memAddr((addr_t)(memAddr)), memSize((size_t)(memSize))
		{

			RequestAddressType(MemAddrType);
			RequestArithmeticType(MemSizeType);

			assert((addr_t)(memAddr));
			assert((size_t)(memSize));

			if (enabled) this->Enable();

		}

		void Enable() {

			if (this->enabled) return;

#ifdef _WIN32

			this->enabled = VirtualProtect(
				this->memAddr, this->memSize,
				PAGE_EXECUTE_READWRITE,
				&this->origProtect
			);

#else

			const long pageSize = sysconf(_SC_PAGESIZE);

			if (pageSize == -1) return;

			const long alignMemAddr = (long)(this->memAddr) & ~(pageSize - 1);
			const long pageCount = ((long)(this->memAddr) + (long)(this->memSize) - alignMemAddr) / pageSize + 1;

			this->enabled = !mprotect((void*)(alignMemAddr), pageCount * pageSize, PROT_READ | PROT_EXEC | PROT_WRITE);

#endif

		}

		void Disable() {

			if (!this->enabled) return;

#ifdef _WIN32

			DWORD oldProtect = NULL;

			this->enabled = !VirtualProtect(
				this->memAddr, this->memSize,
				this->origProtect, &oldProtect
			);

#else

			const long pageSize = sysconf(_SC_PAGESIZE);

			if (pageSize == -1) return;

			const long alignMemAddr = (long)(this->memAddr) & ~(pageSize - 1);
			const long pageCount = ((long)(this->memAddr) + (long)(this->memSize) - alignMemAddr) / pageSize + 1;

			this->enabled = mprotect((void*)(alignMemAddr), pageCount * pageSize, PROT_READ | PROT_EXEC);

#endif

		}

		~UnprotectScope() {

			this->Disable();

		}

	};

	using UnprotectScopePtr = std::shared_ptr<UnprotectScope>;

	class Patch {
	private:

		bool enabled = false;

		std::vector<byte_t> patchData;
		std::vector<byte_t> origData;

	public:

		const addr_t memAddr;
		const size_t memSize;

	public:

		Patch() = delete;
		Patch(const Patch& object) = delete;
		Patch(Patch&& object) = delete;

		Patch& operator=(const Patch& object) = delete;
		Patch& operator=(Patch&& object) = delete;

		template<class MemAddrType = addr_t, class PatchAddrType = addr_t, class MemSizeType = size_t>
		Patch(const MemAddrType memAddr, const PatchAddrType patchAddr, const MemSizeType memSize, const bool enabled = true) :
			patchData((size_t)(memSize)), origData((size_t)(memSize)), memAddr((addr_t)(memAddr)), memSize((size_t)(memSize))
		{

			RequestAddressType(MemAddrType);
			RequestAddressType(PatchAddrType);
			RequestArithmeticType(MemSizeType);

			assert((addr_t)(memAddr));
			assert((addr_t)(patchAddr));
			assert((size_t)(memSize));

			memcpy(this->patchData.data(), (addr_t)(patchAddr), this->memSize);
			memcpy(this->origData.data(), this->memAddr, this->memSize);

			if (enabled) this->Enable();

		}

		void Enable() {

			if (this->enabled) return;

			{
				const UnprotectScope scope(this->memAddr, this->memSize);
				memcpy(this->memAddr, this->patchData.data(), this->memSize);
			}

			this->enabled = true;

		}

		void Disable() {

			if (!this->enabled) return;

			{
				const UnprotectScope scope(this->memAddr, this->memSize);
				memcpy(this->memAddr, this->origData.data(), this->memSize);
			}

			this->enabled = false;

		}

		~Patch() {

			this->Disable();

		}

	};

	using PatchPtr = std::shared_ptr<Patch>;

	class JumpHook {
	private:

#pragma pack(push, 1)

		struct JumpInstruction {

			const byte_t opcode = 0xE9;
			const int offset;

			JumpInstruction(const int offset) :
				offset(offset) {}

		};

#pragma pack(pop)

		Patch patch;

	public:

		JumpHook() = delete;
		JumpHook(const JumpHook& object) = delete;
		JumpHook(JumpHook&& object) = delete;

		JumpHook& operator=(const JumpHook& object) = delete;
		JumpHook& operator=(JumpHook&& object) = delete;

		template<class InjectAddrType = addr_t, class HookAddrType = addr_t>
		JumpHook(const InjectAddrType injectAddr, const HookAddrType hookAddr, const bool enabled = true) :
			patch(injectAddr, &JumpInstruction(((size_t)(hookAddr)) - (((size_t)(injectAddr)) +
				sizeof(JumpInstruction))), sizeof(JumpInstruction), enabled)
		{

			RequestAddressType(InjectAddrType);
			RequestAddressType(HookAddrType);

			assert((addr_t)(injectAddr));
			assert((addr_t)(hookAddr));

		}

		const Patch& GetPatch() const {

			return this->patch;

		}

		void Enable() {

			this->patch.Enable();

		}

		void Disable() {

			this->patch.Disable();

		}

	};

	using JumpHookPtr = std::shared_ptr<JumpHook>;

	class Scanner {
	public:

		addr_t memAddr;
		size_t memSize;

	public:

		Scanner() = delete;

		template<class MemAddrType = addr_t, class MemSizeType = size_t>
		Scanner(const MemAddrType memAddr, const MemSizeType memSize) :
			memAddr((addr_t)(memAddr)), memSize((size_t)(memSize))
		{

			RequestAddressType(MemAddrType);
			RequestArithmeticType(MemSizeType);

			assert((addr_t)(memAddr));
			assert((size_t)(memSize));

		}

		addr_t Find(const char* pattern, const char* mask) const {

			const char* currentByte = (char*)(this->memAddr);
			const char* lastByte = (char*)((size_t)(this->memAddr) +
				this->memSize - strlen(mask));

			for (size_t i; currentByte < lastByte; ++currentByte) {

				for (i = 0; mask[i]; ++i) if (mask[i] == 'x' &&
					pattern[i] != currentByte[i]) break;

				if (!mask[i]) break;

			}

			return currentByte != lastByte ? (addr_t)(currentByte) : nullptr;

		}

	};

	using ScannerPtr = std::shared_ptr<Scanner>;

	static size_t GetFileSize(const std::string& fileName) {

		std::fstream file(fileName, std::ios::binary);

		if (!file || !file.is_open()) return -1;
		if (!file.seekg(0, std::ios::end)) return -1;

		return file.tellg();

	}

	template<class MemAddrType = addr_t, class ModuleAddrType = addr_t, class ModuleSizeType = size_t>
	static bool GetModuleInfo(const MemAddrType memAddr, ModuleAddrType& moduleAddr, ModuleSizeType& moduleSize) {

		RequestAddressType(MemAddrType);
		RequestAddressType(ModuleAddrType);
		RequestArithmeticType(ModuleSizeType);

#ifdef _WIN32

		MEMORY_BASIC_INFORMATION info = {};

		if (!VirtualQuery((LPCVOID)(memAddr), &info, sizeof(info))) return false;

		if (!(moduleAddr = (ModuleAddrType)(info.AllocationBase))) return false;

		const auto dos = (IMAGE_DOS_HEADER*)(info.AllocationBase);
		const auto pe = (IMAGE_NT_HEADERS*)((DWORD)(dos) + dos->e_lfanew);

		if (pe->Signature != IMAGE_NT_SIGNATURE) return false;

		if (!(moduleSize = (ModuleSizeType)(pe->OptionalHeader.SizeOfImage))) return false;

#else

		Dl_info info = {};
		struct stat buf = {};

		if (!dladdr((addr_t)(memAddr), &info)) return false;
		if (stat(info.dli_fname, &buf) != 0) return false;

		if (!(moduleAddr = (ModuleAddrType)(info.dli_fbase))) return false;
		if (!(moduleSize = (ModuleSizeType)(buf.st_size))) return false;

#endif

		return true;

	}

	static inline float qsqrt(const float number) {

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

		__asm__ __volatile__(
			".intel_syntax noprefix;"
			"mov eax, %1;"
			"sub eax, 0x3f800000;"
			"sar eax, 1;"
			"add eax, 0x3f800000;"
			"mov %0, eax;"
			".att_syntax;"
			: "=r"(result)
			: "r"(number)
			: "eax"
		);

#endif

		return result;

	}

}

#define MakeObjectContainer(ObjectType)	std::make_shared<Memory::ObjectContainer<ObjectType>>
#define MakeUnprotectScope				std::make_shared<Memory::UnprotectScope>
#define MakePatch						std::make_shared<Memory::Patch>
#define MakeJumpHook					std::make_shared<Memory::JumpHook>
#define MakeScanner						std::make_shared<Memory::Scanner>
