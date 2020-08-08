/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CGameSA.h"

#if defined(SAMP_R1)

SAMP::CGameSA*& SAMP::pGame() { return *(SAMP::CGameSA**)SAMP_ADDROF(0x21A10C); }
char*& SAMP::CGameSA::szGameTextMessage() { return *(char**)SAMP_ADDROF(0x13BEFC); }
bool* SAMP::CGameSA::aUsedPlayerSlots() { return (bool*)SAMP_ADDROF(0x13BF08); }

BOOL SAMP::CGameSA::DoesHeadMoves() {
	return ((BOOL(__thiscall*)(CGameSA*))SAMP_ADDROF(0x2D30))(this);
}

void* SAMP::CGameSA::GetWindowHandle() {
	return ((void* (__thiscall*)(CGameSA*))SAMP_ADDROF(0x2D00))(this);
}

SAMP::CAudio* SAMP::CGameSA::GetAudio() {
	return ((CAudio * (__thiscall*)(CGameSA*))SAMP_ADDROF(0x2D10))(this);
}

SAMP::CCameraSA* SAMP::CGameSA::GetCamera() {
	return ((CCameraSA * (__thiscall*)(CGameSA*))SAMP_ADDROF(0x2D20))(this);
}

SAMP::CPedSA* SAMP::CGameSA::GetPlayerPed() {
	return ((CPedSA * (__thiscall*)(CGameSA*))SAMP_ADDROF(0x1010))(this);
}

SAMP::CGameSA::CGameSA() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9B5C0))(this);
}

void SAMP::CGameSA::SetCursorMode(int nMode, BOOL bImmediatelyHideCursor) {
	((void(__thiscall*)(CGameSA*, int, BOOL))SAMP_ADDROF(0x9BD30))(this, nMode, bImmediatelyHideCursor);
}

void SAMP::CGameSA::ProcessInputEnabling() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9BC10))(this);
}

void SAMP::CGameSA::LetTimeGo(bool bLet) {
	((void(__thiscall*)(CGameSA*, bool))SAMP_ADDROF(0x9C0F0))(this, bLet);
}

void SAMP::CGameSA::SetWorldWeather(char nWeather) {
	((void(__thiscall*)(CGameSA*, char))SAMP_ADDROF(0x9C130))(this, nWeather);
}

void SAMP::CGameSA::SetFrameLimiter(int nValue) {
	((void(__thiscall*)(CGameSA*, int))SAMP_ADDROF(0x9C190))(this, nValue);
}

char SAMP::CGameSA::GetCurrentInterior() {
	return ((char(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9C490))(this);
}

void SAMP::CGameSA::EnableClock(bool bEnable) {
	((void(__thiscall*)(CGameSA*, bool))SAMP_ADDROF(0x9CA00))(this, bEnable);
}

void SAMP::CGameSA::SetRequiredVehicleModels(unsigned char* pModelCount) {
	((void(__thiscall*)(CGameSA*, unsigned char*))SAMP_ADDROF(0x9CBB0))(this, pModelCount);
}

void SAMP::CGameSA::CreateRacingCheckpoint() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9D400))(this);
}

void SAMP::CGameSA::ProcessCheckpoints() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9D480))(this);
}

float SAMP::CGameSA::FindGroundZ(CVectorSA point) {
	return ((float(__thiscall*)(CGameSA*, CVectorSA))SAMP_ADDROF(0x9BA40))(this, point);
}

int SAMP::CGameSA::IsStarted() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9BF70))(this);
}

void SAMP::CGameSA::RequestModel(int nModel, int nLoadingStream) {
	((void(__thiscall*)(CGameSA*, int, int))SAMP_ADDROF(0x9BF80))(this, nModel, nLoadingStream);
}

void SAMP::CGameSA::LoadRequestedModels() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9BFA0))(this);
}

BOOL SAMP::CGameSA::IsModelAvailable(int nModel) {
	return ((int(__thiscall*)(CGameSA*, int))SAMP_ADDROF(0x9BFB0))(this, nModel);
}

void SAMP::CGameSA::ReleaseModel(int nModel, bool bGameFunc) {
	((void(__thiscall*)(CGameSA*, int, bool))SAMP_ADDROF(0x9BFD0))(this, nModel, bGameFunc);
}

void SAMP::CGameSA::SetWorldTime(char nHour, char nMinute) {
	((void(__thiscall*)(CGameSA*, char, char))SAMP_ADDROF(0x9C0A0))(this, nHour, nMinute);
}

