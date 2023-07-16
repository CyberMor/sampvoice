/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#include "CRemotePlayer.h"

#if defined(SAMP_R1)

SAMP::CRemotePlayer::CRemotePlayer() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x12E20))(this);
}

SAMP::CRemotePlayer::~CRemotePlayer() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x12EA0))(this);
}

void SAMP::CRemotePlayer::Process() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x12EF0))(this);
}

void SAMP::CRemotePlayer::ProcessHead() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x10EA0))(this);
}

void SAMP::CRemotePlayer::SetMarkerState(BOOL bState) {
	((void(__thiscall*)(CRemotePlayer*, BOOL))SAMP_ADDROF(0x10FF0))(this, bState);
}

void SAMP::CRemotePlayer::SetMarkerPosition(int x, int y, int z) {
	((void(__thiscall*)(CRemotePlayer*, int, int, int))SAMP_ADDROF(0x11030))(this, x, y, z);
}

BOOL SAMP::CRemotePlayer::Surfing() {
	return ((BOOL(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x110D0))(this);
}

BOOL SAMP::CRemotePlayer::SurfingOnObject() {
	return ((BOOL(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x11100))(this);
}

void SAMP::CRemotePlayer::ProcessSurfing() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x11130))(this);
}

void SAMP::CRemotePlayer::OnEnterVehicle() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x112D0))(this);
}

void SAMP::CRemotePlayer::OnExitVehicle() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x113A0))(this);
}

void SAMP::CRemotePlayer::ProcessSpecialAction() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x113F0))(this);
}

void SAMP::CRemotePlayer::Update(PACKET::AimData* pData) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::AimData*))SAMP_ADDROF(0x12080))(this, pData);
}

void SAMP::CRemotePlayer::Update(PACKET::UnoccupiedData* pData) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::UnoccupiedData*))SAMP_ADDROF(0x121D0))(this, pData);
}

void SAMP::CRemotePlayer::Update(PACKET::TrailerData* pData) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::TrailerData*))SAMP_ADDROF(0x12520))(this, pData);
}

void SAMP::CRemotePlayer::ResetData() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x12830))(this);
}

float SAMP::CRemotePlayer::GetDistanceToPlayer(CRemotePlayer* pPlayer) {
	return ((float(__thiscall*)(CRemotePlayer*, CRemotePlayer*))SAMP_ADDROF(0x12930))(this, pPlayer);
}

void SAMP::CRemotePlayer::ChangeState(char nOldState, char nNewState) {
	((void(__thiscall*)(CRemotePlayer*, char, char))SAMP_ADDROF(0x12AE0))(this, nOldState, nNewState);
}

int SAMP::CRemotePlayer::GetStatus() {
	return ((int(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x12BA0))(this);
}

BOOL SAMP::CRemotePlayer::Spawn(int a2, int nModel, CVectorSA* pPosition, float fRotation, D3DCOLOR color, char nFightingStyle) {
	return ((BOOL(__thiscall*)(CRemotePlayer*, int, int, CVectorSA*, float, D3DCOLOR, char))SAMP_ADDROF(0x13890))(this, a2, nModel, pPosition, fRotation, color, nFightingStyle);
}

void SAMP::CRemotePlayer::Update(PACKET::OnfootData* pData, TICK timestamp) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::OnfootData*, TICK))SAMP_ADDROF(0x139A0))(this, pData, timestamp);
}

void SAMP::CRemotePlayer::Update(PACKET::IncarData* pData, TICK timestamp) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::IncarData*, TICK))SAMP_ADDROF(0x13A80))(this, pData, timestamp);
}

void SAMP::CRemotePlayer::Update(PACKET::PassengerData* pData, TICK timestamp) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::PassengerData*, TICK))SAMP_ADDROF(0x13B70))(this, pData, timestamp);
}

void SAMP::CRemotePlayer::Remove() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x13C60))(this);
}

void SAMP::CRemotePlayer::Kill() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x13CA0))(this);
}

void SAMP::CRemotePlayer::Chat(const char* szText) {
	((void(__thiscall*)(CRemotePlayer*, const char*))SAMP_ADDROF(0x13D30))(this, szText);
}

float SAMP::CRemotePlayer::GetDistanceToLocalPlayer() {
	return ((float(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x129A0))(this);
}

void SAMP::CRemotePlayer::ExitVehicle() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x12AB0))(this);
}

void SAMP::CRemotePlayer::EnterVehicle(ID nVehicle, BOOL bPassenger) {
	((void(__thiscall*)(CRemotePlayer*, ID, BOOL))SAMP_ADDROF(0x12A20))(this, nVehicle, bPassenger);
}

