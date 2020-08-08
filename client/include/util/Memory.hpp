/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <vector>
#include <type_traits>
#include <string>

#include <assert.h>
#include <Windows.h>

#define RequestArithmeticType(type) static_assert(std::is_arithmetic<type>::value, #type " is not arithmetic type")
#define RequestAddressType(type) static_assert(std::is_arithmetic<type>::value || std::is_pointer<type>::value, #type " is not address type")

namespace Memory {

	template<class ObjectType> class ObjectContainer {
	private:

		std::vector<BYTE> bytes;

	public:

		ObjectContainer() : bytes(sizeof(ObjectType)) {}
		ObjectContainer(const DWORD addMemSize) : bytes(sizeof(ObjectType) + addMemSize) {}

		template<class MemAddrType = LPVOID, class MemSizeType = DWORD>
		ObjectContainer(const MemAddrType memAddr, const MemSizeType memSize) : bytes((DWORD)(memSize)) {

			RequestAddressType(MemAddrType);
			RequestArithmeticType(MemSizeType);

			assert((LPVOID)(memAddr));
			assert((DWORD)(memSize));

			memcpy(this->bytes.data(), (LPCVOID)(memAddr), this->bytes.size());

		}

		ObjectType* operator->() const {

			return (ObjectType*)(this->bytes.data());

		}

		ObjectType* operator&() const {

			return (ObjectType*)(this->bytes.data());

		}

		LPVOID GetData() const {

			return (LPVOID)(this->bytes.data());

		}

		DWORD GetSize() const {

			return (DWORD)(this->bytes.size());

		}

	};

	template<class ObjectType> using ObjectContainerPtr = std::shared_ptr<ObjectContainer<ObjectType>>;

	class UnprotectScope {
	private:

		BOOL enabled = FALSE;

		DWORD origProtect = NULL;

	public:

		const LPVOID memAddr;
		const DWORD memSize;

	public:

		UnprotectScope() = delete;
		UnprotectScope(const UnprotectScope& object) = delete;
		UnprotectScope(UnprotectScope&& object) = delete;

		UnprotectScope& operator=(const UnprotectScope& object) = delete;
		UnprotectScope& operator=(UnprotectScope&& object) = delete;

		template<class MemAddrType = LPVOID, class MemSizeType = DWORD>
		UnprotectScope(const MemAddrType memAddr, const MemSizeType memSize, const bool enabled = true) :
			memAddr((LPVOID)(memAddr)), memSize((DWORD)(memSize))
		{

			RequestAddressType(MemAddrType);
			RequestArithmeticType(MemSizeType);

			assert((LPVOID)(memAddr));
			assert((DWORD)(memSize));

			if (enabled) this->Enable();

		}

		void Enable() {

			if (this->enabled) return;

			this->enabled = VirtualProtect(
				this->memAddr, this->memSize,
				PAGE_EXECUTE_READWRITE,
				&this->origProtect
			);

		}

		void Disable() {

			if (!this->enabled) return;

			DWORD oldProtect;

			this->enabled = !VirtualProtect(
				this->memAddr, this->memSize,
				this->origProtect, &oldProtect
			);

		}

		~UnprotectScope() {

			this->Disable();

		}

	};

	using UnprotectScopePtr = std::shared_ptr<UnprotectScope>;

	class Patch {
	private:

		BOOL enabled = FALSE;

		std::vector<BYTE> patchData;
		std::vector<BYTE> origData;

	public:

		const LPVOID memAddr;
		const DWORD memSize;

	public:

		Patch() = delete;
		Patch(const Patch& object) = delete;
		Patch(Patch&& object) = delete;

		Patch& operator=(const Patch& object) = delete;
		Patch& operator=(Patch&& object) = delete;