void SAMP::CGameSA::GetWorldTime(char* pHour, char* pMinute) {
	((void(__thiscall*)(CGameSA*, char*, char*))SAMP_ADDROF(0x9C0D0))(this, pHour, pMinute);
}

void SAMP::CGameSA::SetMaxStats() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9C1C0))(this);
}

void SAMP::CGameSA::RefreshRenderer(float fX, float fY) {
	((void(__thiscall*)(CGameSA*, float, float))SAMP_ADDROF(0x9C200))(this, fX, fY);
}

void SAMP::CGameSA::RequestAnimation(const char* szFile) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0x9C230))(this, szFile);
}

int SAMP::CGameSA::IsAnimationLoaded(const char* szFile) {
	return ((int(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0x9C250))(this, szFile);
}

void SAMP::CGameSA::DisplayGameText(const char* szText, int nTime, int nSize) {
	((void(__thiscall*)(CGameSA*, const char*, int, int))SAMP_ADDROF(0x9C2C0))(this, szText, nTime, nSize);
}

SAMP::GTAREF SAMP::CGameSA::CreateMarker(int nIcon, CVectorSA position, int nColor, int nType) {
	return ((unsigned long(__thiscall*)(CGameSA*, int, CVectorSA, D3DCOLOR, int))SAMP_ADDROF(0x9C340))(this, nIcon, position, nColor, nType);
}

void SAMP::CGameSA::IncreasePlayerMoney(int nInc) {
	((void(__thiscall*)(CGameSA*, int))SAMP_ADDROF(0x9C520))(this, nInc);
}

int SAMP::CGameSA::GetPlayerMoney() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9C540))(this);
}

const char* SAMP::CGameSA::GetWeaponName(int nWeapon) {
	return ((const char* (__thiscall*)(CGameSA*, int))SAMP_ADDROF(0x9C550))(this, nWeapon);
}

SAMP::GTAREF SAMP::CGameSA::CreateWeaponPickup(int nModel, int nAmmo, CVectorSA position) {
	return ((unsigned long(__thiscall*)(CGameSA*, int, int, CVectorSA))SAMP_ADDROF(0x9C870))(this, nModel, nAmmo, position);
}

IDirect3DDevice9* SAMP::CGameSA::GetDevice() {
	return ((IDirect3DDevice9 * (__thiscall*)(CGameSA*))SAMP_ADDROF(0x9C910))(this);
}

CWeaponInfo* SAMP::CGameSA::GetWeaponInfo(int nWeapon, int nSkill) {
	return ((CWeaponInfo * (__thiscall*)(CGameSA*, int, int))SAMP_ADDROF(0x9C980))(this, nWeapon, nSkill);
}

void SAMP::CGameSA::SetWorldGravity(float fValue) {
	((void(__thiscall*)(CGameSA*, float))SAMP_ADDROF(0x9C9A0))(this, fValue);
}

void SAMP::CGameSA::SetWantedLevel(char nLevel) {
	((void(__thiscall*)(CGameSA*, char))SAMP_ADDROF(0x9C9C0))(this, nLevel);
}

void SAMP::CGameSA::EnableZoneDisplaying(bool bEnable) {
	((void(__thiscall*)(CGameSA*, bool))SAMP_ADDROF(0x9CAC0))(this, bEnable);
}

void SAMP::CGameSA::EnableStuntBonus(bool bEnable) {
	((void(__thiscall*)(CGameSA*, bool))SAMP_ADDROF(0x9CAE0))(this, bEnable);
}

void SAMP::CGameSA::EnableHUD(BOOL bEnable) {
	((void(__thiscall*)(CGameSA*, BOOL))SAMP_ADDROF(0x9D310))(this, bEnable);
}

void SAMP::CGameSA::ResetMoney() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9D620))(this);
}

void SAMP::CGameSA::DrawGangZone(float* pPos, char nColor) {
	((void(__thiscall*)(CGameSA*, float*, D3DCOLOR))SAMP_ADDROF(0x9C9E0))(this, pPos, nColor);
}

void SAMP::CGameSA::DeleteMarker(GTAREF handle) {
	((void(__thiscall*)(CGameSA*, GTAREF))SAMP_ADDROF(0x9C470))(this, handle);
}

void SAMP::CGameSA::LoadAnimationsAndModels() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9CE00))(this);
}

int SAMP::CGameSA::IsMenuVisible() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9BF60))(this);
}

