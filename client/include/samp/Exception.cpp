/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "Exception.h"

#if defined(SAMP_R1)

int& SAMP::Exception::nCount() { return *(int*)SAMP_ADDROF(0x1118B0); }
void*& SAMP::Exception::pContextRecord() { return *(void**)SAMP_ADDROF(0x10D8A8); }
char* SAMP::Exception::szCrashDialogText() { return (char*)SAMP_ADDROF(0x10D8B0); }

BOOL SAMP::Exception::Print(int nCode, void* pExceptionPointers, const char* szWarning) {
	return ((BOOL(__stdcall*)(int, void*, const char*))SAMP_ADDROF(0x5CED0))(nCode, pExceptionPointers, szWarning);
}

void SAMP::Exception::SendCrashReport() {
	((void(__cdecl*)())SAMP_ADDROF(0x5CCC0))();
}

BOOL SAMP::Exception::CrashDialogProc(void* hWnd, unsigned int uMsg, unsigned int wParam, long lParam) {
	return ((BOOL(__stdcall*)(void*, unsigned int, unsigned int, long))SAMP_ADDROF(0x5CD90))(hWnd, uMsg, wParam, lParam);
}

void SAMP::Exception::ConstructCrashDialogText(BOOL bModules) {
	((void(__cdecl*)(BOOL))SAMP_ADDROF(0x5CAD0))(bModules);
}

long SAMP::Exception::Handler(void* pExceptionPointers) {
	return ((long(__stdcall*)(void*))SAMP_ADDROF(0x5CE90))(pExceptionPointers);
}

#elif defined(SAMP_R3)

int& SAMP::Exception::nCount() { return *(int*)SAMP_ADDROF(0x125930); }
void*& SAMP::Exception::pContextRecord() { return *(void**)SAMP_ADDROF(0x121928); }
char* SAMP::Exception::szCrashDialogText() { return (char*)SAMP_ADDROF(0x121930); }

BOOL SAMP::Exception::Print(int nCode, void* pExceptionPointers, const char* szWarning) {
	return ((BOOL(__stdcall*)(int, void*, const char*))SAMP_ADDROF(0x60270))(nCode, pExceptionPointers, szWarning);
}

void SAMP::Exception::SendCrashReport() {
	((void(__cdecl*)())SAMP_ADDROF(0x60060))();
}

BOOL SAMP::Exception::CrashDialogProc(void* hWnd, unsigned int uMsg, unsigned int wParam, long lParam) {
	return ((BOOL(__stdcall*)(void*, unsigned int, unsigned int, long))SAMP_ADDROF(0x60130))(hWnd, uMsg, wParam, lParam);
}

void SAMP::Exception::ConstructCrashDialogText(BOOL bModules) {
	((void(__cdecl*)(BOOL))SAMP_ADDROF(0x5FE70))(bModules);
}

long SAMP::Exception::Handler(void* pExceptionPointers) {
	return ((long(__stdcall*)(void*))SAMP_ADDROF(0x60230))(pExceptionPointers);
}

#endif
