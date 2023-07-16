/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CVehicleSA.h"

#if defined(SAMP_R1)

SAMP::CVehicleSA::CVehicleSA(int nModel, CVectorSA position, float fRotation, BOOL bKeepModelLoaded, BOOL bHasSiren) {
	((void(__thiscall*)(CVehicleSA*, int, CVectorSA, float, BOOL, BOOL))SAMP_ADDROF(0xB1E70))(this, nModel, position, fRotation, bKeepModelLoaded, bHasSiren);
}

void SAMP::CVehicleSA::ResetPointers() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0B70))(this);
}

void SAMP::CVehicleSA::ChangeInterior(int nInterior) {
	((void(__thiscall*)(CVehicleSA*, int))SAMP_ADDROF(0xB0B40))(this, nInterior);
}

BOOL SAMP::CVehicleSA::IsOccupied() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0BE0))(this);
}

void SAMP::CVehicleSA::SetInvulnerable(BOOL bInv) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB0C40))(this, bInv);
}

void SAMP::CVehicleSA::SetLocked(BOOL bLocked) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB0CE0))(this, bLocked);
}

float SAMP::CVehicleSA::GetHealth() {
	return ((float(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0D50))(this);
}

void SAMP::CVehicleSA::SetHealth(float fValue) {
	((void(__thiscall*)(CVehicleSA*, float))SAMP_ADDROF(0xB0D70))(this, fValue);
}

void SAMP::CVehicleSA::SetColor(NUMBER nPrimary, NUMBER nSecondary) {
	((void(__thiscall*)(CVehicleSA*, NUMBER, NUMBER))SAMP_ADDROF(0xB0D90))(this, nPrimary, nSecondary);
}

int SAMP::CVehicleSA::GetSubtype() {
	return ((int(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0E40))(this);
}

BOOL SAMP::CVehicleSA::IsSunk() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0E60))(this);
}

BOOL SAMP::CVehicleSA::IsWrecked() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0E80))(this);
}

BOOL SAMP::CVehicleSA::DriverIsPlayerPed() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0EA0))(this);
}

BOOL SAMP::CVehicleSA::IsTrainPart() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0F10))(this);
}

void SAMP::CVehicleSA::ProcessMarkers() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1190))(this);
}

void SAMP::CVehicleSA::Lock(BOOL bLock) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB12F0))(this, bLock);
}

BOOL SAMP::CVehicleSA::UpdateLastDrivenTime() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1320))(this);
}

char SAMP::CVehicleSA::GetTires() {
	return ((char(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB14E0))(this);
}

void SAMP::CVehicleSA::UpdateDamage(int nPanels, int nDoors, char nLights) {
	((void(__thiscall*)(CVehicleSA*, int, int, char))SAMP_ADDROF(0xB1570))(this, nPanels, nDoors, nLights);
}

int SAMP::CVehicleSA::GetPanelsDamage() {
	return ((int(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1630))(this);
}

int SAMP::CVehicleSA::GetDoorsDamage() {
	return ((int(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1660))(this);
}

char SAMP::CVehicleSA::GetLightsDamage() {
	return ((char(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1690))(this);
}

void SAMP::CVehicleSA::AttachTrailer() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB16C0))(this);
}

void SAMP::CVehicleSA::DetachTrailer() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB16E0))(this);
}

void SAMP::CVehicleSA::SetTrailer(CVehicleSA* pTrailer) {
	((void(__thiscall*)(CVehicleSA*, CVehicleSA*))SAMP_ADDROF(0xB1730))(this, pTrailer);
}

SAMP::CVehicleSA* SAMP::CVehicleSA::GetTrailer() {
	return ((CVehicleSA * (__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1740))(this);
}

void SAMP::CVehicleSA::SetLicensePlateText(const char* szText) {
	((void(__thiscall*)(CVehicleSA*, const char*))SAMP_ADDROF(0xB1BF0))(this, szText);
}

void SAMP::CVehicleSA::SetRotation(float fValue) {
	((void(__thiscall*)(CVehicleSA*, float))SAMP_ADDROF(0xB1C10))(this, fValue);
}

void SAMP::CVehicleSA::ConstructLicensePlate() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1C40))(this);
}