BOOL SAMP::CGameSA::UsingGamepad() {
	return ((BOOL(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9D120))(this);
}

void SAMP::CGameSA::SetCheckpoint(CVectorSA* pPosition, CVectorSA* pSize) {
	((void(__thiscall*)(CGameSA*, CVectorSA*, CVectorSA*))SAMP_ADDROF(0x9D340))(this, pPosition, pSize);
}

void SAMP::CGameSA::SetRacingCheckpoint(int nType, CVectorSA* pPosition, CVectorSA* pNextPosition, float fSize) {
	((void(__thiscall*)(CGameSA*, int, CVectorSA*, CVectorSA*, float))SAMP_ADDROF(0x9D660))(this, nType, pPosition, pNextPosition, fSize);
}

void SAMP::CGameSA::InitGame() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9BED0))(this);
}

void SAMP::CGameSA::StartGame() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9BF20))(this);
}

void SAMP::CGameSA::DeleteRacingCheckpoint() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9C310))(this);
}

SAMP::CPedSA* SAMP::CGameSA::CreatePed(int nModel, CVectorSA position, float fRotation, int a6, int a7) {
	return ((CPedSA * (__thiscall*)(CGameSA*, int, CVectorSA, float, int, int))SAMP_ADDROF(0x9B750))(this, nModel, position, fRotation, a6, a7);
}

SAMP::CVehicleSA* SAMP::CGameSA::CreateVehicle(int nModel, CVectorSA position, float fRotation, BOOL bHasSiren) {
	return ((CVehicleSA * (__thiscall*)(CGameSA*, int, CVectorSA, float, int))SAMP_ADDROF(0x9B890))(this, nModel, position, fRotation, bHasSiren);
}

SAMP::CObjectSA* SAMP::CGameSA::CreateObject(int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance) {
	return ((CObjectSA * (__thiscall*)(CGameSA*, int, CVectorSA, CVectorSA, float))SAMP_ADDROF(0x9B970))(this, nModel, position, rotation, fDrawDistance);
}

void SAMP::CGameSA::DisableTrainTraffic() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9C1F0))(this);
}

void SAMP::CGameSA::ReleaseAnimation(const char* szFile) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0x9C270))(this, szFile);
}

void SAMP::CGameSA::UpdateFarClippingPlane() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9C4B0))(this);
}

void SAMP::CGameSA::CreatePickup(int nModel, int nType, CVectorSA position, GTAREF* handle) {
	((void(__thiscall*)(CGameSA*, int, int, CVectorSA, GTAREF*))SAMP_ADDROF(0x9C7A0))(this, nModel, nType, position, handle);
}

void SAMP::CGameSA::Restart() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9C950))(this);
}

void SAMP::CGameSA::SetNumberOfIntroTextLinesThisFrame(unsigned short nValue) {
	((void(__thiscall*)(CGameSA*, unsigned short))SAMP_ADDROF(0x9C9D0))(this, nValue);
}

void SAMP::CGameSA::LoadScene(const char* szFile) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0x9CB50))(this, szFile);
}

int SAMP::CGameSA::GetUsedMemory() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9CB70))(this);
}

int SAMP::CGameSA::GetStreamingMemory() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9CB80))(this);
}

int SAMP::CGameSA::GetTimer() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9CCD0))(this);
}

void SAMP::CGameSA::LoadCollisionFile(const char* szFile) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0x9D0E0))(this, szFile);
}

void SAMP::CGameSA::LoadCullZone(const char* szLine) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0x9D100))(this, szLine);
}

void SAMP::CGameSA::DisableAutoAiming() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9D130))(this);
}

void SAMP::CGameSA::EnableRadar(BOOL bEnable) {
	((void(__thiscall*)(CGameSA*, BOOL))SAMP_ADDROF(0x9C2A0))(this, bEnable);
}

void SAMP::CGameSA::Sleep(int elapsed, int fpsLimit) {
	((void(__thiscall*)(CGameSA*, int, int))SAMP_ADDROF(0x9B6D0))(this, elapsed, fpsLimit);
}

BOOL SAMP::CGameSA::RemovePed(CPedSA* pPed) {
	return ((BOOL(__thiscall*)(CGameSA*, CPedSA*))SAMP_ADDROF(0x9B850))(this, pPed);
}

void SAMP::CGameSA::ProcessFrameLimiter() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9D170))(this);
}

#elif defined(SAMP_R3)

