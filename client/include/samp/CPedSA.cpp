/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CPedSA.h"

#if defined(SAMP_R1)

SAMP::CPedSA::CPedSA() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6330))(this);
}

SAMP::CPedSA::CPedSA(unsigned char nPlayerNumber, int nModel, CVectorSA position, float fRotation) {
	((void(__thiscall*)(CPedSA*, unsigned char, int, CVectorSA, float))SAMP_ADDROF(0xAB580))(this, nPlayerNumber, nModel, position, fRotation);
}

void SAMP::CPedSA::ResetPointers() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6470))(this);
}

void SAMP::CPedSA::SetInitialState() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6490))(this);
}

SAMP::AimStuff::Aim* SAMP::CPedSA::GetAim() {
	return ((AimStuff::Aim * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xA64E0))(this);
}

void SAMP::CPedSA::SetAim(const AimStuff::Aim* pAim) {
	((void(__thiscall*)(CPedSA*, const AimStuff::Aim*))SAMP_ADDROF(0xA64F0))(this, pAim);
}

char SAMP::CPedSA::GetCurrentWeapon() {
	return ((char(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6510))(this);
}

SAMP::GTAREF SAMP::CPedSA::GetVehicleRef() {
	return ((GTAREF(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6550))(this);
}

void SAMP::CPedSA::DeleteArrow() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6570))(this);
}

BOOL SAMP::CPedSA::IsOnScreen() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA65A0))(this);
}

void SAMP::CPedSA::SetImmunities(int BP, int FP, int EP, int CP, int MP) {
	((void(__thiscall*)(CPedSA*, int, int, int, int, int))SAMP_ADDROF(0xA65C0))(this, BP, FP, EP, CP, MP);
}

float SAMP::CPedSA::GetHealth() {
	return ((float(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6610))(this);
}

void SAMP::CPedSA::SetHealth(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xA6630))(this, fValue);
}

float SAMP::CPedSA::GetArmour() {
	return ((float(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6650))(this);
}

void SAMP::CPedSA::SetArmour(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xA6670))(this, fValue);
}

int SAMP::CPedSA::GetFlags() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6690))(this);
}

void SAMP::CPedSA::SetFlags(int nValue) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA66B0))(this, nValue);
}

BOOL SAMP::CPedSA::IsDead() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA66D0))(this);
}

char SAMP::CPedSA::GetState() {
	return ((char(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6700))(this);
}

void SAMP::CPedSA::SetState(char nValue) {
	((void(__thiscall*)(CPedSA*, char))SAMP_ADDROF(0xA6710))(this, nValue);
}

void SAMP::CPedSA::SetRotation(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xA67D0))(this, fValue);
}

void SAMP::CPedSA::ForceRotation(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xA6820))(this, fValue);
}

BOOL SAMP::CPedSA::IsPassenger() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6880))(this);
}

CVehicle* SAMP::CPedSA::GetVehicle() {
	return ((CVehicle * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xA68C0))(this);
}

void SAMP::CPedSA::ClearWeapons() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA68D0))(this);
}

void SAMP::CPedSA::SetArmedWeapon(int nWeapon, bool bGameFunc) {
	((void(__thiscall*)(CPedSA*, int, bool))SAMP_ADDROF(0xA6920))(this, nWeapon, bGameFunc);
}

float SAMP::CPedSA::GetDistanceToEntity(const CEntitySA* pEntity) {
	return ((float(__thiscall*)(CPedSA*, const CEntitySA*))SAMP_ADDROF(0xA6A60))(this, pEntity);
}

int SAMP::CPedSA::GetVehicleSeatIndex() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6AC0))(this);
}

void SAMP::CPedSA::PutIntoVehicle(GTAREF vehicle, int nSeat) {
	((void(__thiscall*)(CPedSA*, unsigned long, int))SAMP_ADDROF(0xA6B50))(this, vehicle, nSeat);
}

void SAMP::CPedSA::EnterVehicle(GTAREF vehicle, BOOL bAsPassenger) {
	((void(__thiscall*)(CPedSA*, unsigned long, bool))SAMP_ADDROF(0xA6CD0))(this, vehicle, bAsPassenger);
}

