/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CVectorSA.h"

#define MAX_TEXT_LABELS 2048

SAMP_BEGIN

struct SAMP_API TextLabel {

	char*		m_pText;
	D3DCOLOR	m_color;
	CVectorSA	m_position;
	float		m_fDrawDistance;
	bool		m_bBehindWalls;
	ID			m_nAttachedToPlayer;
	ID			m_nAttachedToVehicle;

};

class SAMP_API CLabelPool {
public:

	TextLabel	m_object[MAX_TEXT_LABELS];
	BOOL		m_bNotEmpty[MAX_TEXT_LABELS];

	CLabelPool();
	~CLabelPool();

	void Create(ID nId, const char* szText, D3DCOLOR color, CVectorSA position, float fDrawDistance, bool bBehindWalls, ID nAttachedToPlayer, ID nAttachedToVehicle);
	BOOL Delete(ID nId);
	void Draw();

};

SAMP_END

#elif defined(SAMP_R3)

#include "CVectorSA.h"

#define MAX_TEXT_LABELS 2048

SAMP_BEGIN

struct SAMP_API TextLabel {

	char*		m_pText;
	D3DCOLOR	m_color;
	CVectorSA	m_position;
	float		m_fDrawDistance;
	bool		m_bBehindWalls;
	ID			m_nAttachedToPlayer;
	ID			m_nAttachedToVehicle;

};

class SAMP_API CLabelPool {
public:

	TextLabel	m_object[MAX_TEXT_LABELS];
	BOOL		m_bNotEmpty[MAX_TEXT_LABELS];

	CLabelPool();
	~CLabelPool();

	void Create(ID nId, const char* szText, D3DCOLOR color, CVectorSA position, float fDrawDistance, bool bBehindWalls, ID nAttachedToPlayer, ID nAttachedToVehicle);
	BOOL Delete(ID nId);
	void Draw();

};

SAMP_END

#endif
