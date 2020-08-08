/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

SAMP_BEGIN

class SAMP_API CTextDrawSelection {
public:

	BOOL		m_bIsActive;
	D3DCOLOR	m_hoveredColor;
	ID			m_nHoveredId;

	CTextDrawSelection() {
		m_bIsActive = false;
		m_hoveredColor = -1;
		m_nHoveredId = -1;
	}

	~CTextDrawSelection() {
		if (m_bIsActive)
			ResetTextDraws();
	}

	void ResetTextDraws();
	void RawProcess();
	void Process();
	void Enable(D3DCOLOR hoveredColor);
	void SendNotification();
	void Disable();
	BOOL MsgProc(int uMsg, int wParam, int lParam);

};

CTextDrawSelection*& pTextdrawSelection();

SAMP_END

#elif defined(SAMP_R3)

SAMP_BEGIN

class SAMP_API CTextDrawSelection {
public:

	BOOL		m_bIsActive;
	D3DCOLOR	m_hoveredColor;
	ID			m_nHoveredId;

	CTextDrawSelection() {
		m_bIsActive = false;
		m_hoveredColor = -1;
		m_nHoveredId = -1;
	}

	~CTextDrawSelection() {
		if (m_bIsActive)
			ResetTextDraws();
	}

	void ResetTextDraws();
	void RawProcess();
	void Process();
	void Enable(D3DCOLOR hoveredColor);
	void SendNotification();
	void Disable();
	BOOL MsgProc(int uMsg, int wParam, int lParam);

};

CTextDrawSelection*& pTextDrawSelection();

SAMP_END

#endif
