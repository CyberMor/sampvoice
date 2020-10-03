/*
	This is a SAMP API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "commonSA.h"

DWORD SAMP::baseSampAddress { NULL };

void SAMP::InitSamp(const DWORD baseAddress) noexcept
{
	SAMP::baseSampAddress = baseAddress;
}
