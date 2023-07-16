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

class SAMP_API CDialog {
public:

	IDirect3DDevice9*	m_pDevice;
	unsigned long		m_position[2];
	unsigned long		m_size[2];
	unsigned long		m_buttonOffset[2];
	CDXUTDialog*		m_pDialog;
	CDXUTListBox*		m_pListbox;
	CDXUTIMEEditBox*	m_pEditbox;
	BOOL				m_bIsActive;
	int					m_nType;
	int					m_nId;
	char*				m_szText;
	int					m_textSize[2];
	char				m_szCaption[65];
	BOOL				m_bServerside;

	CDialog(IDirect3DDevice9* pDevice);

	enum { MessageBox, Input, List, PasswordInput, TabList, HeadersList };

	void GetScreenRect(CRectSA* pRect);
	int GetTextScreenLength(const char* szString);
	void Hide();
	void ResetDialogControls(CDXUTDialog* pDialog);
	void Show(int nId, int nType, const char* szCaption, const char* szText, const char* szLeftButton, const char* szRightButton, BOOL bServerside);
	void Close(char nProcessButton);
	void Draw();
	void UpdateRects();

};

CDialog*& pDialog();

SAMP_END

#elif defined(SAMP_R3)

#include "CRectSA.h"

SAMP_BEGIN

class SAMP_API CDialog {
public:

	IDirect3DDevice9*	m_pDevice;
	unsigned long		m_position[2];
	unsigned long		m_size[2];
	unsigned long		m_buttonOffset[2];
	CDXUTDialog*		m_pDialog;
	CDXUTListBox*		m_pListbox;
	CDXUTIMEEditBox*	m_pEditbox;
	BOOL				m_bIsActive;
	int					m_nType;
	unsigned int		m_nId;
	char*				m_szText;
	unsigned long		m_textSize[2];
	char				m_szCaption[65];
	BOOL				m_bServerside;
	char pad[536];

	enum { MessageBox, Input, List, PasswordInput, TabList, HeadersList };

	CDialog(IDirect3DDevice9* pDevice);

	void GetScreenRect(CRectSA* pRect);
	int GetTextScreenLength(const char* szString);
	void Hide();
	void ResetDialogControls(CDXUTDialog* pDialog);
	void Show(int nId, int nType, const char* szCaption, const char* szText, const char* szLeftButton, const char* szRightButton, BOOL bServerside);
	void Close(char nProcessButton);
	void Draw();

};

CDialog*& pDialog();

SAMP_END

#endif
