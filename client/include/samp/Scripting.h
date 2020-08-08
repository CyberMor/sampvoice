/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"

#if defined(SAMP_R1)

class CRunningScript;

SAMP_BEGIN_NP

namespace Scripting {

	struct SAMP_API OpcodeInfo {

		unsigned short	m_wOpCode;
		char			m_szParams[16]; // i - int, f - float, v - var, s - string, z - zero terminating

	};

	typedef int(__thiscall* PROCESSOR)(CRunningScript*);

	CRunningScript*&	pThread();
	unsigned char*		aBuffer(); // [256]
	unsigned long&		nLastUsedOpcode();
	unsigned long*&		apThreadLocals(); // [18]
	unsigned int&		bLocalDebug();
	PROCESSOR&			ProcessOneCommand();

	void Initialize();
	int ExecBuffer();
	template<typename... Args>
	int ProcessCommand(const OpcodeInfo* pOpcodeInfo, Args... args) {
		return ((int(__cdecl*)(const OpcodeInfo*, Args...))SAMP_ADDROF(0xABCD0))(pOpcodeInfo, args...);
	}

}

SAMP_END_NP

#elif defined(SAMP_R3)

class CRunningScript;

SAMP_BEGIN_NP

namespace Scripting {

	struct SAMP_API OpcodeInfo {

		unsigned short	m_wOpCode;
		char			m_szParams[16]; // i - int, f - float, v - var, s - string, z - zero terminating

	};

	typedef int(__thiscall* PROCESSOR)(CRunningScript*);

	CRunningScript*&	pThread();
	unsigned char*		aBuffer(); // [256]
	unsigned long&		nLastUsedOpcode();
	unsigned long*&		apThreadLocals(); // [18]
	BOOL&				bLocalDebug();
	PROCESSOR&			ProcessOneCommand();

	void Initialize();
	int ExecBuffer();
	template<typename... Args>
	int ProcessCommand(const OpcodeInfo* pOpcodeInfo, Args... args) {
		return ((int(__cdecl*)(const OpcodeInfo*, Args...))SAMP_ADDROF(0xB1A80))(pOpcodeInfo, args...);
	}

}

SAMP_END_NP

#endif