D3DCOLOR SAMP::CRemotePlayer::GetColorAsARGB() {
	return ((D3DCOLOR(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x12A00))(this);
}

D3DCOLOR SAMP::CRemotePlayer::GetColorAsRGBA() {
	return ((D3DCOLOR(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x129F0))(this);
}

void SAMP::CRemotePlayer::SetColor(D3DCOLOR color) {
	((void(__thiscall*)(CRemotePlayer*, D3DCOLOR))SAMP_ADDROF(0x129D0))(this, color);
}

void SAMP::CRemotePlayer::SetOnfootTargetSpeedAndPosition(CVectorSA* pPosition, CVectorSA* pSpeed) {
	((void(__thiscall*)(CRemotePlayer*, CVectorSA*, CVectorSA*))SAMP_ADDROF(0x11A60))(this, pPosition, pSpeed);
}

void SAMP::CRemotePlayer::Update(PACKET::BulletData* pData) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::BulletData*))SAMP_ADDROF(0x12BE0))(this, pData);
}

BOOL SAMP::CRemotePlayer::DoesExist() {
	return ((BOOL(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x1080))(this);
}

void SAMP::CRemotePlayer::UpdateOnfootRotation() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x11990))(this);
}

void SAMP::CRemotePlayer::UpdateOnfootSpeedAndPosition() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x11840))(this);
}

void SAMP::CRemotePlayer::UpdateIncarSpeedAndPosition() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x11AC0))(this);
}

void SAMP::CRemotePlayer::UpdateIncarRotation() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x11DA0))(this);
}

void SAMP::CRemotePlayer::SetIncarTargetSpeedAndPosition(CMatrixSA* pMatrix, CVectorSA* pPosition, CVectorSA* pSpeed) {
	((void(__thiscall*)(CRemotePlayer*, CMatrixSA*, CVectorSA*, CVectorSA*))SAMP_ADDROF(0x11F10))(this, pMatrix, pPosition, pSpeed);
}

void SAMP::CRemotePlayer::UpdateTrain(CMatrixSA* pMatrix, CVectorSA* pSpeed, float fSpeed) {
	((void(__thiscall*)(CRemotePlayer*, CMatrixSA*, CVectorSA*, float))SAMP_ADDROF(0x11F80))(this, pMatrix, pSpeed, fSpeed);
}

#elif defined(SAMP_R3)

SAMP::CRemotePlayer::CRemotePlayer() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x16040))(this);
}

SAMP::CRemotePlayer::~CRemotePlayer() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x160C0))(this);
}

void SAMP::CRemotePlayer::ProcessHead() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x13FD0))(this);
}

void SAMP::CRemotePlayer::SetMarkerState(BOOL bState) {
	((void(__thiscall*)(CRemotePlayer*, BOOL))SAMP_ADDROF(0x14120))(this, bState);
}

void SAMP::CRemotePlayer::SetMarkerPosition(int x, int y, int z) {
	((void(__thiscall*)(CRemotePlayer*, int, int, int))SAMP_ADDROF(0x14160))(this, x, y, z);
}

BOOL SAMP::CRemotePlayer::SurfingOnVehicle() {
	return ((BOOL(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x14200))(this);
}

BOOL SAMP::CRemotePlayer::SurfingOnObject() {
	return ((BOOL(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x14230))(this);
}

void SAMP::CRemotePlayer::ProcessSurfing() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x14260))(this);
}

void SAMP::CRemotePlayer::OnEnterVehicle() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x14410))(this);
}

void SAMP::CRemotePlayer::OnExitVehicle() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x144E0))(this);
}

void SAMP::CRemotePlayer::ProcessSpecialAction() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x14530))(this);
}

void SAMP::CRemotePlayer::UpdateOnfootSpeedAndPosition() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x14780))(this);
}

void SAMP::CRemotePlayer::UpdateOnfootRotation() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x14B10))(this);
}

void SAMP::CRemotePlayer::SetOnfootTargetSpeedAndPosition(CVectorSA* pPosition, CVectorSA* pSpeed) {
	((void(__thiscall*)(CRemotePlayer*, CVectorSA*, CVectorSA*))SAMP_ADDROF(0x14BE0))(this, pPosition, pSpeed);
}

void SAMP::CRemotePlayer::UpdateIncarSpeedAndPosition() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x14C50))(this);
}

void SAMP::CRemotePlayer::UpdateIncarRotation() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x14F50))(this);
}

void SAMP::CRemotePlayer::SetIncarTargetSpeedAndPosition(CMatrixSA* pMatrix, CVectorSA* pPosition, CVectorSA* pSpeed) {
	((void(__thiscall*)(CRemotePlayer*, CMatrixSA*, CVectorSA*, CVectorSA*))SAMP_ADDROF(0x150C0))(this, pMatrix, pPosition, pSpeed);
}

