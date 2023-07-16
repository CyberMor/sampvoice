/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CFonts.h"

SAMP_BEGIN

class SAMP_API CSpawnScreen {
public:

	BOOL					m_bEnabled;
	char*					m_szSpawnText;
	CFonts*					m_pFont;
	IDirect3DDevice9*		m_pDevice;
	IDirect3DTexture9*		m_pTexture;
	IDirect3DStateBlock9*	m_pStateBlockSaved;
	IDirect3DStateBlock9*	m_pStateBlockDraw;
	void*					m_pSprite;

	CSpawnScreen(IDirect3DDevice9* pDevice);
	~CSpawnScreen();

	void SetText(const char* szString);
	void OnResetDevice();
	void OnLostDevice();
	void Draw();

};

CSpawnScreen*& pSpawnScreen();

SAMP_END

#elif defined(SAMP_R3)

#include "CFonts.h"

SAMP_BEGIN

class SAMP_API CSpawnScreen {
public:

	BOOL					m_bEnabled;
	char*					m_szSpawnText;
	CFonts*					m_pFont;
	IDirect3DDevice9*		m_pDevice;
	IDirect3DTexture9*		m_pTexture;
	IDirect3DStateBlock9*	m_pStateBlockSaved;
	IDirect3DStateBlock9*	m_pStateBlockDraw;
	ID3DXSprite*			m_pSprite;

	CSpawnScreen(IDirect3DDevice9* pDevice);
	~CSpawnScreen();

	void SetText(const char* szString);
	void OnResetDevice();
	void OnLostDevice();
	void Draw();

};

CSpawnScreen*& pSpawnScreen();

SAMP_END

#endif