char*& SAMP::CGameSA::szGameTextMessage() { return *(char**)SAMP_ADDROF(0x150084); }
bool* SAMP::CGameSA::aUsedPlayerSlots() { return (bool*)SAMP_ADDROF(0x150090); }
SAMP::CGameSA*& SAMP::pGame() { return *(SAMP::CGameSA**)SAMP_ADDROF(0x26E8F4); }

SAMP::CGameSA::CGameSA() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9F870))(this);
}

SAMP::CPedSA* SAMP::CGameSA::GetPlayerPed() {
	return ((SAMP::CPedSA * (__thiscall*)(CGameSA*))SAMP_ADDROF(0x1010))(this);
}

float SAMP::CGameSA::FindGroundZ(CVectorSA vPoint) {
	return ((float(__thiscall*)(CGameSA*, CVectorSA))SAMP_ADDROF(0x9FCF0))(this, vPoint);
}

void SAMP::CGameSA::SetCursorMode(int nMode, BOOL bImmediatelyHideCursor) {
	((void(__thiscall*)(CGameSA*, int, BOOL))SAMP_ADDROF(0x9FFE0))(this, nMode, bImmediatelyHideCursor);
}

void SAMP::CGameSA::InitGame() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0180))(this);
}

void SAMP::CGameSA::StartGame() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA01D0))(this);
}

BOOL SAMP::CGameSA::IsMenuVisible() {
	return ((BOOL(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0210))(this);
}

BOOL SAMP::CGameSA::IsStarted() {
	return ((BOOL(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0220))(this);
}

void SAMP::CGameSA::RequestModel(int nModel, int nLoadingStream) {
	((void(__thiscall*)(CGameSA*, int, int))SAMP_ADDROF(0xA0230))(this, nModel, nLoadingStream);
}

void SAMP::CGameSA::LoadRequestedModels() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0250))(this);
}

BOOL SAMP::CGameSA::IsModelAvailable(int nModel) {
	return ((BOOL(__thiscall*)(CGameSA*, int))SAMP_ADDROF(0xA0260))(this, nModel);
}

void SAMP::CGameSA::ReleaseModel(int nModel, bool bGameFunc) {
	((void(__thiscall*)(CGameSA*, int, bool))SAMP_ADDROF(0xA0290))(this, nModel, bGameFunc);
}

void SAMP::CGameSA::SetWorldTime(char nHour, char nMinute) {
	((void(__thiscall*)(CGameSA*, char, char))SAMP_ADDROF(0xA03A0))(this, nHour, nMinute);
}

void SAMP::CGameSA::GetWorldTime(char* nHour, char* nMinute) {
	((void(__thiscall*)(CGameSA*, char*, char*))SAMP_ADDROF(0xA03D0))(this, nHour, nMinute);
}

void SAMP::CGameSA::LetTimeGo(bool bLet) {
	((void(__thiscall*)(CGameSA*, bool))SAMP_ADDROF(0xA03F0))(this, bLet);
}

void SAMP::CGameSA::SetWorldWeather(char nWeather) {
	((void(__thiscall*)(CGameSA*, char))SAMP_ADDROF(0xA0430))(this, nWeather);
}

void SAMP::CGameSA::SetFrameLimiter(int nValue) {
	((void(__thiscall*)(CGameSA*, int))SAMP_ADDROF(0xA04A0))(this, nValue);
}

void SAMP::CGameSA::SetMaxStats() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA04D0))(this);
}

void SAMP::CGameSA::DisableTrainTraffic() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0500))(this);
}

void SAMP::CGameSA::RefreshRenderer(float fX, float fY) {
	((void(__thiscall*)(CGameSA*, float, float))SAMP_ADDROF(0xA0510))(this, fX, fY);
}

void SAMP::CGameSA::RequestAnimation(const char* szFile) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0xA0540))(this, szFile);
}

BOOL SAMP::CGameSA::IsAnimationLoaded(const char* szFile) {
	return ((BOOL(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0xA0560))(this, szFile);
}

void SAMP::CGameSA::ReleaseAnimation(const char* szFile) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0xA0580))(this, szFile);
}

void SAMP::CGameSA::DisplayGameText(const char* szText, int nTime, int nSize) {
	((void(__thiscall*)(CGameSA*, const char*, int, int))SAMP_ADDROF(0xA05D0))(this, szText, nTime, nSize);
}

void SAMP::CGameSA::DeleteRacingCheckpoint() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0630))(this);
}

