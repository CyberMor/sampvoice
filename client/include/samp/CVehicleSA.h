/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CEntitySA.h"
#include "CVectorSA.h"

#define LICENSE_PLATE_TEXT_LEN 32

class CVehicle;

SAMP_BEGIN

class SAMP_API CVehicleSA : public CEntitySA {
public:

	// void **m_lpVtbl = 0xDB1AC;

	CVehicleSA*			m_pTrailer;
	::CVehicle*			m_pGameVehicle;
	char pad_50[8];
	BOOL				m_bIsInvulnerable;
	BOOL				m_bIsLightsOn;
	BOOL				m_bIsLocked;
	bool				m_bIsObjective;
	BOOL				m_bObjectiveBlipCreated;
	TICK				m_timeSinceLastDriven;
	BOOL				m_bHasBeenDriven;
	char pad_71[4];
	BOOL				m_bEngineState;
	NUMBER				m_nPrimaryColor;
	NUMBER				m_nSecondaryColor;
	BOOL				m_bNeedsToUpdateColor;
	BOOL				m_bUnoccupiedSync;
	BOOL				m_bRemoteUnocSync;
	BOOL				m_bKeepModelLoaded;
	int					m_bHasSiren;
	IDirect3DTexture9*	m_pLicensePlate;
	char				m_szLicensePlateText[LICENSE_PLATE_TEXT_LEN + 1];
	GTAREF				m_marker;

	CVehicleSA() { *(void**)this = (void*)SAMP_ADDROF(0xDB1AC); }
	CVehicleSA(int nModel, CVectorSA position, float fRotation, BOOL bKeepModelLoaded, BOOL bHasSiren);

	virtual ~CVehicleSA() SAMP_VIRTUAL;
	virtual void Add() SAMP_VIRTUAL;
	virtual void Remove() SAMP_VIRTUAL;

	void ChangeInterior(int nId);
	void ResetPointers();
	BOOL HasDriver();
	BOOL IsOccupied();
	void SetInvulnerable(BOOL bInv);
	void SetLocked(BOOL block);
	float GetHealth();
	void SetHealth(float fValue);
	void SetColor(NUMBER nPrimary, NUMBER nSecondary);
	void UpdateColor();
	int GetSubtype();
	BOOL IsSunk();
	BOOL IsWrecked();
	BOOL DriverIsPlayerPed();
	BOOL HasPlayerPed();
	BOOL IsTrainPart();
	BOOL HasTurret();
	void EnableSiren(bool bEnable);
	BOOL SirenEnabled();
	void SetLandingGearState(BOOL bHide);
	BOOL GetLandingGearState();
	void SetHydraThrusters(int nDirection);
	int GetHydraThrusters();
	void ProcessMarkers();
	void Lock(BOOL bLock);
	BOOL UpdateLastDrivenTime();
	float GetTrainSpeed();
	void SetTrainSpeed(float fValue);
	void SetTires(char nState);
	char GetTires();
	void UpdateDamage(int nPanels, int nDoors, char nLights);
	int GetPanelsDamage();
	int GetDoorsDamage();
	char GetLightsDamage();
	void AttachTrailer();
	void DetachTrailer();
	void SetTrailer(CVehicleSA* pVehicle);
	CVehicleSA* GetTrailer();
	CVehicleSA* GetTractor();
	BOOL IsTrailer();
	BOOL IsTowtruck();
	BOOL IsRC();
	void EnableLights(bool bEnable);
	void RemovePassengers();
	BOOL AddComponent(unsigned short nId);
	BOOL RemoveComponent(unsigned short nId);
	void SetPaintjob(NUMBER nId);
	BOOL DoesExist();
	void SetLicensePlateText(const char* szText);
	void SetRotation(float fValue);
	void ConstructLicensePlate();
	void ShutdownLicensePlate();
	BOOL HasSiren();
	char GetMaxPassengers();
	void SetWindowOpenFlag(NUMBER nDoorId);
	void ClearWindowOpenFlag(NUMBER nDoorId);
	void EnableEngine(BOOL bEnable);

};

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"
#include "CEntitySA.h"

