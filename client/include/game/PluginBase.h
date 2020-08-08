#pragma once

#include <cstddef>
#include <cstdint>

#define PLUGIN_API
#define _IGNORED_

#define VALIDATE_SIZE(st, size) static_assert(sizeof(st) == size, "invalid size structure")

namespace plugin {

	/*namespace Core {

		// Gets the SDK version
		PLUGIN_API unsigned int GetVersion();

	}*/

	struct dummy_func_t { }; // Dummy func tag type
	static const dummy_func_t dummy; // Dummy func tag object

// meta<> template
#if (defined(__GNUC__) || defined(__GNUG__) || defined(__clang__))
	template<auto F>
	struct meta;
#elif (defined(_MSC_VER))
	template<typename FT, FT>
	struct meta;
#else

#endif

	// helpers for event creating

	/*template<typename Meta>
	using event_t = std::conditional_t<std::is_same<typename Meta::calling_convention_t, CallingConventions::Thiscall>::value,
		ThiscallEvent<typename Meta::refs_t, Meta::cb_priority, typename Meta::args_t, typename Meta::def_t>,
		CdeclEvent<typename Meta::refs_t, Meta::cb_priority, typename Meta::args_t, typename Meta::def_t>
	>;

	template<typename Meta, typename Refs, int Priority = Meta::cb_priority, typename Args = typename Meta::args_t, typename Def = typename Meta::def_t>
	using custom_event_t = std::conditional_t<std::is_same<typename Meta::calling_convention_t, CallingConventions::Thiscall>::value,
		ThiscallEvent<Refs, Priority, Args, Def>,
		CdeclEvent<Refs, Priority, Args, Def>
	>;*/

	// multi-version address list
	template<int...>
	struct MvAddresses {};

	// Gets the virtual method table from the object @self
	inline void** GetVMT(const void* self) {
		return *(void***)(self);
	}

	// Gets the virtual method from @self in the table index @index 
	inline void* GetVMT(const void* self, size_t index) {
		return GetVMT(self)[index];
	}

	template <unsigned int address, typename... Args>
	void Call(Args... args) {
		reinterpret_cast<void(__cdecl *)(Args...)>(address)(args...);
	}

	template <typename Ret, unsigned int address, typename... Args>
	Ret CallAndReturn(Args... args) {
		return reinterpret_cast<Ret(__cdecl *)(Args...)>(address)(args...);
	}

	template <unsigned int address, typename C, typename... Args>
	void CallMethod(C _this, Args... args) {
		reinterpret_cast<void(__thiscall *)(C, Args...)>(address)(_this, args...);
	}

	template <typename Ret, unsigned int address, typename C, typename... Args>
	Ret CallMethodAndReturn(C _this, Args... args) {
		return reinterpret_cast<Ret(__thiscall *)(C, Args...)>(address)(_this, args...);
	}

	template <unsigned int tableIndex, typename C, typename... Args>
	void CallVirtualMethod(C _this, Args... args) {
		reinterpret_cast<void(__thiscall *)(C, Args...)>((*reinterpret_cast<void ***>(_this))[tableIndex])(_this, args...);
	}

	template <typename Ret, unsigned int tableIndex, typename C, typename... Args>
	Ret CallVirtualMethodAndReturn(C _this, Args... args) {
		return reinterpret_cast<Ret(__thiscall *)(C, Args...)>((*reinterpret_cast<void ***>(_this))[tableIndex])(_this, args...);
	}

	/*template <typename... Args>
	void CallDyn(unsigned int address, Args... args) {
		reinterpret_cast<void(__cdecl *)(Args...)>(GetGlobalAddress(address))(args...);
	}

	template <typename Ret, typename... Args>
	Ret CallAndReturnDyn(unsigned int address, Args... args) {
		return reinterpret_cast<Ret(__cdecl *)(Args...)>(GetGlobalAddress(address))(args...);
	}

	template <typename C, typename... Args>
	void CallMethodDyn(unsigned int address, C _this, Args... args) {
		reinterpret_cast<void(__thiscall *)(C, Args...)>(GetGlobalAddress(address))(_this, args...);
	}

	template <typename Ret, typename C, typename... Args>
	Ret CallMethodAndReturnDyn(unsigned int address, C _this, Args... args) {
		return reinterpret_cast<Ret(__thiscall *)(C, Args...)>(GetGlobalAddress(address))(_this, args...);
	}

	template <typename... Args>
	void CallDynGlobal(unsigned int address, Args... args) {
		reinterpret_cast<void(__cdecl *)(Args...)>(address)(args...);
	}

	template <typename Ret, typename... Args>
	Ret CallAndReturnDynGlobal(unsigned int address, Args... args) {
		return reinterpret_cast<Ret(__cdecl *)(Args...)>(address)(args...);
	}

	template <unsigned int address, typename C, typename... Args>
	void CallMethodDynGlobal(C _this, Args... args) {
		reinterpret_cast<void(__thiscall *)(C, Args...)>(address)(_this, args...);
	}

	template <typename Ret, unsigned int address, typename C, typename... Args>
	Ret CallMethodAndReturnDynGlobal(C _this, Args... args) {
		return reinterpret_cast<Ret(__thiscall *)(C, Args...)>(address)(_this, args...);
	}*/

}
