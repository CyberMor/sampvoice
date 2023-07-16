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

SAMP_BEGIN

class SAMP_API CScoreboard {
public:

	BOOL				m_bIsEnabled;
	int					m_nPlayerCount;
	float				m_position[2];
	float				m_fScalar;
	float				m_size[2];
	float pad[5];
	IDirect3DDevice9*	m_pDevice;
	CDXUTDialog*		m_pDialog;
	CDXUTListBox*		m_pListBox;
	int					m_nCurrentOffset;
	BOOL				m_bIsSorted;

	CScoreboard(IDirect3DDevice9* pDevice);

	void Recalc();
	void GetRect(CRectSA* pRect);
	void Close(bool bHideCursor);
	void ResetDialogControls(CDXUTDialog* pDialog);
	void SendNotification();
	void UpdateList();
	void Draw();
	void Enable();

};

CScoreboard*& pScoreboard();

SAMP_END

#elif defined(SAMP_R3)

#include "CRectSA.h"

SAMP_BEGIN

class SAMP_API CScoreboard {
public:

	BOOL				m_bIsEnabled;
	int					m_nPlayerCount;
	float				m_position[2];
	float				m_fScalar;
	float				m_size[2];
	float pad_[5];
	IDirect3DDevice9*	m_pDevice;
	CDXUTDialog*		m_pDialog;
	CDXUTListBox*		m_pListbox;
	int					m_nCurrentOffset;
	BOOL				m_bIsSorted;

	CScoreboard(IDirect3DDevice9* pDevice);

	void Recalc();
	void GetRect(CRectSA* pRect);
	void Close(bool bHideCursor);
	void ResetDialogControls(CDXUTDialog* pDialog);
	void SendNotification();
	void UpdateList();
	void Draw();
	void Enable();

};

CScoreboard*& pScoreboard();

SAMP_END

#endif
