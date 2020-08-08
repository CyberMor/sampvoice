/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CLicensePlate.h"

#if defined(SAMP_R1)

SAMP::CLicensePlate*& SAMP::pLicensePlateManager() { return *(SAMP::CLicensePlate**)SAMP_ADDROF(0x21A100); }

SAMP::CLicensePlate::CLicensePlate(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CLicensePlate*, IDirect3DDevice9*))SAMP_ADDROF(0x692D0))(this, pDevice);
}

SAMP::CLicensePlate::~CLicensePlate() {
	((void(__thiscall*)(CLicensePlate*))SAMP_ADDROF(0x69300))(this);
}

void SAMP::CLicensePlate::OnLostDevice() {
	((void(__thiscall*)(CLicensePlate*))SAMP_ADDROF(0x690D0))(this);
}

void SAMP::CLicensePlate::OnResetDevice() {
	((void(__thiscall*)(CLicensePlate*))SAMP_ADDROF(0x69120))(this);
}

IDirect3DTexture9* SAMP::CLicensePlate::Create(const char* szText) {
	return ((IDirect3DTexture9 * (__thiscall*)(CLicensePlate*, const char*))SAMP_ADDROF(0x691A0))(this, szText);
}

#elif defined(SAMP_R3)

SAMP::CLicensePlate*& SAMP::pLicensePlateManager() { return *(SAMP::CLicensePlate**)SAMP_ADDROF(0x26E8E8); }

SAMP::CLicensePlate::CLicensePlate(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CLicensePlate*, IDirect3DDevice9*))SAMP_ADDROF(0x6D240))(this, pDevice);
}

SAMP::CLicensePlate::~CLicensePlate() {
	((void(__thiscall*)(CLicensePlate*))SAMP_ADDROF(0x6D270))(this);
}

void SAMP::CLicensePlate::OnLostDevice() {
	((void(__thiscall*)(CLicensePlate*))SAMP_ADDROF(0x6D040))(this);
}

void SAMP::CLicensePlate::OnResetDevice() {
	((void(__thiscall*)(CLicensePlate*))SAMP_ADDROF(0x6D090))(this);
}

IDirect3DTexture9* SAMP::CLicensePlate::Create(const char* szText) {
	return ((IDirect3DTexture9 * (__thiscall*)(CLicensePlate*, const char*))SAMP_ADDROF(0x6D110))(this, szText);
}

#endif
