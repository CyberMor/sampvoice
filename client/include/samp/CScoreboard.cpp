/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CScoreboard.h"

#if defined(SAMP_R1)

SAMP::CScoreboard*& SAMP::pScoreboard() { return *(SAMP::CScoreboard**)SAMP_ADDROF(0x21A0B4); }

SAMP::CScoreboard::CScoreboard(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CScoreboard*, IDirect3DDevice9*))SAMP_ADDROF(0x6A370))(this, pDevice);
}

void SAMP::CScoreboard::GetRect(CRectSA* pRect) {
	((void(__thiscall*)(CScoreboard*, CRectSA*))SAMP_ADDROF(0x6A2D0))(this, pRect);
}

void SAMP::CScoreboard::ResetDialogControls(CDXUTDialog* pDialog) {
	((void(__thiscall*)(CScoreboard*, CDXUTDialog*))SAMP_ADDROF(0x6A3F0))(this, pDialog);
}

void SAMP::CScoreboard::Enable() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6A3D0))(this);
}

void SAMP::CScoreboard::Close(bool bHideCursor) {
	((void(__thiscall*)(CScoreboard*, bool))SAMP_ADDROF(0x6A320))(this, bHideCursor);
}

void SAMP::CScoreboard::SendNotification() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6A550))(this);
}

void SAMP::CScoreboard::Recalc() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6A270))(this);
}

void SAMP::CScoreboard::Draw() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6AA10))(this);
}

void SAMP::CScoreboard::UpdateList() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6A680))(this);
}

#elif defined(SAMP_R3)

SAMP::CScoreboard*& SAMP::pScoreboard() { return *(SAMP::CScoreboard**)SAMP_ADDROF(0x26E894); }

SAMP::CScoreboard::CScoreboard(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CScoreboard*, IDirect3DDevice9*))SAMP_ADDROF(0x6E2C0))(this, pDevice);
}

void SAMP::CScoreboard::Recalc() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6E1C0))(this);
}

void SAMP::CScoreboard::GetRect(CRectSA* pRect) {
	((void(__thiscall*)(CScoreboard*, CRectSA*))SAMP_ADDROF(0x6E220))(this, pRect);
}

void SAMP::CScoreboard::Close(bool bHideCursor) {
	((void(__thiscall*)(CScoreboard*, bool))SAMP_ADDROF(0x6E270))(this, bHideCursor);
}

void SAMP::CScoreboard::ResetDialogControls(CDXUTDialog* pDialog) {
	((void(__thiscall*)(CScoreboard*, CDXUTDialog*))SAMP_ADDROF(0x6E340))(this, pDialog);
}

void SAMP::CScoreboard::SendNotification() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6E4A0))(this);
}

void SAMP::CScoreboard::UpdateList() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6E5C0))(this);
}

void SAMP::CScoreboard::Draw() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6E960))(this);
}

void SAMP::CScoreboard::Enable() {
	((void(__thiscall*)(CScoreboard*))SAMP_ADDROF(0x6EC80))(this);
}

#endif