SAMP::GTAREF SAMP::CGameSA::CreateMarker(int nIcon, CVectorSA vPosition, int nColor, int nType) {
	return ((GTAREF(__thiscall*)(CGameSA*, int, CVectorSA, int, int))SAMP_ADDROF(0xA0660))(this, nIcon, vPosition, nColor, nType);
}

void SAMP::CGameSA::DeleteMarker(GTAREF handle) {
	((void(__thiscall*)(CGameSA*, GTAREF))SAMP_ADDROF(0xA0790))(this, handle);
}

char SAMP::CGameSA::GetCurrentInterior() {
	return ((char(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA07B0))(this);
}

void SAMP::CGameSA::UpdateFarClippingPlane() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA07D0))(this);
}

void SAMP::CGameSA::IncreasePlayerMoney(int nInc) {
	((void(__thiscall*)(CGameSA*, int))SAMP_ADDROF(0xA0840))(this, nInc);
}

int SAMP::CGameSA::GetPlayerMoney() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0860))(this);
}

const char* SAMP::CGameSA::GetWeaponName(int nWeapon) {
	return ((const char* (__thiscall*)(CGameSA*, int))SAMP_ADDROF(0xA0870))(this, nWeapon);
}

void SAMP::CGameSA::CreatePickup(int nModel, int nType, CVectorSA vPosition, GTAREF* handle) {
	((void(__thiscall*)(CGameSA*, int, int, CVectorSA, GTAREF*))SAMP_ADDROF(0xA0AC0))(this, nModel, nType, vPosition, handle);
}

SAMP::GTAREF SAMP::CGameSA::CreateWeaponPickup(int nModel, int nAmmo, CVectorSA vPosition) {
	return ((GTAREF(__thiscall*)(CGameSA*, int, int, CVectorSA))SAMP_ADDROF(0xA0BA0))(this, nModel, nAmmo, vPosition);
}

IDirect3DDevice9* SAMP::CGameSA::GetDevice() {
	return ((IDirect3DDevice9 * (__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0C40))(this);
}

void SAMP::CGameSA::Restart() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0C80))(this);
}

CWeaponInfo* SAMP::CGameSA::GetWeaponInfo(int nWeapon, int nSkill) {
	return ((CWeaponInfo * (__thiscall*)(CGameSA*, int, int))SAMP_ADDROF(0xA0CB0))(this, nWeapon, nSkill);
}

void SAMP::CGameSA::SetWorldGravity(float fValue) {
	((void(__thiscall*)(CGameSA*, float))SAMP_ADDROF(0xA0CD0))(this, fValue);
}

void SAMP::CGameSA::SetWantedLevel(char nLevel) {
	((void(__thiscall*)(CGameSA*, char))SAMP_ADDROF(0xA0CF0))(this, nLevel);
}

void SAMP::CGameSA::SetNumberOfIntroTextLinesThisFrame(unsigned short nValue) {
	((void(__thiscall*)(CGameSA*, unsigned short))SAMP_ADDROF(0xA0D00))(this, nValue);
}

void SAMP::CGameSA::DrawGangZone(float* pPos, char nColor) {
	((void(__thiscall*)(CGameSA*, float*, char))SAMP_ADDROF(0xA0D10))(this, pPos, nColor);
}

void SAMP::CGameSA::EnableZoneDisplaying(bool bEnable) {
	((void(__thiscall*)(CGameSA*, bool))SAMP_ADDROF(0xA0DF0))(this, bEnable);
}

void SAMP::CGameSA::EnableStuntBonus(bool bEnable) {
	((void(__thiscall*)(CGameSA*, bool))SAMP_ADDROF(0xA0E10))(this, bEnable);
}

void SAMP::CGameSA::LoadScene(const char* szFile) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0xA0E80))(this, szFile);
}

int SAMP::CGameSA::GetUsedMemory() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0EA0))(this);
}

int SAMP::CGameSA::GetStreamingMemory() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA0EB0))(this);
}

void SAMP::CGameSA::SetRequiredVehicleModels(unsigned char* pModelCount) {
	((void(__thiscall*)(CGameSA*, unsigned char*))SAMP_ADDROF(0xA0EE0))(this, pModelCount);
}

int SAMP::CGameSA::GetTimer() {
	return ((int(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA1040))(this);
}

void SAMP::CGameSA::LoadAnimationsAndModels() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA1170))(this);
}

void SAMP::CGameSA::LoadCollisionFile(const char* szFile) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0xA1450))(this, szFile);
}

