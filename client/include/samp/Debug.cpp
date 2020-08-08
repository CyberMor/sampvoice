/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "Debug.h"

int& SAMP::Debug::nMode() { return *(int*)SAMP_ADDROF(0x13BB18); }
void*& SAMP::Debug::pFirstEntity() { return *(void**)SAMP_ADDROF(0x13BB1C); }
void*& SAMP::Debug::pSecondEntity() { return *(void**)SAMP_ADDROF(0x13BB20); }

void SAMP::Debug::SetProperties(void* pFirstEntity, void* pSecondEntity, int nMode) {
	((void(__cdecl*)(void*, void*, int))SAMP_ADDROF(0x996E0))(pFirstEntity, pSecondEntity, nMode);
}

void SAMP::Debug::Disable() {
	((void(__cdecl*)())SAMP_ADDROF(0x99700))();
}
