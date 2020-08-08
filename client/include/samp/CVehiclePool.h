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
#include "CVectorSA.h"

#define MAX_VEHICLES 2000
#define WAITING_LIST_SIZE 100

class CVehicle;

SAMP_BEGIN

class SAMP_API CVehiclePool {
public:

	struct SAMP_API Info {

		ID			m_nId;
		int			m_nType;
		CVectorSA	m_position;
		float		m_fRotation;
		NUMBER		m_nPrimaryColor;
		NUMBER		m_nSecondaryColor;
		float		m_fHealth;
		char		m_nInterior;
		int			m_nDoorDamageStatus;
		int			m_nPanelDamageStatus;
		char		m_nLightDamageStatus;
		bool		m_bDoorsLocked;
		bool		m_bHasSiren;

	};

	int				m_nCount;

	// vehicles that will be created after loading the model
	struct SAMP_API {

		Info		m_entry[WAITING_LIST_SIZE];
		BOOL		m_bNotEmpty[WAITING_LIST_SIZE];

	}				m_waiting;

	CVehicleSA*		m_pObject[MAX_VEHICLES];
	BOOL			m_bNotEmpty[MAX_VEHICLES];
	::CVehicle*		m_pGameObject[MAX_VEHICLES];
	int pad_6ef4[MAX_VEHICLES];
	ID				m_nLastUndrivenId[MAX_VEHICLES]; // a player who send unoccupied sync data
	TICK			m_lastUndrivenProcessTick[MAX_VEHICLES];
	BOOL			m_bIsActive[MAX_VEHICLES];
	BOOL			m_bIsDestroyed[MAX_VEHICLES];
	TICK			m_tickWhenDestroyed[MAX_VEHICLES];
	CVectorSA		m_spawnedAt[MAX_VEHICLES];
	BOOL			m_bNeedsToInitializeLicensePlates;

	CVehiclePool();
	~CVehiclePool();

	void UpdateCount();
	BOOL Delete(ID nId);
	void ChangeInterior(ID nId, int nInteriorId);
	void SetParams(ID nId, bool bIsObjective, bool bIsLocked);
	ID Find(::CVehicle* pGameObject);
	GTAREF GetRef(int nId);
	GTAREF GetRef(::CVehicle* pGameObject);
	ID GetNearest();
	ID GetNearest(CVectorSA point);
	void AddToWaitingList(const Info* pInfo);
	void ConstructLicensePlates();
	void ShutdownLicensePlates();
	BOOL Create(Info* pInfo);
	void SendDestroyNotification(ID nId);
	void ProcessWaitingList();
	void Process();
	CVehicleSA* GetObject(ID nId);
	BOOL DoesExist(ID nId);

};

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"
#include "CVehicleSA.h"

#define MAX_VEHICLES 2000
#define WAITING_LIST_SIZE 100

SAMP_BEGIN

class SAMP_API CVehiclePool {
public:

	struct SAMP_API Info {

		ID			m_nId;
		int			m_nType;
		CVectorSA	m_position;
		float		m_fRotation;
		NUMBER		m_nPrimaryColor;
		NUMBER		m_nSecondaryColor;
		float		m_fHealth;
		char		m_nInterior;
		int			m_nDoorDamageStatus;
		int			m_nPanelDamageStatus;
		char		m_nLightDamageStatus;
		bool		m_bDoorsLocked;
		bool		m_bHasSiren;

	};

	int				m_nCount;

	// vehicles that will be created after loading the model
	struct SAMP_API {

		Info		m_entry[WAITING_LIST_SIZE];
		BOOL		m_bNotEmpty[WAITING_LIST_SIZE];

	}				m_waitingList;

	CVehicleSA*		m_pObject[MAX_VEHICLES];
	BOOL			m_bNotEmpty[MAX_VEHICLES];
	::CVehicle*		m_pGameObject[MAX_VEHICLES];
	unsigned int pad_6ef4[MAX_VEHICLES];
	ID				m_nLastUndrivenId[MAX_VEHICLES]; // a player who send unoccupied sync data
	TICK			m_lastUndrivenProcessTick[MAX_VEHICLES];
	BOOL			m_bIsActive[MAX_VEHICLES];
	BOOL			m_bIsDestroyed[MAX_VEHICLES];
	TICK			m_tickWhenDestroyed[MAX_VEHICLES];
	CVectorSA		m_spawnedAt[MAX_VEHICLES];
	BOOL			m_bNeedsToInitializeLicensePlates;

	CVehiclePool();
	~CVehiclePool();

	void UpdateCount();
	BOOL Delete(ID nId);
	void ChangeInterior(ID nId, int nInteriorId);
	void SetParams(ID nId, bool bIsObjective, bool bIsLocked);
	ID Find(::CVehicle* pGameObject);
	GTAREF GetRef(int nId);
	GTAREF GetRef(::CVehicle* pGameObject);
	ID GetNearest();
	ID GetNearest(CVectorSA point);
	void AddToWaitingList(const Info* pInfo);
	void ConstructLicensePlates();
	void ShutdownLicensePlates();
	BOOL Create(Info* pInfo);
	void SendDestroyNotification(ID nId);
	void ProcessWaitingList();
	void Process();
	CVehicleSA* GetObject(ID nId);
	BOOL DoesExit(ID nId);

};

SAMP_END

#endif
