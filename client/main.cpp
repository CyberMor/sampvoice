/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Plugin.h"

#include <string>
#include <sstream>

static std::string gLibraryName = "samp.dll";

static DWORD WINAPI LibraryWaitingThread(const LPVOID)
{
    HMODULE samp_base_address;

    while ((samp_base_address = GetModuleHandle(gLibraryName.c_str())) == nullptr)
        SleepForMilliseconds(50);

    Plugin::OnSampLoad(samp_base_address);

    return NULL;
}

BOOL APIENTRY DllMain(const HMODULE module, const DWORD reason, const LPVOID)
{
    if (reason != DLL_PROCESS_ATTACH)
        return TRUE;

    if (!Plugin::OnPluginLoad(module))
        return FALSE;

    if (const auto cmd_line = GetCommandLine(); cmd_line != nullptr)
    {
        std::istringstream cmd_stream { cmd_line };
        std::string        cmd_word;

        while (cmd_stream >> cmd_word && cmd_word != "-svlib");
        if (cmd_stream >> cmd_word) gLibraryName = std::move(cmd_word);
    }

    return CreateThread(NULL, 0, LibraryWaitingThread, NULL, NULL, NULL)
        != nullptr ? TRUE : FALSE;
}
