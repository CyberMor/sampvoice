/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CRectSA.h"

#define MAX_DEATHMESSAGES 5

SAMP_BEGIN

class SAMP_API CDeathWindow {
public:

	BOOL				m_bEnabled;

	struct SAMP_API Entry {

		char			m_szKiller[25];
		char			m_szVictim[25];
		D3DCOLOR		m_killerColor;
		D3DCOLOR		m_victimColor;
		char			m_nWeapon;

	}					m_entry[MAX_DEATHMESSAGES];

	int 				m_nLongestNickWidth;
	int					m_position[2];
	ID3DXFont*			m_pFont;
	ID3DXFont*			m_pWeaponFont1;
	ID3DXFont*			m_pWeaponFont2;
	ID3DXSprite*		m_pSprite;
	IDirect3DDevice9*	m_pDevice;
	BOOL				m_bAuxFontInited;
	ID3DXFont*			m_pAuxFont1;
	ID3DXFont*			m_pAuxFont2;

	CDeathWindow(IDirect3DDevice9* pDevice);
	~CDeathWindow();

	void InitializeAuxFonts();
	void PushBack();
	void DrawText(const char* szText, CRectSA rect, D3DCOLOR color, int nFormat);
	void DrawWeaponSprite(const char* szSpriteId, CRectSA rect, D3DCOLOR color);
	void GetWeaponSpriteRectSize(void* pPoint);
	const char* GetWeaponSpriteId(char nWeapon);
	void ResetFonts();
	void Draw();
	void AddEntry(const char* szKiller, const char* szVictim, D3DCOLOR killerColor, D3DCOLOR victimColor, char nWeapon);
	void AddMessage(const char* szKiller, const char* szVictim, D3DCOLOR killerColor, D3DCOLOR victimColor, char nWeapon);

};

CDeathWindow*& pDeathWindow();

SAMP_END

#elif defined(SAMP_R3)

#include "CRectSA.h"

#define MAX_DEATHMESSAGES 5

SAMP_BEGIN

class SAMP_API CDeathWindow {
public:

	BOOL					m_bEnabled;

	struct SAMP_API Entry {

		char				m_szKiller[25];
		char				m_szVictim[25];
		D3DCOLOR			m_killerColor;
		D3DCOLOR			m_victimColor;
		char				m_nWeapon;

	}						m_entry[MAX_DEATHMESSAGES];

	int 					m_nLongestNickWidth;
	int						m_position[2];
	ID3DXFont*				m_pFont;
	ID3DXFont*				m_pWeaponFont1;
	ID3DXFont*				m_pWeaponFont2;
	ID3DXSprite*			m_pSprite;
	IDirect3DDevice9*		m_pDevice;
	BOOL					m_bAuxFontInitialized;
	ID3DXFont*				m_pAuxFont1;
	ID3DXFont*				m_pAuxFont2;

	CDeathWindow(IDirect3DDevice9* pDevice);
	~CDeathWindow();

	void InitializeAuxFonts();
	void PushBack();
	void DrawText(const char* szText, CRectSA rect, D3DCOLOR color, int nFormat);
	void DrawWeaponSprite(const char* szSpriteId, CRectSA rect, D3DCOLOR color);
	void GetWeaponSpriteRectSize(void* pPoint);
	const char* GetWeaponSpriteId(char nWeapon);
	void ResetFonts();
	void Draw();
	void AddEntry(const char* szKiller, const char* szVictim, D3DCOLOR killerColor, D3DCOLOR victimColor, char nWeapon);
	void AddMessage(const char* szKiller, const char* szVictim, D3DCOLOR killerColor, D3DCOLOR victimColor, char nWeapon);

};

CDeathWindow*& pDeathWindow();

SAMP_END

#endif