void SAMP::CPedSA::ExitVehicle() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6DA0))(this);
}

void SAMP::CPedSA::WarpFromVehicle(CVectorSA putAt) {
	((void(__thiscall*)(CPedSA*, CVectorSA))SAMP_ADDROF(0xA6E30))(this, putAt);
}

void SAMP::CPedSA::SetSpawnInfo(const CVectorSA* pPosition, float fRotation) {
	((void(__thiscall*)(CPedSA*, const CVectorSA*, float))SAMP_ADDROF(0xA7010))(this, pPosition, fRotation);
}

void SAMP::CPedSA::SetControllable(BOOL bEnable) {
	((void(__thiscall*)(CPedSA*, BOOL))SAMP_ADDROF(0xA7050))(this, bEnable);
}

char SAMP::CPedSA::GetDeathInfo(ID* pKiller) {
	return ((unsigned char(__thiscall*)(CPedSA*, ID*))SAMP_ADDROF(0xA7110))(this, pKiller);
}

void SAMP::CPedSA::HandsUp() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA73D0))(this);
}

BOOL SAMP::CPedSA::DoesHandsUp() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA7420))(this);
}

void SAMP::CPedSA::HoldObject(int nModel) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA7480))(this, nModel);
}

void SAMP::CPedSA::EnableJetpack() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA75D0))(this);
}

void SAMP::CPedSA::DisableJetpack() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA7620))(this);
}

BOOL SAMP::CPedSA::HasJetpack() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA7680))(this);
}

BOOL SAMP::CPedSA::EnablePassengerDrivebyMode() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA7850))(this);
}

void SAMP::CPedSA::Extinguish() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA79B0))(this);
}

void SAMP::CPedSA::SetWalkStyle(const char* szName) {
	((void(__thiscall*)(CPedSA*, const char*))SAMP_ADDROF(0xA7A90))(this, szName);
}

void SAMP::CPedSA::PerformAnimation(const char* szName, const char* szFile, float fFramedelta, int nLoopA, int nLockX, int nLockY, int nLockF, int nTime) {
	((void(__thiscall*)(CPedSA*, const char*, const char*, float, int, int, int, int, int))SAMP_ADDROF(0xA7AF0))(this, szName, szFile, fFramedelta, nLoopA, nLockX, nLockY, nLockF, nTime);
}

void SAMP::CPedSA::LinkToInterior(char nId, BOOL bRefresh) {
	((void(__thiscall*)(CPedSA*, char, BOOL))SAMP_ADDROF(0xA7C00))(this, nId, bRefresh);
}

void SAMP::CPedSA::DestroyParachute() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA7CA0))(this);
}

BOOL SAMP::CPedSA::IsOnGround() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8120))(this);
}

void SAMP::CPedSA::ResetDamageEntity() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8140))(this);
}

void SAMP::CPedSA::RemoveWeaponModel(int nModel) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA817C))(this, nModel);
}

float SAMP::CPedSA::GetAimZ() {
	return ((float(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA81B0))(this);
}

void SAMP::CPedSA::SetAimZ(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xA81F0))(this, fValue);
}

CEntity* SAMP::CPedSA::GetContactEntity() {
	return ((::CEntity * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8240))(this);
}

CVehicle* SAMP::CPedSA::GetContactVehicle() {
	return ((::CVehicle * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8250))(this);
}

int SAMP::CPedSA::GetStat() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8260))(this);
}

BOOL SAMP::CPedSA::PerformingCustomAnimation() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA82A0))(this);
}

void SAMP::CPedSA::StartDancing(int nStyle) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA8370))(this, nStyle);
}

void SAMP::CPedSA::StopDancing() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA83C0))(this);
}

BOOL SAMP::CPedSA::DoesDancing() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8400))(this);
}

const char* SAMP::CPedSA::GetAnimationForDance(int nMove) {
	return ((const char* (__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA8410))(this, nMove);
}

void SAMP::CPedSA::DropStuff() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA84A0))(this);
}

void SAMP::CPedSA::ProcessDrunk() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8690))(this);
}

