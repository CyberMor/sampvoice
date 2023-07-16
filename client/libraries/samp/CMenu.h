/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#define MAX_MENU_ITEMS 12
#define MAX_COLUMNS 2
#define MAX_MENU_LINE 32

SAMP_BEGIN

class SAMP_API CMenu {
public:

	struct SAMP_API Interaction {

		BOOL			m_bMenu;
		BOOL			m_bRow[MAX_MENU_ITEMS];
		BOOL			m_bPadding[8 - ((MAX_MENU_ITEMS + 1) % 8)];

	};

	unsigned char		m_nId;
	char				m_szTitle[MAX_MENU_LINE];
	char				m_szItems[MAX_MENU_ITEMS][MAX_COLUMNS][MAX_MENU_LINE];
	char				m_szHeader[MAX_COLUMNS][MAX_MENU_LINE];
	float				m_fPosX;
	float				m_fPosY;
	float				m_fFirstColumnWidth;
	float				m_fSecondColumnWidth;
	unsigned char		m_nColumns;
	Interaction			m_interaction;
	unsigned char		m_nColumnCount[MAX_COLUMNS];
	GTAREF				m_panel;

	CMenu(const char* szTitle, float fX, float fY, float fFirstColumnWidth, float fSecondColumnWidth, const Interaction* pInteraction);
	CMenu() { Hide(); }

	void AddItem(NUMBER nColumn, NUMBER nRow, const char* szText);
	void SetColumnTitle(NUMBER nColumn, const char* szText);
	void Hide();
	char* GetItem(NUMBER nColumn, NUMBER nRow);
	char* GetTitle();
	char* MS(NUMBER nColumn, NUMBER nRow);
	char GetActiveRow();
	void Show();

};

SAMP_END

#elif defined(SAMP_R3)

#define MAX_MENU_ITEMS 12
#define MAX_COLUMNS 2
#define MAX_MENU_LINE 32

SAMP_BEGIN

class SAMP_API CMenu {
public:

	struct SAMP_API Interaction {

		BOOL		m_bMenu;
		BOOL		m_bRow[MAX_MENU_ITEMS];
		BOOL		m_bPadding[8 - ((MAX_MENU_ITEMS + 1) % 8)];

	};

	unsigned char	m_nId;
	char			m_szTitle[MAX_MENU_LINE];
	char			m_szItems[MAX_MENU_ITEMS][MAX_COLUMNS][MAX_MENU_LINE];
	char			m_szHeader[MAX_COLUMNS][MAX_MENU_LINE];
	float			m_fPosX;
	float			m_fPosY;
	float			m_fFirstColumnWidth;
	float			m_fSecondColumnWidth;
	unsigned char	m_nColumns;
	Interaction		m_interaction;
	unsigned char	m_nColumnCount[MAX_COLUMNS];
	GTAREF			m_panel;

	CMenu(const char* szTitle, float fX, float fY, char nColumns, float fFirstColumnWidth, float fSecondColumnWidth, const Interaction* pInteraction);
	CMenu() { Hide(); }

	void AddItem(NUMBER nColumn, NUMBER nRow, const char* szText);
	void SetColumnTitle(NUMBER nColumn, const char* szText);
	void Hide();
	char* GetItem(NUMBER nColumn, NUMBER nRow);
	char* GetTitle();
	char* MS(NUMBER nColumn, NUMBER nRow);
	char GetActiveRow();
	void Show();

};

SAMP_END

#endif
