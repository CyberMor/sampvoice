/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Plugin.h"

#include <string>
#include <sstream>

static std::string libraryName = "samp.dll";

static DWORD WINAPI LibraryWaitingThread(HMODULE sampBaseAddress) {

	while (!(sampBaseAddress = GetModuleHandle(libraryName.c_str()))) Sleep(50);

	Plugin::OnSampLoad(sampBaseAddress);

	return NULL;

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved) {

	if (dwReasonForCall != DLL_PROCESS_ATTACH) return TRUE;

	if (!Plugin::OnPluginLoad(hModule)) return FALSE;

	if (auto cmdLine = GetCommandLine()) {

		std::string iString;
		std::istringstream cmdStream(cmdLine);

		while ((cmdStream >> iString) && (iString != "-svlib"));
		if (cmdStream >> iString) libraryName = iString;

	}

	return (BOOL)(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)
		(&LibraryWaitingThread), NULL, NULL, NULL));

}