int SAMP::CPedSA::GetDrunkLevel() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8840))(this);
}

void SAMP::CPedSA::SetDrunkLevel(int nLevel) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA8850))(this, nLevel);
}

void SAMP::CPedSA::EnableCellphone(BOOL bEnable) {
	((void(__thiscall*)(CPedSA*, BOOL))SAMP_ADDROF(0xA8910))(this, bEnable);
}

BOOL SAMP::CPedSA::UsingCellphone() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8940))(this);
}

void SAMP::CPedSA::SetFightingStyle(int nStyle) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA8970))(this, nStyle);
}

void SAMP::CPedSA::StartUrinating() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA89A0))(this);
}

void SAMP::CPedSA::StopUrinating() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8A80))(this);
}

BOOL SAMP::CPedSA::DoesUrinating() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8B00))(this);
}

void SAMP::CPedSA::GetBonePosition(int nBone, CVectorSA* pPosition) {
	((void(__thiscall*)(CPedSA*, int, CVectorSA*))SAMP_ADDROF(0xA8D70))(this, nBone, pPosition);
}

void SAMP::CPedSA::SetKeys(short controllerState, short sLeftStickX, short sLeftStickY) {
	((void(__thiscall*)(CPedSA*, short, short, short))SAMP_ADDROF(0xA9BE0))(this, controllerState, sLeftStickX, sLeftStickY);
}

void SAMP::CPedSA::CreateArrow(int nColor) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAA000))(this, nColor);
}

void SAMP::CPedSA::GiveWeapon(int nWeapon, int nAmmo) {
	((void(__thiscall*)(CPedSA*, int, int))SAMP_ADDROF(0xAA060))(this, nWeapon, nAmmo);
}

void SAMP::CPedSA::Kill() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAA8A0))(this);
}

void SAMP::CPedSA::GiveStuff(int nType) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAABA0))(this, nType);
}

short SAMP::CPedSA::GetKeys(short* pLeftStickX, short* pLeftStickY) {
	return ((short(__thiscall*)(CPedSA*, short*, short*))SAMP_ADDROF(0xA9E70))(this, pLeftStickX, pLeftStickY);
}

BOOL SAMP::CPedSA::IsInVehicle() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6730))(this);
}

void SAMP::CPedSA::Destroy() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB840))(this);
}

void SAMP::CPedSA::ApplyCommandTask(const char* szName, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11) {
	((void(__thiscall*)(CPedSA*, const char*, int, int, int, int, int, int, int, int, int))SAMP_ADDROF(0xA8870))(this, szName, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

CWeapon* SAMP::CPedSA::GetCurrentWeaponSlot() {
	return ((CWeapon * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6A00))(this);
}

BOOL SAMP::CPedSA::HasAccessory() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA96D0))(this);
}

void SAMP::CPedSA::DeleteAccessory(int nSlot) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA96F0))(this, nSlot);
}

BOOL SAMP::CPedSA::GetAccessoryState(int nSlot) {
	return ((BOOL(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA9750))(this, nSlot);
}

void SAMP::CPedSA::DeleteAllAccessories() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB380))(this);
}

void SAMP::CPedSA::AddAccessory(int nSlot, const Accessory* pInfo) {
	((void(__thiscall*)(CPedSA*, int, const Accessory*))SAMP_ADDROF(0xAB3E0))(this, nSlot, pInfo);
}

SAMP::CObjectSA* SAMP::CPedSA::GetAccessory(int nSlot) {
	return ((CObjectSA * (__thiscall*)(CPedSA*, int))SAMP_ADDROF(0x101E0))(this, nSlot);
}

void SAMP::CPedSA::SetModelIndex(int nModel) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAA820))(this, nModel);
}

CPed* SAMP::CPedSA::GetAimedPed() {
	return ((::CPed * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xA9800))(this);
}

void SAMP::CPedSA::DestroyCommandTask() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA88C0))(this);
}

int SAMP::CPedSA::GetStuff() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8530))(this);
}

BOOL SAMP::CPedSA::ApplyStuff() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8540))(this);
}

