/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CDialog.h"

#if defined(SAMP_R1)

SAMP::CDialog*& SAMP::pDialog() { return *(SAMP::CDialog**)SAMP_ADDROF(0x21A0B8); }

SAMP::CDialog::CDialog(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CDialog*, IDirect3DDevice9*))SAMP_ADDROF(0x6AE30))(this, pDevice);
}

void SAMP::CDialog::Hide() {
	((void(__thiscall*)(CDialog*))SAMP_ADDROF(0x6B210))(this);
}

void SAMP::CDialog::Close(char nProcessButton) {
	((void(__thiscall*)(CDialog*, char))SAMP_ADDROF(0x6C040))(this, nProcessButton);
}

void SAMP::CDialog::Draw() {
	((void(__thiscall*)(CDialog*))SAMP_ADDROF(0x6B240))(this);
}

void SAMP::CDialog::Show(int nId, int nType, const char* szCaption, const char* szText, const char* szLeftButton, const char* szRightButton, BOOL bServerside) {
	((void(__thiscall*)(CDialog*, int, int, const char*, const char*, const char*, const char*, BOOL))SAMP_ADDROF(0x6B9C0))(this, nId, nType, szCaption, szText, szLeftButton, szRightButton, bServerside);
}

void SAMP::CDialog::GetScreenRect(CRectSA* pRect) {
	((void(__thiscall*)(CDialog*, CRectSA*))SAMP_ADDROF(0x6B060))(this, pRect);
}

void SAMP::CDialog::UpdateRects() {
	((void(__thiscall*)(CDialog*))SAMP_ADDROF(0x6AEB0))(this);
}

int SAMP::CDialog::GetTextScreenLength(const char* pText) {
	return ((int(__thiscall*)(CDialog*, const char*))SAMP_ADDROF(0x6B0D0))(this, pText);
}

void SAMP::CDialog::ResetDialogControls(CDXUTDialog* pDialog) {
	return ((void(__thiscall*)(CDialog*, CDXUTDialog*))SAMP_ADDROF(0x6B3D0))(this, pDialog);
}

#elif defined(SAMP_R3)

SAMP::CDialog*& SAMP::pDialog() { return *(SAMP::CDialog**)SAMP_ADDROF(0x26E898); }

SAMP::CDialog::CDialog(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CDialog*, IDirect3DDevice9*))SAMP_ADDROF(0x6ED30))(this, pDevice);
}

void SAMP::CDialog::GetScreenRect(CRectSA* pRect) {
	((void(__thiscall*)(CDialog*, CRectSA*))SAMP_ADDROF(0x6EF60))(this, pRect);
}

int SAMP::CDialog::GetTextScreenLength(const char* szString) {
	return ((int(__thiscall*)(CDialog*, const char*))SAMP_ADDROF(0x6EF90))(this, szString);
}

void SAMP::CDialog::Hide() {
	((void(__thiscall*)(CDialog*))SAMP_ADDROF(0x6F110))(this);
}

void SAMP::CDialog::ResetDialogControls(CDXUTDialog* pDialog) {
	((void(__thiscall*)(CDialog*, CDXUTDialog*))SAMP_ADDROF(0x6F2D0))(this, pDialog);
}

void SAMP::CDialog::Show(int nId, int nType, const char* szCaption, const char* szText, const char* szLeftButton, const char* szRightButton, BOOL bServerside) {
	((void(__thiscall*)(CDialog*, int, int, const char*, const char*, const char*, const char*, BOOL))SAMP_ADDROF(0x6F8C0))(this, nId, nType, szCaption, szText, szLeftButton, szRightButton, bServerside);
}

void SAMP::CDialog::Close(char nProcessButton) {
	((void(__thiscall*)(CDialog*, char))SAMP_ADDROF(0x6FF40))(this, nProcessButton);
}

void SAMP::CDialog::Draw() {
	((void(__thiscall*)(CDialog*))SAMP_ADDROF(0x6F140))(this);
}

#endif
