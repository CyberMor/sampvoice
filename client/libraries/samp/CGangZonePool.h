/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#define MAX_GANGZONES 1024

SAMP_BEGIN

struct SAMP_API GangZone {

	struct SAMP_API {

		float	left;
		float	bottom;
		float	right;
		float	top;

	}			m_rect;

	D3DCOLOR	m_color;
	D3DCOLOR	m_altColor;

};

class SAMP_API CGangZonePool {
public:

	GangZone*	m_pObject[MAX_GANGZONES];
	BOOL		m_bNotEmpty[MAX_GANGZONES];

	CGangZonePool();
	~CGangZonePool();

	void Create(ID nId, float left, float top, float right, float bottom, D3DCOLOR color);
	void StartFlashing(ID nId, D3DCOLOR color);
	void StopFlashing(ID nId);
	void Delete(ID nId);
	void Draw();

};

SAMP_END

#elif defined(SAMP_R3)

#define MAX_GANGZONES 1024

SAMP_BEGIN

struct SAMP_API GangZone {

	struct SAMP_API {

		float	left;
		float	bottom;
		float	right;
		float	top;

	}			m_rect;

	D3DCOLOR	m_color;
	D3DCOLOR	m_altColor;

};

class SAMP_API CGangZonePool {
public:

	GangZone*	m_pObject[MAX_GANGZONES];
	BOOL		m_bNotEmpty[MAX_GANGZONES];

	CGangZonePool();
	~CGangZonePool();

	void Create(ID nId, float left, float top, float right, float bottom, D3DCOLOR color);
	void StartFlashing(ID nId, D3DCOLOR color);
	void StopFlashing(ID nId);
	void Delete(ID nId);
	void Draw();

};

SAMP_END

#endif