float SAMP::CPedSA::GetRotation() {
	return ((float(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6750))(this);
}

void SAMP::CPedSA::RemoveWeaponWhenEnteringVehicle() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA69D0))(this);
}

BOOL SAMP::CPedSA::CurrentWeaponHasAmmo() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA6A20))(this);
}

CEntity* SAMP::CPedSA::GetFloor() {
	return ((::CEntity * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xA72D0))(this);
}

CWeaponInfo* SAMP::CPedSA::GetCurrentWeaponInfo() {
	return ((::CWeaponInfo * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xA7380))(this);
}

unsigned short SAMP::CPedSA::GetCurrentWeaponAmmo() {
	return ((unsigned short(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA7A10))(this);
}

CWeapon* SAMP::CPedSA::GetWeaponSlot(int nWeapon) {
	return ((::CWeapon * (__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xA7A50))(this, nWeapon);
}

BOOL SAMP::CPedSA::OpenParachute() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xA7D90))(this);
}

void SAMP::CPedSA::ProcessParachuteEvent(const char* szName) {
	((void(__thiscall*)(CPedSA*, const char*))SAMP_ADDROF(0xA7EE0))(this, szName);
}

const char* SAMP::CPedSA::GetLoadedShoppingDataSubsection() {
	return ((const char* (__thiscall*)(CPedSA*))SAMP_ADDROF(0xA8B80))(this);
}

void SAMP::CPedSA::LoadShoppingDataSubsection(const char* szName) {
	((void(__thiscall*)(CPedSA*, const char*))SAMP_ADDROF(0xA8BA0))(this, szName);
}

void SAMP::CPedSA::SetWeaponAmmo(unsigned char nWeapon, unsigned short nAmmo) {
	((void(__thiscall*)(CPedSA*, unsigned char, unsigned short))SAMP_ADDROF(0xAA950))(this, nWeapon, nAmmo);
}

void SAMP::CPedSA::ProcessDancing() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAA980))(this);
}

#elif defined(SAMP_R3)

SAMP::CPedSA::CPedSA() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB1E0))(this);
}

SAMP::CPedSA::CPedSA(int nPlayerNumber, int nModel, CVectorSA position, float fRotation) {
	((void(__thiscall*)(CPedSA*, int, int, CVectorSA, float))SAMP_ADDROF(0xAB1E0))(this, nPlayerNumber, nModel, position, fRotation);
}

void SAMP::CPedSA::ResetPointers() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB320))(this);
}

void SAMP::CPedSA::SetInitialState() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB340))(this);
}

SAMP::AimStuff::Aim* SAMP::CPedSA::GetAim() {
	return ((AimStuff::Aim * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB390))(this);
}

void SAMP::CPedSA::SetAim(const AimStuff::Aim* pAim) {
	((void(__thiscall*)(CPedSA*, const AimStuff::Aim*))SAMP_ADDROF(0xAB3A0))(this, pAim);
}

char SAMP::CPedSA::GetCurrentWeapon() {
	return ((char(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB3C0))(this);
}

SAMP::GTAREF SAMP::CPedSA::GetVehicleRef() {
	return ((GTAREF(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB400))(this);
}

void SAMP::CPedSA::DeleteArrow() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB420))(this);
}

BOOL SAMP::CPedSA::IsOnScreen() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB450))(this);
}

void SAMP::CPedSA::SetImmunities(BOOL BP, BOOL FP, BOOL EP, BOOL CP, BOOL MP) {
	((void(__thiscall*)(CPedSA*, BOOL, BOOL, BOOL, BOOL, BOOL))SAMP_ADDROF(0xAB470))(this, BP, FP, EP, CP, MP);
}

float SAMP::CPedSA::GetHealth() {
	return ((float(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB4C0))(this);
}

void SAMP::CPedSA::SetHealth(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xAB4E0))(this, fValue);
}

float SAMP::CPedSA::GetArmour() {
	return ((float(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB500))(this);
}

void SAMP::CPedSA::SetArmour(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xAB520))(this, fValue);
}

int SAMP::CPedSA::GetFlags() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB540))(this);
}

void SAMP::CPedSA::SetFlags(int nValue) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAB560))(this, nValue);
}

