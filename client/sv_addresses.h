#pragma once

#include <Windows.h>
#include <Psapi.h>

#include "memory.hpp"
#include "logger.h"

#include "samp/samp.h"

namespace addresses {
	
	// Адрес базы samp.dll
	static memory::address_t addr_samp_dll					= memory::null;

	// Адреса функций
	static memory::address_t func_destructor_addr			= memory::null;
	static memory::address_t func_add_to_chat_wnd			= memory::null;
	static memory::address_t func_add_client_cmd			= memory::null;
	static memory::address_t func_toggle_cursor				= memory::null;
	static memory::address_t func_cursor_unlock_actor_cam	= memory::null;

	// Адреса участков кода
	static memory::address_t code_call_destructor1_addr		= memory::null;
	static memory::address_t code_call_destructor2_addr		= memory::null;
	static memory::address_t code_post_init_sampinfo_addr	= memory::null;
	static memory::address_t code_init_rakclient_addr		= memory::null;

	// Инициализировать адреса
	static void init() {

		while (!(addr_samp_dll = (memory::address_t)(GetModuleHandle("samp.dll")))) Sleep(10);

		func_destructor_addr			= addr_samp_dll + SAMP_FUNC_CNETGAMEDESTRUCTOR;
		func_add_to_chat_wnd			= addr_samp_dll + SAMP_FUNC_ADDTOCHATWND;
		func_add_client_cmd				= addr_samp_dll + SAMP_FUNC_ADDCLIENTCMD;
		func_toggle_cursor				= addr_samp_dll + SAMP_FUNC_TOGGLECURSOR;
		func_cursor_unlock_actor_cam	= addr_samp_dll + SAMP_FUNC_CURSORUNLOCKACTORCAM;

		code_call_destructor1_addr		= addr_samp_dll + SAMP_HOOKENTER_CNETGAME_DESTR;
		code_call_destructor2_addr		= addr_samp_dll + SAMP_HOOKENTER_CNETGAME_DESTR2;
		code_post_init_sampinfo_addr	= addr_samp_dll + SAMP_HOOK_INIT_SAMP;
		code_init_rakclient_addr		= addr_samp_dll + SAMP_HOOK_INIT_RAKCLIENT;

	}

}
