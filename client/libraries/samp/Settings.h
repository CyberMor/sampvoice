/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#define SETTINGS_STRING_LEN 256

SAMP_BEGIN

struct SAMP_API Settings {

	BOOL m_bDebugMode; // -d
	BOOL m_bOnlineGame; // -c
	BOOL m_bWindowedMode; // unused
	char m_szPass[SETTINGS_STRING_LEN + 1]; // -z
	char m_szHost[SETTINGS_STRING_LEN + 1]; // -h
	char m_szPort[SETTINGS_STRING_LEN + 1]; // -p
	char m_szNick[SETTINGS_STRING_LEN + 1]; // -n
	char m_szDebugScript[SETTINGS_STRING_LEN + 1]; // -l

	static void Initialize();
	static void GetFromCommandLine(const char* szLine, char* szString);
	static void GetFromQuotes(const char* szLine, char* szString);

};

Settings& settings();

SAMP_END

#elif defined(SAMP_R3)

#define SETTINGS_STRING_LEN 256

SAMP_BEGIN

struct SAMP_API Settings {

	BOOL m_bDebugMode; // -d
	BOOL m_bOnlineGame; // -c
	BOOL m_bWindowedMode; // unused
	char m_szPass[SETTINGS_STRING_LEN + 1]; // -z
	char m_szHost[SETTINGS_STRING_LEN + 1]; // -h
	char m_szPort[SETTINGS_STRING_LEN + 1]; // -p
	char m_szNick[SETTINGS_STRING_LEN + 1]; // -n
	char m_szDebugScript[SETTINGS_STRING_LEN + 1]; // -l

	static void Initialize();
	static void GetFromCommandLine(const char* szLine, char* szBuffer);
	static void GetFromQuotes(const char* szLine, char* szBuffer);

};

Settings& settings();

SAMP_END

#endif
