/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CHelpDialog.h"

#if defined(SAMP_R3)

SAMP::CHelpDialog*& SAMP::pHelpDialog() { return *(SAMP::CHelpDialog**)SAMP_ADDROF(0x26E8BC); }

SAMP::CHelpDialog::CHelpDialog(IDirect3DDevice9* pDevice) {
	((void(__thiscall*)(CHelpDialog*, IDirect3DDevice9*))SAMP_ADDROF(0x6B3B0))(this, pDevice);
}

void SAMP::CHelpDialog::Show() {
	((void(__thiscall*)(CHelpDialog*))SAMP_ADDROF(0x6B3C0))(this);
}

#endif
