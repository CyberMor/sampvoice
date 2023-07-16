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

class SAMP_API CObjectSelection {
public:

	BOOL	m_bIsActive;
	ID		m_nHoveredObject;

	CObjectSelection();

	ID DefineObject();
	void DrawLabels();
	void Enable(BOOL bEnable);
	void Draw();
	void SendNotification();
	BOOL MsgProc(int uMsg, int wParam, int lParam);

};

CObjectSelection*& pObjectSelection();

SAMP_END

#elif defined(SAMP_R3)

SAMP_BEGIN

class SAMP_API CObjectSelection {
public:

	BOOL	m_bIsActive;
	ID		m_nHoveredObject;

	CObjectSelection();

	ID DefineObject();
	void DrawLabels();
	void Enable(BOOL bEnable);
	void Draw();
	void SendNotification(); // enter
	BOOL MsgProc(int uMsg, int wParam, int lParam);

};

CObjectSelection*& pObjectSelection();

SAMP_END

#endif
