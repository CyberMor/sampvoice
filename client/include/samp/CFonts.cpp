/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CFonts.h"

#if defined(SAMP_R1)

SAMP::CFonts*& SAMP::pFontRenderer() { return *(SAMP::CFonts**)SAMP_ADDROF(0x21A0FC); }

SAMP::CFonts::CFonts(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CFonts*, IDirect3DDevice9*))SAMP_ADDROF(0x67410))(this, pDevice);
}

SAMP::CFonts::~CFonts() {
	((void(__thiscall*)(CFonts*))SAMP_ADDROF(0x66A20))(this);
}

void SAMP::CFonts::OnLostDevice() {
	((void(__thiscall*)(CFonts*))SAMP_ADDROF(0x66AA0))(this);
}

void SAMP::CFonts::OnResetDevice() {
	((void(__thiscall*)(CFonts*))SAMP_ADDROF(0x66AE0))(this);
}

void SAMP::CFonts::GetTextScreenSize(void* pPoint, const char* pText, int nFormat) {
	((void(__thiscall*)(CFonts*, void*, const char*, unsigned long))SAMP_ADDROF(0x66B20))(this, pPoint, pText, nFormat);
}

void SAMP::CFonts::GetLittleTextScreenSize(void* pPoint, const char* pText, int nFormat) {
	((void(__thiscall*)(CFonts*, void*, const char*, unsigned long))SAMP_ADDROF(0x66BD0))(this, pPoint, pText, nFormat);
}

void SAMP::CFonts::DrawText(ID3DXSprite* pSprite, const char* szText, CRectSA rect, D3DCOLOR color, BOOL bShadow) {
	((void(__thiscall*)(CFonts*, ID3DXSprite*, const char*, CRectSA, D3DCOLOR, BOOL))SAMP_ADDROF(0x66C80))(this, pSprite, szText, rect, color, bShadow);
}

void SAMP::CFonts::DrawLittleText(ID3DXSprite* pSprite, const char* szText, CRectSA rect, int nFormat, D3DCOLOR color, BOOL bShadow) {
	((void(__thiscall*)(CFonts*, ID3DXSprite*, const char*, CRectSA, int, D3DCOLOR, BOOL))SAMP_ADDROF(0x66E00))(this, pSprite, szText, rect, nFormat, color, bShadow);
}

void SAMP::CFonts::Reset() {
	((void(__thiscall*)(CFonts*))SAMP_ADDROF(0x67200))(this);
}

void SAMP::CFonts::DrawLicensePlateText(const char* szText, CRectSA rect, D3DCOLOR color) {
	((void(__thiscall*)(CFonts*, const char*, CRectSA, D3DCOLOR))SAMP_ADDROF(0x66F70))(this, szText, rect, color);
}

#elif defined(SAMP_R3)

SAMP::CFonts*& SAMP::pFontRenderer() { return *(SAMP::CFonts**)SAMP_ADDROF(0x26E8E4); }

SAMP::CFonts::CFonts(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CFonts*, IDirect3DDevice9*))SAMP_ADDROF(0x6B380))(this, pDevice);
}

SAMP::CFonts::~CFonts() {
	((void(__thiscall*)(CFonts*))SAMP_ADDROF(0x6A990))(this);
}

void SAMP::CFonts::OnLostDevice() {
	((void(__thiscall*)(CFonts*))SAMP_ADDROF(0x6AA10))(this);
}

void SAMP::CFonts::OnResetDevice() {
	((void(__thiscall*)(CFonts*))SAMP_ADDROF(0x6AA50))(this);
}

void SAMP::CFonts::GetTextScreenSize(void* pSize, const char* szText, int nFormat) {
	((void(__thiscall*)(CFonts*, void*, const char*, int))SAMP_ADDROF(0x6AA90))(this, pSize, szText, nFormat);
}

void SAMP::CFonts::GetLittleTextScreenSize(void* pSize, const char* szText, int nFormat) {
	((void(__thiscall*)(CFonts*, void*, const char*, int))SAMP_ADDROF(0x6AB40))(this, pSize, szText, nFormat);
}

void SAMP::CFonts::DrawText(ID3DXSprite* pSprite, const char* szText, CRectSA rect, D3DCOLOR color, BOOL bShadow) {
	((void(__thiscall*)(CFonts*, ID3DXSprite*, const char*, CRectSA, D3DCOLOR, BOOL))SAMP_ADDROF(0x6ABF0))(this, pSprite, szText, rect, color, bShadow);
}

void SAMP::CFonts::DrawLittleText(ID3DXSprite* pSprite, const char* szText, CRectSA rect, int nFormat, D3DCOLOR color, BOOL bShadow) {
	((void(__thiscall*)(CFonts*, ID3DXSprite*, const char*, CRectSA, int, D3DCOLOR, BOOL))SAMP_ADDROF(0x6AD70))(this, pSprite, szText, rect, nFormat, color, bShadow);
}

void SAMP::CFonts::DrawLicensePlateText(const char* szText, CRectSA rect, D3DCOLOR color) {
	((void(__thiscall*)(CFonts*, const char*, CRectSA, D3DCOLOR))SAMP_ADDROF(0x6AEE0))(this, szText, rect, color);
}

void SAMP::CFonts::Reset() {
	((void(__thiscall*)(CFonts*))SAMP_ADDROF(0x6B170))(this);
}

#endif