void SAMP::CVehicleSA::ShutdownLicensePlate() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1C90))(this);
}

BOOL SAMP::CVehicleSA::HasDriver() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0B90))(this);
}

void SAMP::CVehicleSA::UpdateColor() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0DE0))(this);
}

BOOL SAMP::CVehicleSA::HasPlayerPed() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0ED0))(this);
}

BOOL SAMP::CVehicleSA::HasTurret() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB0F50))(this);
}

void SAMP::CVehicleSA::EnableSiren(bool bEnable) {
	((void(__thiscall*)(CVehicleSA*, bool))SAMP_ADDROF(0xB0FF0))(this, bEnable);
}

BOOL SAMP::CVehicleSA::SirenEnabled() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1010))(this);
}

void SAMP::CVehicleSA::SetLandingGearState(BOOL bHide) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB1050))(this, bHide);
}

BOOL SAMP::CVehicleSA::GetLandingGearState() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB10E0))(this);
}

void SAMP::CVehicleSA::SetHydraThrusters(int nDirection) {
	((void(__thiscall*)(CVehicleSA*, int))SAMP_ADDROF(0xB1150))(this, nDirection);
}

int SAMP::CVehicleSA::GetHydraThrusters() {
	return ((int(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1170))(this);
}

float SAMP::CVehicleSA::GetTrainSpeed() {
	return ((float(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1390))(this);
}

void SAMP::CVehicleSA::SetTrainSpeed(float fValue) {
	((void(__thiscall*)(CVehicleSA*, float))SAMP_ADDROF(0xB13B0))(this, fValue);
}

void SAMP::CVehicleSA::SetTires(char nState) {
	((void(__thiscall*)(CVehicleSA*, char))SAMP_ADDROF(0xB13F0))(this, nState);
}

SAMP::CVehicleSA* SAMP::CVehicleSA::GetTractor() {
	return ((CVehicleSA * (__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB17A0))(this);
}

BOOL SAMP::CVehicleSA::IsTrailer() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1820))(this);
}

BOOL SAMP::CVehicleSA::IsTowtruck() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1880))(this);
}

BOOL SAMP::CVehicleSA::IsRC() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB18B0))(this);
}

void SAMP::CVehicleSA::EnableLights(bool bEnable) {
	((void(__thiscall*)(CVehicleSA*, bool))SAMP_ADDROF(0xB1900))(this, bEnable);
}

void SAMP::CVehicleSA::RemovePassengers() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1990))(this);
}

BOOL SAMP::CVehicleSA::AddComponent(unsigned short nId) {
	return ((BOOL(__thiscall*)(CVehicleSA*, unsigned short))SAMP_ADDROF(0xB1A70))(this, nId);
}

BOOL SAMP::CVehicleSA::RemoveComponent(unsigned short nId) {
	return ((BOOL(__thiscall*)(CVehicleSA*, unsigned short))SAMP_ADDROF(0xB1B50))(this, nId);
}

void SAMP::CVehicleSA::SetPaintjob(NUMBER nId) {
	((void(__thiscall*)(CVehicleSA*, NUMBER))SAMP_ADDROF(0xB1B90))(this, nId);
}

BOOL SAMP::CVehicleSA::DoesExist() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1BE0))(this);
}

BOOL SAMP::CVehicleSA::HasSiren() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1DD0))(this);
}

char SAMP::CVehicleSA::GetMaxPassengers() {
	return ((char(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB1DE0))(this);
}

void SAMP::CVehicleSA::SetWindowOpenFlag(NUMBER nDoorId) {
	((void(__thiscall*)(CVehicleSA*, NUMBER))SAMP_ADDROF(0xB1E10))(this, nDoorId);
}

void SAMP::CVehicleSA::ClearWindowOpenFlag(NUMBER nDoorId) {
	((void(__thiscall*)(CVehicleSA*, NUMBER))SAMP_ADDROF(0xB1E40))(this, nDoorId);
}

void SAMP::CVehicleSA::EnableEngine(BOOL bEnable) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB2510))(this, bEnable);
}

#elif defined(SAMP_R3)

