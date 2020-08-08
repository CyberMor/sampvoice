/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "KeyFilter.h"

#include "Logger.h"

SPSCQueue<KeyFilter::KeyEvent> KeyFilter::keyQueue(64);

std::atomic_int KeyFilter::activeKeys(0);

std::atomic_bool KeyFilter::pressedKeys[256] = {};
std::atomic_bool KeyFilter::statusKeys[256] = {};

void KeyFilter::AddKey(const BYTE keyId) {

	Logger::LogToFile("[dbg:keyfilter] : adding key (id:0x%hhx)...", keyId);

	KeyFilter::statusKeys[keyId].store(true);

}

void KeyFilter::RemoveKey(const BYTE keyId) {

	Logger::LogToFile("[dbg:keyfilter] : removing key (id:0x%hhx)...", keyId);

	if (KeyFilter::statusKeys[keyId].exchange(false) && KeyFilter::pressedKeys[keyId].exchange(false))
		KeyFilter::keyQueue.push(KeyEvent(keyId, false, --KeyFilter::activeKeys));

}

void KeyFilter::RemoveAllKeys() {

	Logger::LogToFile("[dbg:keyfilter] : removing all keys...");

	for (DWORD keyId = 0; keyId < sizeof(KeyFilter::statusKeys); ++keyId)
		if (KeyFilter::statusKeys[keyId].exchange(false) && KeyFilter::pressedKeys[keyId].exchange(false))
			KeyFilter::keyQueue.push(KeyEvent(keyId, false, --KeyFilter::activeKeys));

	KeyFilter::activeKeys.store(0);

}

void KeyFilter::ReleaseAllKeys() {

	Logger::LogToFile("[dbg:keyfilter] : releasing all keys...");

	for (DWORD keyId = 0; keyId < sizeof(KeyFilter::statusKeys); ++keyId)
		if (KeyFilter::statusKeys[keyId].load() && KeyFilter::pressedKeys[keyId].exchange(false))
			KeyFilter::keyQueue.push(KeyEvent(keyId, false, --KeyFilter::activeKeys));

	KeyFilter::activeKeys.store(0);

}

bool KeyFilter::PopKey(KeyEvent& event) {

	if (KeyFilter::keyQueue.empty())
		return false;

	event = *KeyFilter::keyQueue.front();
	KeyFilter::keyQueue.pop();

	return true;

}

VOID KeyFilter::OnWndMessage(
	HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam
) {

	switch (uMsg) {
		case WM_KEYDOWN:
		{

			if (KeyFilter::statusKeys[(BYTE)(wParam)].load() &&
				!KeyFilter::pressedKeys[(BYTE)(wParam)].exchange(true))
				KeyFilter::keyQueue.push(KeyEvent((BYTE)(wParam),
					true, ++KeyFilter::activeKeys));

		} break;
		case WM_KEYUP:
		{

			if (KeyFilter::statusKeys[(BYTE)(wParam)].load() &&
				KeyFilter::pressedKeys[(BYTE)(wParam)].exchange(false))
				KeyFilter::keyQueue.push(KeyEvent((BYTE)(wParam),
					false, --KeyFilter::activeKeys));

		} break;
	}

}
