/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>
#include <d3d9.h>

class ImGuiUtil {
private:

	static bool initStatus;
	static bool win32loadStatus;
	static bool dx9loadStatus;
	static bool renderStatus;

public:

	static bool Init(
		IDirect3DDevice9* pDevice,
		D3DPRESENT_PARAMETERS* pParameters
	);

	static bool IsInited();

	static bool RenderBegin();
	static bool IsRendering();
	static void RenderEnd();

	static LRESULT OnWndMessage(
		HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam
	);

	static void Free();

};
