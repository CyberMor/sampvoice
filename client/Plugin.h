/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <thread>
#include <queue>
#include <map>

#include <WinSock2.h>
#include <Windows.h>
#include <d3d9.h>

#include <raknet/bitstream.h>
#include <raknet/rakclient.h>
#include <util/AddressesBase.h>

#include "resources/resource.h"

#include "ControlPacket.h"
#include "Stream.h"

class Plugin {

    Plugin() = delete;
    ~Plugin() = delete;
    Plugin(const Plugin&) = delete;
    Plugin(Plugin&&) = delete;
    Plugin& operator=(const Plugin&) = delete;
    Plugin& operator=(Plugin&&) = delete;

public:

    static bool OnPluginLoad(HMODULE hModule);
    static bool OnSampLoad(HMODULE hModule);

private:

    static void OnDeviceInit(IDirect3D9* pDirect, IDirect3DDevice9* pDevice,
                             D3DPRESENT_PARAMETERS* pParameters);
    static void OnBeforeReset();
    static void OnRender();
    static void OnAfterReset(IDirect3DDevice9* pDevice,
                             D3DPRESENT_PARAMETERS* pParameters);
    static void OnDeviceFree();

    static LRESULT CALLBACK OnWndMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    static void ConnectHandler(const std::string& serverIp, WORD serverPort);
    static void PluginConnectHandler(SV::ConnectPacket& connectStruct);
    static bool PluginInitHandler(const SV::PluginInitPacket& initPacket);
    static void ControlPacketHandler(const ControlPacket& controlPacket);
    static void DisconnectHandler();

    static void MainLoop();

    static void DrawRadarHook();

    static void OnInitGame();
    static void OnExitGame();

private:

    static HMODULE pModuleHandle;
    static AddressesBasePtr pAddresses;

    static bool muteStatus;
    static bool recordStatus;
    static bool recordBusy;

    static std::map<DWORD, StreamPtr> streamTable;
    static std::string blacklistFilePath;
    static bool gameStatus;

    static LONG origWndProc;
    static HWND origWndHandle;

    static IDirect3D9* pDirect;
    static IDirect3DDevice9* pDevice;
    static D3DPRESENT_PARAMETERS parameters;

    static Memory::CallHookPtr drawRadarHook;

};
