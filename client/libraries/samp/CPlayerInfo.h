/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#include "CRemotePlayer.h"
#include <string>

SAMP_BEGIN

class CPlayerInfo {
public:

	CRemotePlayer*	m_pPlayer;
	BOOL			m_bIsNPC;
#ifndef _DEBUG
private:
	unsigned int	__aling;
public:
#endif
	std::string		m_szNick;
	int				m_nScore;
	unsigned int	m_nPing;

	CPlayerInfo(const char* szName, BOOL bIsNPC);
	~CPlayerInfo();

};

SAMP_END

#elif defined(SAMP_R3)

#include "CRemotePlayer.h"
#include <string>

SAMP_BEGIN

class SAMP_API CPlayerInfo {
public:

	CRemotePlayer*	m_pPlayer;
	int				m_nPing;
#ifndef _DEBUG
private:
	int				__aling;
public:
#endif
	std::string		m_szNick;
	int				m_nScore;
	BOOL			m_bIsNPC;

	CPlayerInfo(const char* szName, BOOL bIsNPC);
	~CPlayerInfo();

};

SAMP_END

#endif
