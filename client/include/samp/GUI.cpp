/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "GUI.h"

#if defined(SAMP_R1)

CDXUTDialogResourceManager*& SAMP::GUI::pResourceMgr() { return *(CDXUTDialogResourceManager**)SAMP_ADDROF(0x21A180); }
CDXUTDialog*& SAMP::GUI::pGameUi() { return *(CDXUTDialog**)SAMP_ADDROF(0x21A184); }
CDXUTDialog*& SAMP::GUI::pScoreboard() { return *(CDXUTDialog**)SAMP_ADDROF(0x21A188); }
CDXUTDialog*& SAMP::GUI::pDialog() { return *(CDXUTDialog**)SAMP_ADDROF(0x21A190); }
CDXUTDialog*& SAMP::GUI::pClassSelection() { return *(CDXUTDialog**)SAMP_ADDROF(0x21A18C); }
IDirect3DSurface9*& SAMP::GUI::pCursor() { return *(IDirect3DSurface9**)SAMP_ADDROF(0x21A198); }
IDirect3DDevice9*& SAMP::GUI::pDevice() { return *(IDirect3DDevice9**)SAMP_ADDROF(0x21A0A8); }

void SAMP::GUI::Initialize() {
	((void(__cdecl*)())SAMP_ADDROF(0xB3FC0))();
}

void SAMP::GUI::OnLostDevice() {
	((void(__cdecl*)())SAMP_ADDROF(0xB2980))();
}

void SAMP::GUI::OnResetDevice() {
	((void(__cdecl*)())SAMP_ADDROF(0xB2BE0))();
}

void SAMP::GUI::GameUIEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext) {
	((void(__stdcall*)(unsigned int, int, CDXUTControl*, void*))SAMP_ADDROF(0xB3EE0))(nEvent, nControlId, pControl, pUserContext);
}

void SAMP::GUI::ClassSelectorEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext) {
	((void(__stdcall*)(unsigned int, int, CDXUTControl*, void*))SAMP_ADDROF(0xB3F50))(nEvent, nControlId, pControl, pUserContext);
}

void SAMP::GUI::ScoreboardEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext) {
	((void(__stdcall*)(unsigned int, int, CDXUTControl*, void*))SAMP_ADDROF(0xB3F20))(nEvent, nControlId, pControl, pUserContext);
}

void SAMP::GUI::DialogEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext) {
	((void(__stdcall*)(unsigned int, int, CDXUTControl*, void*))SAMP_ADDROF(0xB3E50))(nEvent, nControlId, pControl, pUserContext);
}

#elif defined(SAMP_R3)

CDXUTDialogResourceManager*& SAMP::GUI::pResourceMgr() { return *(CDXUTDialogResourceManager**)SAMP_ADDROF(0x26E968); }
CDXUTDialog*& SAMP::GUI::pGameUi() { return *(CDXUTDialog**)SAMP_ADDROF(0x26E96C); }
CDXUTDialog*& SAMP::GUI::pScoreboard() { return *(CDXUTDialog**)SAMP_ADDROF(0x26E970); }
CDXUTDialog*& SAMP::GUI::pDialog() { return *(CDXUTDialog**)SAMP_ADDROF(0x26E978); }
CDXUTDialog*& SAMP::GUI::pClassSelection() { return *(CDXUTDialog**)SAMP_ADDROF(0x26E974); }
IDirect3DSurface9*& SAMP::GUI::pCursor() { return *(IDirect3DSurface9**)SAMP_ADDROF(0x26E984); }
IDirect3DDevice9*& SAMP::GUI::pDevice() { return *(IDirect3DDevice9**)SAMP_ADDROF(0x26E888); }

void SAMP::GUI::Initialize() {
	((void(__cdecl*)())SAMP_ADDROF(0xC5EB0))();
}

void SAMP::GUI::OnLostDevice() {
	((void(__cdecl*)())SAMP_ADDROF(0xC47D0))();
}

void SAMP::GUI::OnResetDevice() {
	((void(__cdecl*)())SAMP_ADDROF(0xC4A30))();
}

void SAMP::GUI::GameUIEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext) {
	((void(__stdcall*)(unsigned int, int, CDXUTControl*, void*))SAMP_ADDROF(0xC5DC0))(nEvent, nControlId, pControl, pUserContext);
}

void SAMP::GUI::ScoreboardEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext) {
	((void(__stdcall*)(unsigned int, int, CDXUTControl*, void*))SAMP_ADDROF(0xC5E00))(nEvent, nControlId, pControl, pUserContext);
}

void SAMP::GUI::DialogEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext) {
	((void(__stdcall*)(unsigned int, int, CDXUTControl*, void*))SAMP_ADDROF(0xC5D30))(nEvent, nControlId, pControl, pUserContext);
}

void SAMP::GUI::ClassSelectionEventHandler(unsigned int nEvent, int nControlId, CDXUTControl* pControl, void* pUserContext) {
	((void(__stdcall*)(unsigned int, int, CDXUTControl*, void*))SAMP_ADDROF(0xC5E30))(nEvent, nControlId, pControl, pUserContext);
}

#endif