BOOL SAMP::CPedSA::IsDead() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB580))(this);
}

char SAMP::CPedSA::GetState() {
	return ((char(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB5B0))(this);
}

void SAMP::CPedSA::SetState(char nValue) {
	((void(__thiscall*)(CPedSA*, char))SAMP_ADDROF(0xAB5C0))(this, nValue);
}

float SAMP::CPedSA::GetRotation() {
	return ((float(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB600))(this);
}

void SAMP::CPedSA::ForceRotation(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xAB680))(this, fValue);
}

void SAMP::CPedSA::SetRotation(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xAB6D0))(this, fValue);
}

BOOL SAMP::CPedSA::IsPassenger() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB730))(this);
}

CVehicle* SAMP::CPedSA::GetVehicle() {
	return ((::CVehicle * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB770))(this);
}

void SAMP::CPedSA::ClearWeapons() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB780))(this);
}

void SAMP::CPedSA::SetArmedWeapon(int nWeapon, bool bGameFunc) {
	((void(__thiscall*)(CPedSA*, int, bool))SAMP_ADDROF(0xAB7D0))(this, nWeapon, bGameFunc);
}

void SAMP::CPedSA::RemoveWeaponWhenEnteringVehicle() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB880))(this);
}

CWeapon* SAMP::CPedSA::GetCurrentWeaponSlot() {
	return ((::CWeapon * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB8B0))(this);
}

BOOL SAMP::CPedSA::CurrentWeaponHasAmmo() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB8D0))(this);
}

float SAMP::CPedSA::GetDistanceToEntity(const CEntitySA* pEntity) {
	return ((float(__thiscall*)(CPedSA*, const CEntitySA*))SAMP_ADDROF(0xAB910))(this, pEntity);
}

int SAMP::CPedSA::GetVehicleSeatIndex() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAB970))(this);
}

void SAMP::CPedSA::PutIntoVehicle(GTAREF vehicle, int nSeat) {
	((void(__thiscall*)(CPedSA*, GTAREF, int))SAMP_ADDROF(0xABA00))(this, vehicle, nSeat);
}

void SAMP::CPedSA::EnterVehicle(GTAREF vehicle, BOOL bAsPassenger) {
	((void(__thiscall*)(CPedSA*, GTAREF, BOOL))SAMP_ADDROF(0xABB80))(this, vehicle, bAsPassenger);
}

void SAMP::CPedSA::ExitVehicle() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xABC50))(this);
}

void SAMP::CPedSA::WarpFromVehicle(CVectorSA putAt) {
	((void(__thiscall*)(CPedSA*, CVectorSA))SAMP_ADDROF(0xABCE0))(this, putAt);
}

void SAMP::CPedSA::SetSpawnInfo(const CVectorSA* pPosition, float fRotation) {
	((void(__thiscall*)(CPedSA*, const CVectorSA*, float))SAMP_ADDROF(0xABEC0))(this, pPosition, fRotation);
}

void SAMP::CPedSA::SetControllable(BOOL bEnable) {
	((void(__thiscall*)(CPedSA*, BOOL))SAMP_ADDROF(0xABF00))(this, bEnable);
}

char SAMP::CPedSA::GetDeathInfo(ID* pKiller) {
	return ((char(__thiscall*)(CPedSA*, ID*))SAMP_ADDROF(0xABFC0))(this, pKiller);
}

CEntity* SAMP::CPedSA::GetFloor() {
	return ((::CEntity * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC180))(this);
}

CWeaponInfo* SAMP::CPedSA::GetCurrentWeaponInfo() {
	return ((::CWeaponInfo * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC230))(this);
}

void SAMP::CPedSA::HandsUp() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC280))(this);
}

BOOL SAMP::CPedSA::DoesHandsUp() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC2D0))(this);
}

void SAMP::CPedSA::HoldObject(int nModel) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAC330))(this, nModel);
}

BOOL SAMP::CPedSA::EnablePassengerDrivebyMode() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC700))(this);
}

void SAMP::CPedSA::Extinguish() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC860))(this);
}

