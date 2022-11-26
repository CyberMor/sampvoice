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

struct Plugin {

    Plugin() = delete;
    ~Plugin() = delete;
    Plugin(const Plugin&) = delete;
    Plugin(Plugin&&) = delete;
    Plugin& operator=(const Plugin&) = delete;
    Plugin& operator=(Plugin&&) = delete;

public:

    static bool OnPluginLoad(HMODULE module) noexcept;
    static bool OnSampLoad(HMODULE module) noexcept;

private:

    static void OnInitGame() noexcept;
    static void OnExitGame() noexcept;

    static void MainLoop();

    static void ConnectHandler(const std::string& host, WORD port);
    static void PluginConnectHandler(SV::ConnectPacket& connect_data) noexcept;
    static bool PluginInitHandler(const SV::PluginInitPacket& plugin_data);
    static void ControlPacketHandler(const ControlPacket& control_packet);
    static void DisconnectHandler();

    static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param);

    static void OnDeviceInit(IDirect3D9* direct, IDirect3DDevice9* device, const D3DPRESENT_PARAMETERS& parameters);
    static void OnBeforeReset();
    static void OnRender();
    static void OnAfterReset(IDirect3DDevice9* device, const D3DPRESENT_PARAMETERS& parameters);
    static void OnDeviceFree();

    static void DrawRadarHook();

private:

    static HMODULE          _module;
    static AddressesBase    _addresses;
    static std::string      _sv_directory;

    static bool             _mute_status;
    static bool             _record_status;
    static bool             _record_busy;

    static std::map<DWORD, std::unique_ptr<Stream>>
                            _stream_table;
    static std::string      _blacklist_file_path;
    static bool             _game_status;

    static LONG             _orig_wnd_proc;
    static HWND             _orig_wnd_handle;

    static Memory::CallHook _draw_radar_hook;

};
