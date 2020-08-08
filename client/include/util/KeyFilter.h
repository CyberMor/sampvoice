/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <atomic>

#include <Windows.h>
#include <SPSCQueue.h>

class KeyFilter {
public:

	struct KeyEvent {

		BYTE keyId;
		bool isPressed;
		int actKeys;

		KeyEvent() {}

		KeyEvent(
			const BYTE keyId,
			const bool isPressed,
			const int actKeys
		) :
			keyId(keyId),
			isPressed(isPressed),
			actKeys(actKeys)
		{}

	};

private:

	static SPSCQueue<KeyEvent> keyQueue;

	static std::atomic_int activeKeys;

	static std::atomic_bool pressedKeys[256];
	static std::atomic_bool statusKeys[256];

public:

	static void AddKey(const BYTE keyId);
	static void RemoveKey(const BYTE keyId);

	static void RemoveAllKeys();
	static void ReleaseAllKeys();

	static bool PopKey(KeyEvent& event);

	static VOID OnWndMessage(
		HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam
	);

};