		template<class MemAddrType = LPVOID, class PatchAddrType = LPVOID, class MemSizeType = DWORD>
		Patch(const MemAddrType memAddr, const PatchAddrType patchAddr, const MemSizeType memSize, const bool enabled = true) :
			patchData((DWORD)(memSize)), origData((DWORD)(memSize)), memAddr((LPVOID)(memAddr)), memSize((DWORD)(memSize))
		{

			RequestAddressType(MemAddrType);
			RequestAddressType(PatchAddrType);
			RequestArithmeticType(MemSizeType);

			assert((LPVOID)(memAddr));
			assert((LPVOID)(patchAddr));
			assert((DWORD)(memSize));

			memcpy(this->patchData.data(), (LPCVOID)(patchAddr), this->memSize);
			memcpy(this->origData.data(), this->memAddr, this->memSize);

			if (enabled) this->Enable();

		}

		void Enable() {

			if (this->enabled) return;

			const UnprotectScope scope(this->memAddr, this->memSize);

			memcpy(this->memAddr, this->patchData.data(), this->memSize);

			this->enabled = TRUE;

		}

		void Disable() {

			if (!this->enabled) return;

			const UnprotectScope scope(this->memAddr, this->memSize);

			memcpy(this->memAddr, this->origData.data(), this->memSize);

			this->enabled = FALSE;

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

			const BYTE opcode = 0xE9;
			const INT offset;

			JumpInstruction(const INT offset) :
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

		template<class InjectAddrType = LPVOID, class HookAddrType = LPVOID>
		JumpHook(const InjectAddrType injectAddr, const HookAddrType hookAddr, const bool enabled = true) :
			patch(injectAddr, &JumpInstruction(((DWORD)(hookAddr)) - (((DWORD)(injectAddr)) +
				sizeof(JumpInstruction))), sizeof(JumpInstruction), enabled)
		{

			RequestAddressType(InjectAddrType);
			RequestAddressType(HookAddrType);

			assert((LPVOID)(injectAddr));
			assert((LPVOID)(hookAddr));

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

	class CallHook {
	private:

#pragma pack(push, 1)
		struct CallInstruction {

			const BYTE opcode = 0xE8;
			const INT offset;

			CallInstruction(const INT offset) :
				offset(offset) {}

		};
#pragma pack(pop)

		const LPVOID callFuncAddr;

		Patch patch;

	public:

		CallHook() = delete;
		CallHook(const CallHook& object) = delete;
		CallHook(CallHook&& object) = delete;

		CallHook& operator=(const CallHook& object) = delete;
		CallHook& operator=(CallHook&& object) = delete;

		template<class InjectAddrType = LPVOID, class HookAddrType = LPVOID>
		CallHook(const InjectAddrType injectAddr, const HookAddrType hookAddr, const bool enabled = true) :
			callFuncAddr((LPVOID)((((DWORD)(injectAddr)) + sizeof(CallInstruction)) + ((CallInstruction*)(injectAddr))->offset)),
			patch(injectAddr, &CallInstruction(((DWORD)(hookAddr)) - (((DWORD)(injectAddr)) + sizeof(CallInstruction))),
				sizeof(CallInstruction), enabled)
		{

			RequestAddressType(InjectAddrType);
			RequestAddressType(HookAddrType);

			assert((LPVOID)(injectAddr));
			assert((LPVOID)(hookAddr));

		}

		const Patch& GetPatch() const {

			return this->patch;

		}

		LPVOID GetCallFuncAddr() const {

			return this->callFuncAddr;

		}

		void Enable() {

			this->patch.Enable();

		}

		void Disable() {

			this->patch.Disable();

		}

	};

	using CallHookPtr = std::shared_ptr<CallHook>;

	class DllFuncHook {
	private:

		JumpHook hook;

	public:

		DllFuncHook() = delete;
		DllFuncHook(const DllFuncHook& object) = delete;
		DllFuncHook(DllFuncHook&& object) = delete;

		DllFuncHook& operator=(const DllFuncHook& object) = delete;
		DllFuncHook& operator=(DllFuncHook&& object) = delete;

		template<class HookAddrType = LPVOID>
		DllFuncHook(const std::string& moduleName, const std::string& functionName, const HookAddrType hookAddr, const bool enabled = true) :
			hook(GetProcAddress(GetModuleHandle(moduleName.c_str()), functionName.c_str()), hookAddr, enabled)
		{

			RequestAddressType(HookAddrType);

			assert((LPVOID)(hookAddr));

		}

		const JumpHook& GetHook() const {

			return this->hook;

		}

		void Enable() {

			this->hook.Enable();

		}

		void Disable() {

			this->hook.Disable();

		}

	};

	using DllFuncHookPtr = std::shared_ptr<DllFuncHook>;

	class Scanner {
	public:

		LPVOID memAddr;
		DWORD memSize;

	public:

		Scanner() = delete;

		template<class MemAddrType = LPVOID, class MemSizeType = DWORD>
		Scanner(const MemAddrType memAddr, const MemSizeType memSize) :
			memAddr((LPVOID)(memAddr)), memSize((DWORD)(memSize))
		{

			RequestAddressType(MemAddrType);
			RequestArithmeticType(MemSizeType);

			assert((LPVOID)(memAddr));
			assert((DWORD)(memSize));

		}

		LPVOID Find(PCCH pattern, PCCH mask) const {

			PCCH currentByte = (PCCH)(this->memAddr);
			PCCH lastByte = (PCCH)((DWORD)(this->memAddr) +
				this->memSize - strlen(mask));

			for (DWORD i; currentByte < lastByte; ++currentByte) {

				for (i = 0; mask[i]; ++i) if (mask[i] == 'x' &&
					pattern[i] != currentByte[i]) break;

				if (!mask[i]) break;

			}
			
			return currentByte != lastByte ? (LPVOID)(currentByte) : nullptr;

		}

	};

	using ScannerPtr = std::shared_ptr<Scanner>;

	template<class MemAddrType = LPVOID, class ModuleAddrType = LPVOID, class ModuleSizeType = DWORD>
	static inline bool GetModuleInfo(const MemAddrType memAddr, ModuleAddrType& moduleAddr, ModuleSizeType& moduleSize) {

		RequestAddressType(MemAddrType);
		RequestAddressType(ModuleAddrType);
		RequestArithmeticType(ModuleSizeType);

		assert((LPVOID)(memAddr));

		MEMORY_BASIC_INFORMATION info = {};

		if (!VirtualQuery((LPCVOID)(memAddr), &info, sizeof(info))) return false;

		const auto dos = (IMAGE_DOS_HEADER*)(info.AllocationBase);
		const auto pe = (IMAGE_NT_HEADERS*)((DWORD)(dos) + dos->e_lfanew);

		if (pe->Signature != IMAGE_NT_SIGNATURE) return false;

		if (!(moduleAddr = (ModuleAddrType)(info.AllocationBase))) return false;
		if (!(moduleSize = (ModuleSizeType)(pe->OptionalHeader.SizeOfImage))) return false;

		return true;

	}

	template<class MemAddrType = LPVOID, class MemSizeType = DWORD>
	static inline void FillWithNops(const MemAddrType memAddr, const MemSizeType memSize) {

		RequestAddressType(MemAddrType);
		RequestArithmeticType(MemSizeType);

		assert((LPVOID)(memAddr));
		assert((DWORD)(memSize));

		const UnprotectScope scope(memAddr, memSize);

		memset((LPVOID)(memAddr), 0x90, (DWORD)(memSize));

	}

}

#define MakeObjectContainer(ObjectType)	std::make_shared<Memory::ObjectContainer<ObjectType>>
#define MakeUnprotectScope				std::make_shared<Memory::UnprotectScope>
#define MakePatch						std::make_shared<Memory::Patch>
#define MakeJumpHook					std::make_shared<Memory::JumpHook>
#define MakeCallHook					std::make_shared<Memory::CallHook>
#define MakeDllFuncHook					std::make_shared<Memory::DllFuncHook>
#define MakeScanner						std::make_shared<Memory::Scanner>
