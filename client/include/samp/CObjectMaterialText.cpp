/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CObjectMaterialText.h"

#if defined(SAMP_R3)

SAMP::CObjectMaterialText*& SAMP::pObjectMaterialTextManager() { return *(SAMP::CObjectMaterialText**)SAMP_ADDROF(0x26E8EC); }

SAMP::CObjectMaterialText::CObjectMaterialText(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CObjectMaterialText*, IDirect3DDevice9*))SAMP_ADDROF(0x70190))(this, pDevice);
}

SAMP::CObjectMaterialText::~CObjectMaterialText() {
	((void(__thiscall*)(CObjectMaterialText*))SAMP_ADDROF(0x701B0))(this);
}

void SAMP::CObjectMaterialText::OnLostDevice() {
	((void(__thiscall*)(CObjectMaterialText*))SAMP_ADDROF(0x70140))(this);
}

void SAMP::CObjectMaterialText::OnResetDevice() {
	((void(__thiscall*)(CObjectMaterialText*))SAMP_ADDROF(0x70170))(this);
}

IDirect3DTexture9* SAMP::CObjectMaterialText::Create(const char* szText, const char* szFont, char nFontSize, int nBgSizeX, int nBgSizeY, D3DCOLOR fontColor, D3DCOLOR bgColor, bool bBold, char align) {
	return ((IDirect3DTexture9 * (__thiscall*)(CObjectMaterialText*, const char*, const char*, char, int, int, D3DCOLOR, D3DCOLOR, bool, char))SAMP_ADDROF(0x701C0))(this, szText, szFont, nFontSize, nBgSizeX, nBgSizeY, fontColor, bgColor, bBold, align);
}

#endif
