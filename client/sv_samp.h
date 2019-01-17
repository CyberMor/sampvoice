#pragma once

#include <Windows.h>
#include <Psapi.h>

#include <d3d9.h>
#include <samp/samp.h>

#include "logger.h"
#include "memory.hpp"

/// [dependency]
namespace samp {

	// Статус модуля
	static bool status = false;

	// Перехватчики
	static memory::hooks::jump_hook *hook_samp_init			= nullptr;
	static memory::hooks::call_hook *hook_call_destruct_1	= nullptr;
	static memory::hooks::call_hook *hook_call_destruct_2	= nullptr;

	// Потоки
	static HANDLE thread_init		= NULL;

	// Модули samp
	static stSAMP		*pSamp		= nullptr;
	static stChatInfo	*pChat		= nullptr;
	static stInputInfo	*pInput		= nullptr;
	static stGameInfo	*pMiscInfo	= nullptr;

	// Обработчики
	static void (CALLBACK *handler_exit)()	= nullptr;

	// -------------------------------

	// Перехватчик деструктора samp_info
	static void __declspec(naked) hook_func_samp_destruct() {

		static void *ret_addr;
		__asm {
			push eax
			mov eax, dword ptr[esp + 4]
			mov ret_addr, eax
			pop eax
		}

		__asm {
			pushad
			mov ebp, esp
			sub esp, __LOCAL_SIZE
		}

		delete hook_call_destruct_1;
		delete hook_call_destruct_2;

		if (handler_exit) handler_exit();

		samp::status = false;

		__asm {
			mov esp, ebp
			popad
			jmp ret_addr
		}

	}

	// Обработчик инициализации samp
	static void __declspec(naked) samp_init() {

		static void *ret_addr;

		__asm {
			pushad
			mov ebp, esp
			sub esp, __LOCAL_SIZE
		}

		if ((pSamp = *(stSAMP**)(addresses::addr_samp_dll + SAMP_INFO_OFFSET)) &&
			(pChat = *(stChatInfo**)(addresses::addr_samp_dll + SAMP_CHAT_INFO_OFFSET)) &&
			(pInput = *(stInputInfo**)(addresses::addr_samp_dll + SAMP_CHAT_INPUT_INFO_OFFSET)) &&
			(pMiscInfo = *(stGameInfo**)(addresses::addr_samp_dll + SAMP_MISC_INFO))
		) {
			samp::status = true;
		}

		ret_addr = hook_samp_init->get_original_addr();
		delete hook_samp_init;

		ResumeThread(thread_init);

		__asm {
			mov esp, ebp
			popad
			jmp ret_addr
		}

	}

	// Инициализировать модуль samp
	static bool init(
		void(CALLBACK *handler_exit)() = nullptr
	) {

		if (samp::status) return false;
		samp::handler_exit = handler_exit;

		if (!(hook_samp_init = new memory::hooks::jump_hook(addresses::code_post_init_sampinfo_addr, samp_init)) ||
			!(hook_call_destruct_1 = new memory::hooks::call_hook(addresses::code_call_destructor1_addr, hook_func_samp_destruct)) ||
			!(hook_call_destruct_2 = new memory::hooks::call_hook(addresses::code_call_destructor2_addr, hook_func_samp_destruct))
		) {
			LogError("samp", "could not allocate memory for hooks");
			return false;
		}

		// Входим в режим ожидания
		if (DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &thread_init, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
			if (SuspendThread(thread_init) == -1) {
				LogError("samp", "could not suspend thread");
				CloseHandle(thread_init);
				return false;
			} CloseHandle(thread_init);
		} else {
			LogError("samp", "could not duplicate handle current thread");
			return false;
		}

		return true;

	}

	// Выгрузить модуль samp
	static void free() {
		
		if (!samp::status) return;

		delete hook_call_destruct_1;
		delete hook_call_destruct_2;

		samp::status = false;

	}

	// -------------------------------

	// Добавить команду в чат
	static bool add_client_command(const char *cmd_name, CMDPROC cmd_handler) {

		if (!samp::status) return false;
		if (!cmd_name || !cmd_handler || !addresses::addr_samp_dll || !pInput) return false;
		if (pInput->iCMDCount == (SAMP_MAX_CLIENTCMDS - 1)) return false;
		if (strlen(cmd_name) > 30) return false;

		((void(__thiscall *) (void *_this, const char *, CMDPROC)) (addresses::func_add_client_cmd)) (pInput, cmd_name, cmd_handler);
		return true;

	}

	// Вывести сообщение в чат
	template<typename... ARGS>
	static inline void add_message_to_chat(D3DCOLOR color, const char *message, ARGS... args) {

		if (!samp::status) return;
		if (!message || !addresses::addr_samp_dll || !pChat) return;

		char buffer[256];
		sprintf_s(buffer, message, args...);

		void(__thiscall *AddToChatWindowBuffer) (void *, ChatMessageType, const char *, const char *, D3DCOLOR, D3DCOLOR) =
			(void(__thiscall *) (void *_this, ChatMessageType Type, const char *szString, const char *szPrefix, D3DCOLOR TextColor, D3DCOLOR PrefixColor))
			(addresses::func_add_to_chat_wnd);

		AddToChatWindowBuffer(pChat, CHAT_TYPE_DEBUG, buffer, nullptr, color, 0);

	}

	// Показать/Спрятать курсор мыши
	bool toggle_samp_cursor(bool state) {

		if (!samp::status) return false;
		if (!addresses::addr_samp_dll || !pMiscInfo) return false;
		if (!pSamp || pInput->iInputEnabled) return false;

		((void(__thiscall *) (void *, int, bool)) (addresses::func_toggle_cursor))(pMiscInfo, state ? 3 : 0, !state);
		if (!state) ((void(__thiscall *) (void *)) (addresses::func_cursor_unlock_actor_cam))(pMiscInfo);
		return true;

	}

}
