/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "VehicleSelection.h"

#if defined(SAMP_R1)

SAMP::CCameraSA*& SAMP::VehicleSelection::pCamera() { return *(SAMP::CCameraSA**)SAMP_ADDROF(0x13BA7C); }
SAMP::CVehicleSA*& SAMP::VehicleSelection::pVehicle() { return *(SAMP::CVehicleSA**)SAMP_ADDROF(0x13BB64); }
CPad*& SAMP::VehicleSelection::pControls() { return *(CPad**)SAMP_ADDROF(0x13BA78); }
BOOL& SAMP::VehicleSelection::bInitialized() { return *(BOOL*)SAMP_ADDROF(0x13BB60); }
int& SAMP::VehicleSelection::nSelectedModel() { return *(int*)SAMP_ADDROF(0x1014B4); }

void SAMP::VehicleSelection::ResetVehicle() {
	((void(__cdecl*)())SAMP_ADDROF(0x99710))();
}

void SAMP::VehicleSelection::Process() {
	((void(__cdecl*)())SAMP_ADDROF(0x99AD0))();
}

#elif defined(SAMP_R3)

SAMP::CCameraSA*& SAMP::VehicleSelection::pCamera() { return *(SAMP::CCameraSA**)SAMP_ADDROF(0x13BA7C); }
SAMP::CVehicleSA*& SAMP::VehicleSelection::pVehicle() { return *(SAMP::CVehicleSA**)SAMP_ADDROF(0x13BB64); }
CPad*& SAMP::VehicleSelection::pControls() { return *(CPad**)SAMP_ADDROF(0x13BA78); }
BOOL& SAMP::VehicleSelection::bInitialized() { return *(BOOL*)SAMP_ADDROF(0x13BB60); }
int& SAMP::VehicleSelection::nSelectedModel() { return *(int*)SAMP_ADDROF(0x1014B4); }

void SAMP::VehicleSelection::ResetVehicle() {
	((void(__cdecl*)())SAMP_ADDROF(0x99710))();
}

void SAMP::VehicleSelection::Process() {
	((void(__cdecl*)())SAMP_ADDROF(0x99AD0))();
}

#endif
