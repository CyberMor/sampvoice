/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Plugin.h"

#include <string>
#include <sstream>

static std::string libraryName { "samp.dll" };

static DWORD WINAPI LibraryWaitingThread(LPVOID)
{
    HMODULE sampBaseAddress { nullptr };

    while ((sampBaseAddress = GetModuleHandle(libraryName.c_str())) == nullptr)
        SleepForMilliseconds(50);

    Plugin::OnSampLoad(sampBaseAddress);

    return NULL;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID)
{
    if (dwReasonForCall != DLL_PROCESS_ATTACH)
        return TRUE;

    if (!Plugin::OnPluginLoad(hModule))
        return FALSE;

    if (const auto cmdLine = GetCommandLine(); cmdLine != nullptr)
    {
        std::istringstream cmdStream { cmdLine };
        std::string iString;

        while (cmdStream >> iString && iString != "-svlib");
        if (cmdStream >> iString) libraryName = std::move(iString);
    }

    const auto waitingThread = CreateThread(NULL, 0,
        LibraryWaitingThread, NULL, NULL, NULL);

    return waitingThread != nullptr ? TRUE : FALSE;
}