#define MAX_LICENSE_PLATE_TEXT 32

class CVehicle;

SAMP_BEGIN

class SAMP_API CVehicleSA : public CEntitySA {
public:

	// void **m_lpVtbl = 0xED6E8;

	CVehicleSA*			m_pTrailer;
	::CVehicle*			m_pGameVehicle;
	char pad_50[8];
	BOOL				m_bIsInvulnerable;
	BOOL				m_bIsLightsOn;
	BOOL				m_bIsLocked;
	bool				m_bIsObjective;
	BOOL				m_bObjectiveBlipCreated;
	TICK				m_timeSinceLastDriven;
	BOOL				m_bHasBeenDriven;
	char pad_71[4];
	BOOL				m_bEngineState;
	unsigned char		m_nPrimaryColor;
	unsigned char		m_nSecondaryColor;
	BOOL				m_bNeedsToUpdateColor;
	BOOL				m_bUnoccupiedSync;
	BOOL				m_bRemoteUnocSync;
	BOOL				m_bKeepModelLoaded;
	int					m_bHasSiren;
	IDirect3DTexture9*	m_pLicensePlate;
	char				m_szLicensePlateText[MAX_LICENSE_PLATE_TEXT + 1];
	GTAREF				m_marker;

	virtual ~CVehicleSA() SAMP_VIRTUAL;
	virtual void Add() SAMP_VIRTUAL;
	virtual void Remove() SAMP_VIRTUAL;

	CVehicleSA() { *(void**)this = (void*)SAMP_ADDROF(0xED6E8); }
	CVehicleSA(int nModel, CVectorSA position, float fRotation, BOOL bKeepModelLoaded, BOOL bHasSiren);

	void ChangeInterior(int nId);
	void ResetPointers();
	BOOL HasDriver();
	BOOL IsOccupied();
	void SetInvulnerable(BOOL bInv);
	void SetLocked(BOOL block);
	float GetHealth();
	void SetHealth(float fValue);
	void SetColor(NUMBER nPrimary, NUMBER nSecondary);
	void UpdateColor();
	int GetSubtype();
	BOOL IsSunk();
	BOOL IsWrecked();
	BOOL DriverIsPlayerPed();
	BOOL HasPlayerPed();
	BOOL IsTrainPart();
	BOOL HasTurret();
	void EnableSiren(bool bEnable);
	BOOL SirenEnabled();
	void SetLandingGearState(BOOL bHide);
	BOOL GetLandingGearState();
	void SetHydraThrusters(int nDirection);
	int GetHydraThrusters();
	void ProcessMarkers();
	void Lock(BOOL bLock);
	BOOL UpdateLastDrivenTime();
	float GetTrainSpeed();
	void SetTrainSpeed(float fValue);
	void SetTires(char nState);
	char GetTires();
	void UpdateDamage(int nPanels, int nDoors, char nLights);
	int GetPanelsDamage();
	int GetDoorsDamage();
	char GetLightsDamage();
	void AttachTrailer();
	void DetachTrailer();
	void SetTrailer(CVehicleSA* pVehicle);
	CVehicleSA* GetTrailer();
	CVehicleSA* GetTractor();
	BOOL IsTrailer();
	BOOL IsTowtruck();
	BOOL IsRC();
	void EnableLights(bool bEnable);
	void RemovePassengers();
	BOOL AddComponent(unsigned short nId);
	BOOL RemoveComponent(unsigned short nId);
	void SetPaintjob(NUMBER nId);
	BOOL DoesExist();
	void SetLicensePlateText(const char* szText);
	void SetRotation(float fValue);
	void ConstructLicensePlate();
	void ShutdownLicensePlate();
	BOOL HasSiren();
	char GetMaxPassengers();
	void SetWindowOpenFlag(NUMBER nDoorId);
	void ClearWindowOpenFlag(NUMBER nDoorId);
	void EnableEngine(BOOL bEnable);

};

SAMP_END

#endif