unsigned short SAMP::CPedSA::GetCurrentWeaponAmmo() {
	return ((unsigned short(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC8C0))(this);
}

void SAMP::CPedSA::EnableJetpack() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC480))(this);
}

void SAMP::CPedSA::DisableJetpack() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC4D0))(this);
}

BOOL SAMP::CPedSA::HasJetpack() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAC530))(this);
}

CWeapon* SAMP::CPedSA::GetWeaponSlot(int nWeapon) {
	return ((::CWeapon * (__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAC900))(this, nWeapon);
}

void SAMP::CPedSA::SetWalkStyle(const char* szName) {
	((void(__thiscall*)(CPedSA*, const char*))SAMP_ADDROF(0xAC940))(this, szName);
}

void SAMP::CPedSA::PerformAnimation(const char* szName, const char* szFile, float fFramedelta, int nLoopA, int nLockX, int nLockY, int nLockF, int nTime) {
	((void(__thiscall*)(CPedSA*, const char*, const char*, float, int, int, int, int, int))SAMP_ADDROF(0xAC9A0))(this, szName, szFile, fFramedelta, nLoopA, nLockX, nLockY, nLockF, nTime);
}

void SAMP::CPedSA::LinkToInterior(char nId, BOOL bRefresh) {
	((void(__thiscall*)(CPedSA*, char, BOOL))SAMP_ADDROF(0xACAB0))(this, nId, bRefresh);
}

void SAMP::CPedSA::DestroyParachute() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xACB50))(this);
}

BOOL SAMP::CPedSA::OpenParachute() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xACC40))(this);
}

void SAMP::CPedSA::ProcessParachuteEvent(const char* szName) {
	((void(__thiscall*)(CPedSA*, const char*))SAMP_ADDROF(0xACD90))(this, szName);
}

BOOL SAMP::CPedSA::IsOnGround() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xACFD0))(this);
}

void SAMP::CPedSA::ResetDamageEntity() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xACFF0))(this);
}

void SAMP::CPedSA::RemoveWeaponModel(int nWeapon) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAD020))(this, nWeapon);
}

float SAMP::CPedSA::GetAimZ() {
	return ((float(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD060))(this);
}

void SAMP::CPedSA::SetAimZ(float fValue) {
	((void(__thiscall*)(CPedSA*, float))SAMP_ADDROF(0xAD0A0))(this, fValue);
}

CEntity* SAMP::CPedSA::GetContactEntity() {
	return ((::CEntity * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD110))(this);
}

CVehicle* SAMP::CPedSA::GetContactVehicle() {
	return ((::CVehicle * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD120))(this);
}

int SAMP::CPedSA::GetStat() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD150))(this);
}

BOOL SAMP::CPedSA::PerformingCustomAnimation() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD170))(this);
}

void SAMP::CPedSA::StartDancing(int nStyle) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAD240))(this, nStyle);
}

void SAMP::CPedSA::StopDancing() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD290))(this);
}

BOOL SAMP::CPedSA::DoesDancing() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD2D0))(this);
}

const char* SAMP::CPedSA::GetAnimationForDance(int nMove) {
	return ((const char* (__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAD2E0))(this, nMove);
}

void SAMP::CPedSA::DropStuff() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD370))(this);
}

int SAMP::CPedSA::GetStuff() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD400))(this);
}

BOOL SAMP::CPedSA::ApplyStuff() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD410))(this);
}

void SAMP::CPedSA::ProcessDrunk() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD560))(this);
}

int SAMP::CPedSA::GetDrunkLevel() {
	return ((int(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD710))(this);
}

void SAMP::CPedSA::SetDrunkLevel(int nValue) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAD720))(this, nValue);
}

void SAMP::CPedSA::ApplyCommandTask(const char* szName, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11) {
	((void(__thiscall*)(CPedSA*, const char*, int, int, int, int, int, int, int, int, int))SAMP_ADDROF(0xAD740))(this, szName, a3, a4, a5, a6, a7, a8, a9, a10, a11);
}

void SAMP::CPedSA::DestroyCommandTask() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD790))(this);
}

void SAMP::CPedSA::EnableCellphone(BOOL bEnable) {
	((void(__thiscall*)(CPedSA*, BOOL))SAMP_ADDROF(0xAD7E0))(this, bEnable);
}

