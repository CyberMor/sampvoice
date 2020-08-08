/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CVehicleSA.h"
#include "CCameraSA.h"
#include "KeyStuff.h"

SAMP_BEGIN

namespace VehicleSelection {

	CCameraSA*&		pCamera();
	CVehicleSA*&	pVehicle();
	CPad*&			pControls();
	BOOL&			bInitialized();
	int&			nSelectedModel();

	void ResetVehicle();
	void Process();

}

SAMP_END

#elif defined(SAMP_R3)

#include "CVehicleSA.h"
#include "CCameraSA.h"
#include "KeyStuff.h"

SAMP_BEGIN

namespace VehicleSelection {

	CCameraSA*& pCamera();
	CVehicleSA*& pVehicle();
	CPad*& pControls();
	BOOL& bInitialized();
	int& nSelectedModel();

	void ResetVehicle();
	void Process();

}

SAMP_END

#endif