void SAMP::CRemotePlayer::UpdateTrain(CMatrixSA* pMatrix, CVectorSA* pSpeed, float fSpeed) {
	((void(__thiscall*)(CRemotePlayer*, CMatrixSA*, CVectorSA*, float))SAMP_ADDROF(0x15130))(this, pMatrix, pSpeed, fSpeed);
}

void SAMP::CRemotePlayer::Update(PACKET::AimData* pData) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::AimData*))SAMP_ADDROF(0x15230))(this, pData);
}

void SAMP::CRemotePlayer::Update(PACKET::UnoccupiedData* pData) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::UnoccupiedData*))SAMP_ADDROF(0x15380))(this, pData);
}

void SAMP::CRemotePlayer::Update(PACKET::TrailerData* pData) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::TrailerData*))SAMP_ADDROF(0x15740))(this, pData);
}

void SAMP::CRemotePlayer::ResetData() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x15A50))(this);
}

float SAMP::CRemotePlayer::GetDistanceToPlayer(CRemotePlayer* pPlayer) {
	return ((float(__thiscall*)(CRemotePlayer*, CRemotePlayer*))SAMP_ADDROF(0x15B40))(this, pPlayer);
}

float SAMP::CRemotePlayer::GetDistanceToLocalPlayer() {
	return ((float(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x15BB0))(this);
}

void SAMP::CRemotePlayer::SetColor(D3DCOLOR color) {
	((void(__thiscall*)(CRemotePlayer*, D3DCOLOR))SAMP_ADDROF(0x15BE0))(this, color);
}

D3DCOLOR SAMP::CRemotePlayer::GetColorAsRGBA() {
	return ((D3DCOLOR(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x15C00))(this);
}

D3DCOLOR SAMP::CRemotePlayer::GetColorAsARGB() {
	return ((D3DCOLOR(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x15C10))(this);
}

void SAMP::CRemotePlayer::EnterVehicle(ID nId, BOOL bPassenger) {
	((void(__thiscall*)(CRemotePlayer*, ID, BOOL))SAMP_ADDROF(0x15C30))(this, nId, bPassenger);
}

void SAMP::CRemotePlayer::ExitVehicle() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x15CC0))(this);
}

void SAMP::CRemotePlayer::ChangeState(char nOld, char nNew) {
	((void(__thiscall*)(CRemotePlayer*, char, char))SAMP_ADDROF(0x15CF0))(this, nOld, nNew);
}

int SAMP::CRemotePlayer::GetStatus() {
	return ((int(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x15DB0))(this);
}

void SAMP::CRemotePlayer::Update(PACKET::BulletData* pData) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::BulletData*))SAMP_ADDROF(0x15DF0))(this, pData);
}

void SAMP::CRemotePlayer::Process() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x16110))(this);
}

BOOL SAMP::CRemotePlayer::Spawn(int a2, int nModel, int a4, CVectorSA* pPosition, float fRotation, D3DCOLOR color, char nFightingStyle) {
	return ((BOOL(__thiscall*)(CRemotePlayer*, int, int, int, CVectorSA*, float, D3DCOLOR, char))SAMP_ADDROF(0x16A90))(this, a2, nModel, a4, pPosition, fRotation, color, nFightingStyle);
}

void SAMP::CRemotePlayer::Update(PACKET::OnfootData* pData, TICK timestamp) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::OnfootData*, TICK))SAMP_ADDROF(0x16BB0))(this, pData, timestamp);
}

void SAMP::CRemotePlayer::Update(PACKET::IncarData* pData, TICK timestamp) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::IncarData*, TICK))SAMP_ADDROF(0x16C90))(this, pData, timestamp);
}

void SAMP::CRemotePlayer::Update(PACKET::PassengerData* pData, TICK timestamp) {
	((void(__thiscall*)(CRemotePlayer*, PACKET::PassengerData*, TICK))SAMP_ADDROF(0x16D80))(this, pData, timestamp);
}

void SAMP::CRemotePlayer::Remove() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x16E70))(this);
}

void SAMP::CRemotePlayer::Kill() {
	((void(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x16EB0))(this);
}

void SAMP::CRemotePlayer::Chat(const char* szText) {
	((void(__thiscall*)(CRemotePlayer*, const char*))SAMP_ADDROF(0x16F60))(this, szText);
}

BOOL SAMP::CRemotePlayer::DoesExist() {
	return ((BOOL(__thiscall*)(CRemotePlayer*))SAMP_ADDROF(0x1080))(this);
}

#endif