SAMP::CVehicleSA::CVehicleSA(int nModel, CVectorSA position, float fRotation, BOOL bKeepModelLoaded, BOOL bHasSiren) {
	((void(__thiscall*)(CVehicleSA*, int, CVectorSA, float, BOOL, BOOL))SAMP_ADDROF(0xB7B30))(this, nModel, position, fRotation, bKeepModelLoaded, bHasSiren);
}

void SAMP::CVehicleSA::ChangeInterior(int nId) {
	((void(__thiscall*)(CVehicleSA*, int))SAMP_ADDROF(0xB6800))(this, nId);
}

void SAMP::CVehicleSA::ResetPointers() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6830))(this);
}

BOOL SAMP::CVehicleSA::HasDriver() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6850))(this);
}

BOOL SAMP::CVehicleSA::IsOccupied() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB68A0))(this);
}

void SAMP::CVehicleSA::SetInvulnerable(BOOL bInv) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB6900))(this, bInv);
}

void SAMP::CVehicleSA::SetLocked(BOOL bLock) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB69A0))(this, bLock);
}

float SAMP::CVehicleSA::GetHealth() {
	return ((float(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6A10))(this);
}

void SAMP::CVehicleSA::SetHealth(float fValue) {
	((void(__thiscall*)(CVehicleSA*, float))SAMP_ADDROF(0xB6A30))(this, fValue);
}

void SAMP::CVehicleSA::SetColor(NUMBER nPrimary, NUMBER nSecondary) {
	((void(__thiscall*)(CVehicleSA*, NUMBER, NUMBER))SAMP_ADDROF(0xB6A50))(this, nPrimary, nSecondary);
}

void SAMP::CVehicleSA::UpdateColor() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6AA0))(this);
}

int SAMP::CVehicleSA::GetSubtype() {
	return ((int(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6B00))(this);
}

BOOL SAMP::CVehicleSA::IsSunk() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6B20))(this);
}

BOOL SAMP::CVehicleSA::IsWrecked() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6B40))(this);
}

BOOL SAMP::CVehicleSA::DriverIsPlayerPed() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6B60))(this);
}

BOOL SAMP::CVehicleSA::HasPlayerPed() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6B90))(this);
}

BOOL SAMP::CVehicleSA::IsTrainPart() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6BD0))(this);
}

BOOL SAMP::CVehicleSA::HasTurret() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6C10))(this);
}

void SAMP::CVehicleSA::EnableSiren(bool bEnable) {
	((void(__thiscall*)(CVehicleSA*, bool))SAMP_ADDROF(0xB6CB0))(this, bEnable);
}

BOOL SAMP::CVehicleSA::SirenEnabled() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6CD0))(this);
}

void SAMP::CVehicleSA::SetLandingGearState(BOOL bHide) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB6D10))(this, bHide);
}

BOOL SAMP::CVehicleSA::GetLandingGearState() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6DA0))(this);
}

void SAMP::CVehicleSA::SetHydraThrusters(int nDirection) {
	((void(__thiscall*)(CVehicleSA*, int))SAMP_ADDROF(0xB6E10))(this, nDirection);
}

int SAMP::CVehicleSA::GetHydraThrusters() {
	return ((int(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6E30))(this);
}

void SAMP::CVehicleSA::ProcessMarkers() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6E50))(this);
}

void SAMP::CVehicleSA::Lock(BOOL bLock) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB6FB0))(this, bLock);
}

BOOL SAMP::CVehicleSA::UpdateLastDrivenTime() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB6FE0))(this);
}

