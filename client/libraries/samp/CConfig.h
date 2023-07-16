/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

#define MAX_CONFIG_ENTRIES 512
#define MAX_CONFIG_ENTRY_NAME 40

SAMP_BEGIN

enum ValueType {

	VALUE_TYPE_NONE,
	VALUE_TYPE_INT,
	VALUE_TYPE_STRING,
	VALUE_TYPE_FLOAT

};

class SAMP_API CConfig {
public:

	struct SAMP_API Entry {

		char	m_szName[MAX_CONFIG_ENTRY_NAME + 1];
		BOOL	m_bReadOnly; // maybe
		int		m_nType;
		int		m_nValue;
		float	m_fValue;
		char*	m_szValue;

	}			m_entry[MAX_CONFIG_ENTRIES];

	BOOL		m_bNotEmpty[MAX_CONFIG_ENTRIES]; // map
	char		m_szFilename[261];
	int			m_nFirstFree;

	CConfig(const char* szFile);
	~CConfig();

	void FindFirstFree();
	int GetIndex(const char* szEntry);
	bool DoesExist(const char* szEntry);
	int CreateEntry(const char* szName);
	int GetIntValue(const char* szEntry);
	const char* GetStringValue(const char* szEntry);
	float GetFloatValue(const char* szEntry);
	BOOL Free(const char* szEntry);
	int GetValueType(const char* szEntry);
	Entry* GetEntry(int nIndex);
	int GetType(const char* szString);
	BOOL Save();
	BOOL WriteIntValue(const char* szEntry, int nValue, BOOL bReadOnly = 0);
	BOOL WriteStringValue(const char* szEntry, const char* szValue, BOOL bReadOnly = 0);
	BOOL WriteFloatValue(const char* szEntry, float fValue, BOOL bReadOnly = 0);
	void Write(const char* szEntry, char* szBuffer);
	BOOL Load();

};

CConfig*& pConfig();

SAMP_END

#elif defined(SAMP_R3)

#define MAX_CONFIG_ENTRIES 512
#define MAX_CONFIG_ENTRY_NAME 40

SAMP_BEGIN

enum ValueType {

	VALUE_TYPE_NONE,
	VALUE_TYPE_INT,
	VALUE_TYPE_STRING,
	VALUE_TYPE_FLOAT

};

class SAMP_API CConfig {
public:

	struct SAMP_API Entry {

		char	m_szName[MAX_CONFIG_ENTRY_NAME + 1];
		BOOL	m_bReadOnly; // maybe
		int		m_nType;
		int		m_nValue;
		float	m_fValue;
		char* m_szValue;

	}			m_entry[MAX_CONFIG_ENTRIES];

	BOOL		m_bNotEmpty[MAX_CONFIG_ENTRIES]; // map
	char		m_szFilename[261];
	int			m_nFirstFree;

	CConfig(const char* szFile);
	~CConfig();

	void FindFirstFree();
	int GetIndex(const char* szEntry);
	bool DoesExist(const char* szEntry);
	int CreateEntry(const char* szName);
	int GetIntValue(const char* szEntry);
	const char* GetStringValue(const char* szEntry);
	float GetFloatValue(const char* szEntry);
	BOOL Free(const char* szEntry);
	int GetValueType(const char* szEntry);
	Entry* GetEntry(int nIndex);
	int GetType(const char* szString);
	BOOL Save();
	BOOL WriteIntValue(const char* szEntry, int nValue, BOOL bReadOnly = 0);
	BOOL WriteStringValue(const char* szEntry, const char* szValue, BOOL bReadOnly = 0);
	BOOL WriteFloatValue(const char* szEntry, float fValue, BOOL bReadOnly = 0);
	void Write(const char* szEntry, char* szBuffer);
	BOOL Load();

};

CConfig*& pConfig();

SAMP_END

#endif
