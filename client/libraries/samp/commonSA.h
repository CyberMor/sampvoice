/*
	This is a SAMP API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#define SAMP_API

#define SAMP_BEGIN_NP namespace SAMP {
#define SAMP_END_NP }

#define SAMP_BEGIN __pragma(pack(push, 1)) namespace SAMP { 
#define SAMP_END } __pragma(pack(pop))

#define SAMP_ADDROF(offset) (SAMP::baseSampAddress + offset)

#define SAMP_UNUSED 0

#define SAMP_VIRTUAL {}

#if !defined(SAMP_R1) && !defined(SAMP_R3)
#define SAMP_R1
#endif

#include <d3d9.h>
#include <d3dx9.h>

class CDXUTDialog;
class CDXUTListBox;
class CDXUTEditBox;
class CDXUTScrollBar;
class CDXUTIMEEditBox;

typedef unsigned long TICK;

namespace SAMP {

	typedef int GTAREF;
	typedef unsigned short ID;
	typedef unsigned char NUMBER;

	extern DWORD baseSampAddress;
	extern void InitSamp(const DWORD baseAddress) noexcept;

}