float SAMP::CVehicleSA::GetTrainSpeed() {
	return ((float(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7050))(this);
}

void SAMP::CVehicleSA::SetTrainSpeed(float fValue) {
	((void(__thiscall*)(CVehicleSA*, float))SAMP_ADDROF(0xB7070))(this, fValue);
}

void SAMP::CVehicleSA::SetTires(char nState) {
	((void(__thiscall*)(CVehicleSA*, char))SAMP_ADDROF(0xB70B0))(this, nState);
}

char SAMP::CVehicleSA::GetTires() {
	return ((char(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB71A0))(this);
}

void SAMP::CVehicleSA::UpdateDamage(int nPanels, int nDoors, char nLights) {
	((void(__thiscall*)(CVehicleSA*, int, int, char))SAMP_ADDROF(0xB7230))(this, nPanels, nDoors, nLights);
}

int SAMP::CVehicleSA::GetPanelsDamage() {
	return ((int(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB72F0))(this);
}

int SAMP::CVehicleSA::GetDoorsDamage() {
	return ((int(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7320))(this);
}

char SAMP::CVehicleSA::GetLightsDamage() {
	return ((char(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7350))(this);
}

void SAMP::CVehicleSA::AttachTrailer() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7380))(this);
}

void SAMP::CVehicleSA::DetachTrailer() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB73A0))(this);
}

void SAMP::CVehicleSA::SetTrailer(CVehicleSA* pVehicle) {
	((void(__thiscall*)(CVehicleSA*, CVehicleSA*))SAMP_ADDROF(0xB73F0))(this, pVehicle);
}

SAMP::CVehicleSA* SAMP::CVehicleSA::GetTrailer() {
	return ((CVehicleSA * (__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7400))(this);
}

SAMP::CVehicleSA* SAMP::CVehicleSA::GetTractor() {
	return ((CVehicleSA * (__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7460))(this);
}

BOOL SAMP::CVehicleSA::IsTrailer() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB74E0))(this);
}

BOOL SAMP::CVehicleSA::IsTowtruck() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7540))(this);
}

BOOL SAMP::CVehicleSA::IsRC() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7570))(this);
}

void SAMP::CVehicleSA::EnableLights(bool bEnable) {
	((void(__thiscall*)(CVehicleSA*, bool))SAMP_ADDROF(0xB75C0))(this, bEnable);
}

void SAMP::CVehicleSA::RemovePassengers() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7650))(this);
}

BOOL SAMP::CVehicleSA::AddComponent(unsigned short nId) {
	return ((BOOL(__thiscall*)(CVehicleSA*, unsigned short))SAMP_ADDROF(0xB7730))(this, nId);
}

BOOL SAMP::CVehicleSA::RemoveComponent(unsigned short nId) {
	return ((BOOL(__thiscall*)(CVehicleSA*, unsigned short))SAMP_ADDROF(0xB7810))(this, nId);
}

void SAMP::CVehicleSA::SetPaintjob(NUMBER nId) {
	((void(__thiscall*)(CVehicleSA*, NUMBER))SAMP_ADDROF(0xB7850))(this, nId);
}

BOOL SAMP::CVehicleSA::DoesExist() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB78A0))(this);
}

void SAMP::CVehicleSA::SetLicensePlateText(const char* szText) {
	((void(__thiscall*)(CVehicleSA*, const char*))SAMP_ADDROF(0xB78B0))(this, szText);
}

void SAMP::CVehicleSA::SetRotation(float fValue) {
	((void(__thiscall*)(CVehicleSA*, float))SAMP_ADDROF(0xB78D0))(this, fValue);
}

void SAMP::CVehicleSA::ConstructLicensePlate() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7900))(this);
}

void SAMP::CVehicleSA::ShutdownLicensePlate() {
	((void(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7950))(this);
}

BOOL SAMP::CVehicleSA::HasSiren() {
	return ((BOOL(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7A90))(this);
}

char SAMP::CVehicleSA::GetMaxPassengers() {
	return ((char(__thiscall*)(CVehicleSA*))SAMP_ADDROF(0xB7AA0))(this);
}

void SAMP::CVehicleSA::SetWindowOpenFlag(NUMBER nDoorId) {
	((void(__thiscall*)(CVehicleSA*, NUMBER))SAMP_ADDROF(0xB7AD0))(this, nDoorId);
}

void SAMP::CVehicleSA::ClearWindowOpenFlag(NUMBER nDoorId) {
	((void(__thiscall*)(CVehicleSA*, NUMBER))SAMP_ADDROF(0xB7B00))(this, nDoorId);
}

void SAMP::CVehicleSA::EnableEngine(BOOL bEnable) {
	((void(__thiscall*)(CVehicleSA*, BOOL))SAMP_ADDROF(0xB81D0))(this, bEnable);
}

#endif
