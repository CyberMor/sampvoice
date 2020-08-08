/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#define DEBUG_MODE_VEHICLE_SELECTION 10

SAMP_BEGIN

namespace Debug {

	int& nMode();
	void*& pFirstEntity();
	void*& pSecondEntity();

	void SetProperties(void* pFirstEntity, void* pSecondEntity, int nMode);
	void Disable();

}

SAMP_END