void SAMP::CGameSA::LoadCullZone(const char* szLine) {
	((void(__thiscall*)(CGameSA*, const char*))SAMP_ADDROF(0xA1470))(this, szLine);
}

BOOL SAMP::CGameSA::UsingGamepad() {
	return ((BOOL(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA1490))(this);
}

void SAMP::CGameSA::DisableAutoAiming() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA14A0))(this);
}

void SAMP::CGameSA::EnableHUD(BOOL bEnable) {
	((void(__thiscall*)(CGameSA*, BOOL))SAMP_ADDROF(0xA1680))(this, bEnable);
}

void SAMP::CGameSA::SetCheckpoint(CVectorSA* pPos, CVectorSA* pSize) {
	((void(__thiscall*)(CGameSA*, CVectorSA*, CVectorSA*))SAMP_ADDROF(0xA16B0))(this, pPos, pSize);
}

void SAMP::CGameSA::CreateRacingCheckpoint() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA1770))(this);
}

void SAMP::CGameSA::ProcessCheckpoints() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA17F0))(this);
}

void SAMP::CGameSA::ResetMoney() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA1990))(this);
}

void SAMP::CGameSA::SetRacingCheckpoint(int nType, CVectorSA* pCurrentPos, CVectorSA* pNextPos, float fSize) {
	((void(__thiscall*)(CGameSA*, int, CVectorSA*, CVectorSA*, float))SAMP_ADDROF(0xA19D0))(this, nType, pCurrentPos, pNextPos, fSize);
}

void SAMP::CGameSA::EnableRadar(BOOL bEnable) {
	((void(__thiscall*)(CGameSA*, BOOL))SAMP_ADDROF(0xA05B0))(this, bEnable);
}

void* SAMP::CGameSA::GetWindowHandle() {
	return ((void* (__thiscall*)(CGameSA*))SAMP_ADDROF(0x2CF0))(this);
}

SAMP::CAudio* SAMP::CGameSA::GetAudio() {
	return ((CAudio * (__thiscall*)(CGameSA*))SAMP_ADDROF(0x2D00))(this);
}

SAMP::CCameraSA* SAMP::CGameSA::GetCamera() {
	return ((CCameraSA * (__thiscall*)(CGameSA*))SAMP_ADDROF(0x2D10))(this);
}

BOOL SAMP::CGameSA::DoesHeadMoves() {
	return ((BOOL(__thiscall*)(CGameSA*))SAMP_ADDROF(0x2D20))(this);
}

void SAMP::CGameSA::EnableClock(bool bEnable) {
	((void(__thiscall*)(CGameSA*, bool))SAMP_ADDROF(0xA0D30))(this, bEnable);
}

void SAMP::CGameSA::Sleep(int elapsed, int fpsLimit) {
	((void(__thiscall*)(CGameSA*, int, int))SAMP_ADDROF(0x9F980))(this, elapsed, fpsLimit);
}

SAMP::CPedSA* SAMP::CGameSA::CreatePed(int nModel, CVectorSA position, float fRotation, int a6, int a7) {
	return ((CPedSA * (__thiscall*)(CGameSA*, int, CVectorSA, float, int, int))SAMP_ADDROF(0x9FA00))(this, nModel, position, fRotation, a6, a7);
}

BOOL SAMP::CGameSA::RemovePed(CPedSA* pPed) {
	return ((BOOL(__thiscall*)(CGameSA*, CPedSA*))SAMP_ADDROF(0x9FB00))(this, pPed);
}

SAMP::CVehicleSA* SAMP::CGameSA::CreateVehicle(int nModel, CVectorSA position, float fRotation, BOOL bHasSiren) {
	return ((CVehicleSA * (__thiscall*)(CGameSA*, int, CVectorSA, float, BOOL))SAMP_ADDROF(0x9FB40))(this, nModel, position, fRotation, bHasSiren);
}

SAMP::CObjectSA* SAMP::CGameSA::CreateObject(int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance, char a11, char a12) {
	return ((CObjectSA * (__thiscall*)(CGameSA*, int, CVectorSA, CVectorSA, float, char, char))SAMP_ADDROF(0x9FC20))(this, nModel, position, rotation, fDrawDistance, a11, a12);
}

void SAMP::CGameSA::ProcessInputEnabling() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0x9FEC0))(this);
}

void SAMP::CGameSA::ProcessFrameLimiter() {
	((void(__thiscall*)(CGameSA*))SAMP_ADDROF(0xA14E0))(this);
}

#endif