BOOL SAMP::CPedSA::UsingCellphone() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD810))(this);
}

void SAMP::CPedSA::SetFightingStyle(int nStyle) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAD840))(this, nStyle);
}

void SAMP::CPedSA::StartUrinating() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD870))(this);
}

void SAMP::CPedSA::StopUrinating() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD950))(this);
}

BOOL SAMP::CPedSA::DoesUrinating() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAD9D0))(this);
}

const char* SAMP::CPedSA::GetLoadedShoppingDataSubsection() {
	return ((const char* (__thiscall*)(CPedSA*))SAMP_ADDROF(0xADA50))(this);
}

void SAMP::CPedSA::LoadShoppingDataSubsection(const char* szName) {
	((void(__thiscall*)(CPedSA*, const char*))SAMP_ADDROF(0xADA70))(this, szName);
}

CPed* SAMP::CPedSA::GetAimedPed() {
	return ((::CPed * (__thiscall*)(CPedSA*))SAMP_ADDROF(0xAE6D0))(this);
}

void SAMP::CPedSA::SetKeys(short controllerState, short sLeftStickX, short sLeftStickY) {
	((void(__thiscall*)(CPedSA*, short, short, short))SAMP_ADDROF(0xAEAB0))(this, controllerState, sLeftStickX, sLeftStickY);
}

short SAMP::CPedSA::GetKeys(short* pLeftStickX, short* pLeftStickY) {
	return ((short(__thiscall*)(CPedSA*, short*, short*))SAMP_ADDROF(0xAED40))(this, pLeftStickX, pLeftStickY);
}

void SAMP::CPedSA::CreateArrow(int nColor) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAEEA0))(this, nColor);
}

void SAMP::CPedSA::SetModelIndex(int nModel) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAF6C0))(this, nModel);
}

void SAMP::CPedSA::Kill() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAF740))(this);
}

void SAMP::CPedSA::SetWeaponAmmo(unsigned char nWeapon, unsigned short nAmmo) {
	((void(__thiscall*)(CPedSA*, unsigned char, unsigned short))SAMP_ADDROF(0xAF7F0))(this, nWeapon, nAmmo);
}

void SAMP::CPedSA::ProcessDancing() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAF820))(this);
}

void SAMP::CPedSA::GiveStuff(int nType) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAFA40))(this, nType);
}

void SAMP::CPedSA::Destroy() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xB0710))(this);
}

void SAMP::CPedSA::SetCameraMode(char nMode) {
	((void(__thiscall*)(CPedSA*, char))SAMP_ADDROF(0x13F70))(this, nMode);
}

void SAMP::CPedSA::SetCameraExtZoomAndAspectRatio(float fExtZoom, float fAspectRatio) {
	((void(__thiscall*)(CPedSA*, float, float))SAMP_ADDROF(0x13F90))(this, fExtZoom, fAspectRatio);
}

BOOL SAMP::CPedSA::HasAccessory() {
	return ((BOOL(__thiscall*)(CPedSA*))SAMP_ADDROF(0xAE5A0))(this);
}

void SAMP::CPedSA::DeleteAccessory(int nSlot) {
	((void(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAE5C0))(this, nSlot);
}

BOOL SAMP::CPedSA::GetAccessoryState(int nSlot) {
	return ((BOOL(__thiscall*)(CPedSA*, int))SAMP_ADDROF(0xAE620))(this, nSlot);
}

void SAMP::CPedSA::DeleteAllAccessories() {
	((void(__thiscall*)(CPedSA*))SAMP_ADDROF(0xB0220))(this);
}

void SAMP::CPedSA::AddAccessory(int nSlot, const Accessory* pInfo) {
	((void(__thiscall*)(CPedSA*, int, const Accessory*))SAMP_ADDROF(0xB0280))(this, nSlot, pInfo);
}

SAMP::CObjectSA* SAMP::CPedSA::GetAccessory(int nSlot) {
	return ((CObjectSA * (__thiscall*)(CPedSA*, int))SAMP_ADDROF(0x13330))(this, nSlot);
}

#endif
