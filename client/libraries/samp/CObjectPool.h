/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CObjectSA.h"
#include "CVectorSA.h"

#define MAX_OBJECTS 1000

class CObject;

SAMP_BEGIN

class SAMP_API CObjectPool {
public:

	int			m_nLargestId;
	BOOL		m_bNotEmpty[MAX_OBJECTS];
	CObjectSA*	m_pObject[MAX_OBJECTS];

	static TICK& lastProcess();

	CObjectPool();
	~CObjectPool();

	void UpdateLargestId();
	int GetCount();
	BOOL Delete(ID nId);
	BOOL Create(ID nId, int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance);
	CObjectSA* Find(::CObject* pGameObject);
	int GetId(::CObject* pGameObject);
	void Process();
	void ConstructMaterials();
	void ShutdownMaterials();
	void Draw();
	void DrawLast();

	CObjectSA* GetObject(ID nId) {
		if (m_bNotEmpty[nId])
			return m_pObject[nId];
		return nullptr;
	}

};

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"
#include "CObjectSA.h"

#define MAX_OBJECTS 1000

class CObject;

SAMP_BEGIN

class SAMP_API CObjectPool {
public:

	int			m_nLargestId;
	BOOL		m_bNotEmpty[MAX_OBJECTS];
	CObjectSA*	m_pObject[MAX_OBJECTS];

	CObjectPool();
	~CObjectPool();

	void UpdateLargestId();
	int GetCount();
	BOOL Delete(ID nId);
	BOOL Create(ID nId, int nModel, CVectorSA position, CVectorSA rotation, float fDrawDistance);
	CObjectSA* Find(::CObject* pGameObject);
	int GetId(::CObject* pGameObject);
	void Process();
	void ConstructMaterials();
	void ShutdownMaterials();
	void Draw();
	void DrawLast();
	CObjectSA* GetObject(ID nId);

};

SAMP_END

#endif
