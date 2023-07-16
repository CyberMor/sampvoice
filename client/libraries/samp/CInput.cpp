/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CInput.h"

#if defined(SAMP_R1)

SAMP::CInput*& SAMP::pInputBox() { return *(SAMP::CInput**)SAMP_ADDROF(0x21A0E8); }

SAMP::CInput::CInput(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CInput*, IDirect3DDevice9*))SAMP_ADDROF(0x65730))(this, pDevice);
}

void SAMP::CInput::GetRect(CRectSA* pRect) {
	((void(__thiscall*)(CInput*, CRectSA*))SAMP_ADDROF(0x657A0))(this, pRect);
}

void SAMP::CInput::Open() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x657E0))(this);
}

void SAMP::CInput::Close() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x658E0))(this);
}

void SAMP::CInput::AddRecall(const char* pText) {
	((void(__thiscall*)(CInput*, const char*))SAMP_ADDROF(0x65930))(this, pText);
}

void SAMP::CInput::RecallUp() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x65990))(this);
}

void SAMP::CInput::RecallDown() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x65A00))(this);
}

void SAMP::CInput::EnableCursor() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x65A50))(this);
}

SAMP::CMDPROC SAMP::CInput::GetCommandHandler(const char* pName) {
	return ((CMDPROC(__thiscall*)(CInput*, const char*))SAMP_ADDROF(0x65A70))(this, pName);
}

void SAMP::CInput::SetDefaultCommand(CMDPROC pProc) {
	((void(__thiscall*)(CInput*, CMDPROC))SAMP_ADDROF(0x65AC0))(this, pProc);
}

void SAMP::CInput::AddCommand(const char* pName, CMDPROC pProc) {
	((void(__thiscall*)(CInput*, const char*, CMDPROC))SAMP_ADDROF(0x65AD0))(this, pName, pProc);
}

int SAMP::CInput::MsgProc(int uMsg, int wParam, int lParam) {
	return ((int(__thiscall*)(CInput*, int, int, int))SAMP_ADDROF(0x65B30))(this, uMsg, wParam, lParam);
}

void SAMP::CInput::ResetDialogControls(CDXUTDialog* pGameUI) {
	((void(__thiscall*)(CInput*, CDXUTDialog*))SAMP_ADDROF(0x65BA0))(this, pGameUI);
}

void SAMP::CInput::Send(const char* szString) {
	((void(__thiscall*)(CInput*, const char*))SAMP_ADDROF(0x65C60))(this, szString);
}

void SAMP::CInput::ProcessInput() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x65D30))(this);
}

#elif defined(SAMP_R3)

SAMP::CInput*& SAMP::pInputBox() { return *(SAMP::CInput**)SAMP_ADDROF(0x26E8CC); }

SAMP::CInput::CInput(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CInput*, IDirect3DDevice9*))SAMP_ADDROF(0x68C60))(this, pDevice);
}

void SAMP::CInput::GetRect(CRectSA* pRect) {
	((void(__thiscall*)(CInput*, CRectSA*))SAMP_ADDROF(0x68CD0))(this, pRect);
}

void SAMP::CInput::Open() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x68D10))(this);
}

void SAMP::CInput::Close() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x68E10))(this);
}

void SAMP::CInput::AddRecall(const char* szString) {
	((void(__thiscall*)(CInput*, const char*))SAMP_ADDROF(0x68E60))(this, szString);
}

void SAMP::CInput::RecallUp() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x68EC0))(this);
}

void SAMP::CInput::RecallDown() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x68F30))(this);
}

void SAMP::CInput::EnableCursor() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x68F80))(this);
}

SAMP::CMDPROC SAMP::CInput::GetCommandHandler(const char* szName) {
	return ((CMDPROC(__thiscall*)(CInput*, const char*))SAMP_ADDROF(0x68FA0))(this, szName);
}

void SAMP::CInput::SetDefaultCommand(CMDPROC proc) {
	((void(__thiscall*)(CInput*, CMDPROC))SAMP_ADDROF(0x68FF0))(this, proc);
}

void SAMP::CInput::AddCommand(const char* szName, CMDPROC handler) {
	((void(__thiscall*)(CInput*, const char*, CMDPROC))SAMP_ADDROF(0x69000))(this, szName, handler);
}

BOOL SAMP::CInput::MsgProc(int uMsg, int wParam, int lParam) {
	return ((BOOL(__thiscall*)(CInput*, int, int, int))SAMP_ADDROF(0x69060))(this, uMsg, wParam, lParam);
}

void SAMP::CInput::ResetDialogControls(CDXUTDialog* pGameUi) {
	((void(__thiscall*)(CInput*, CDXUTDialog*))SAMP_ADDROF(0x690D0))(this, pGameUi);
}

void SAMP::CInput::Send(const char* szString) {
	((void(__thiscall*)(CInput*, const char*))SAMP_ADDROF(0x69190))(this, szString);
}

void SAMP::CInput::ProcessInput() {
	((void(__thiscall*)(CInput*))SAMP_ADDROF(0x69260))(this);
}

#endif
