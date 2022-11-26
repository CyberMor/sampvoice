/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Plugin.h"

#include <cassert>

#include <game/CRadar.h>
#include <game/CWorld.h>
#include <game/CSprite.h>
#include <samp/CScoreboard.h>
#include <util/Path.h>
#include <util/GameUtil.h>
#include <util/ImGuiUtil.h>
#include <util/KeyFilter.h>
#include <util/RakNet.h>
#include <util/Logger.h>
#include <util/Timer.h>

#include "Record.h"
#include "Playback.h"
#include "Network.h"
#include "BlackList.h"
#include "PluginConfig.h"
#include "MicroIcon.h"
#include "SpeakerList.h"
#include "PluginMenu.h"
#include "GlobalStream.h"
#include "StreamAtPoint.h"
#include "StreamAtVehicle.h"
#include "StreamAtPlayer.h"
#include "StreamAtObject.h"
#include "Header.h"

#define FontResource             IDR_FONT1, RT_FONT
#define LogoIconResource         IDB_PNG1,  "PNG"
#define BlurShaderResource       IDR_HLSL1, "HLSL"
#define PassiveMicroIconResource IDB_PNG4,  "PNG"
#define ActiveMicroIconResource  IDB_PNG2,  "PNG"
#define MutedMicroIconResource   IDB_PNG3,  "PNG"
#define SpeakerIconResource      IDB_PNG5,  "PNG"

bool Plugin::OnPluginLoad(const HMODULE module) noexcept
{
    if ((_module = module) == nullptr)
        return false;

    _sv_directory = GameUtil::GetSampvoiceDirectory();
    if (_sv_directory.empty()) return false;

    if (!Logger::Init((_sv_directory + '\\' + SV::kLogFileName).c_str()))
        return false;

    if (!Render::Init())
    {
        Logger::LogToFile("[sv:err:plugin] : failed to init render module");
        Logger::Free();
        return false;
    }

    Render::SetDeviceInitCallback(OnDeviceInit);
    Render::SetBeforeResetCallback(OnBeforeReset);
    Render::SetRenderCallback(OnRender);
    Render::SetAfterResetCallback(OnAfterReset);
    Render::SetDeviceFreeCallback(OnDeviceFree);

    return true;
}

bool Plugin::OnSampLoad(const HMODULE module) noexcept
{
    _addresses = { reinterpret_cast<DWORD>(module) };

    if (!PluginConfig::Load(_sv_directory + '\\' + SV::kConfigFileName))
    {
        Logger::LogToFile("[sv:err:plugin] : failed to load configs");
    }

    if (!Samp::Init(_addresses))
    {
        Logger::LogToFile("[sv:err:plugin] : failed to init samp");
        Render::Free();
        Logger::Free();
        return false;
    }

    Samp::SetLoadCallback(OnInitGame);
    Samp::SetExitCallback(OnExitGame);

    if (!Network::Init(_addresses))
    {
        Logger::LogToFile("[sv:err:plugin] : failed to init network");
        Samp::Free();
        Render::Free();
        Logger::Free();
        return false;
    }

    Network::SetConnectCallback(ConnectHandler);
    Network::SetSvConnectCallback(PluginConnectHandler);
    Network::SetSvInitCallback(PluginInitHandler);
    Network::SetDisconnectCallback(DisconnectHandler);

    if (!Playback::Init(_addresses))
    {
        Logger::LogToFile("[sv:err:plugin] : failed to init playback");
        Network::Free();
        Samp::Free();
        Render::Free();
        Logger::Free();
        return false;
    }

    return true;
}

void Plugin::OnInitGame() noexcept
{
    _draw_radar_hook = { reinterpret_cast<LPVOID>(0x58FC53), &DrawRadarHook };

    GameUtil::DisableAntiCheat(_addresses.GetBaseAddr());

    SpeakerList::Show();
    MicroIcon::Show();
}

void Plugin::OnExitGame() noexcept
{
    Network::Free();

    _stream_table.clear();

    Record::Free();
    Playback::Free();

    PluginConfig::Save(_sv_directory + '\\' + SV::kConfigFileName);
    BlackList::Free();

    Render::Free();
    Logger::Free();
}

void Plugin::MainLoop()
{
    if (Samp::IsLoaded())
    {
        if (_game_status && !GameUtil::IsGameActive())
        {
            Logger::LogToFile("[sv:dbg:plugin] : game paused");

            for (const auto& stream : _stream_table)
                stream.second->Reset();

            KeyFilter::ReleaseAllKeys();

            _game_status = false;
        }
        else if (!_game_status && GameUtil::IsGameActive())
        {
            Logger::LogToFile("[sv:dbg:plugin] : game resumed");

            for (const auto& stream : _stream_table)
                stream.second->Reset();

            _game_status = true;
        }

        for (ControlPacketContainer control_packet; Network::ReceiveControlPacket(control_packet);)
        {
            ControlPacketHandler(*control_packet);
        }

        for (VoicePacketContainer voice_packet; Network::ReceiveVoicePacket(voice_packet);)
        {
            if (!BlackList::IsPlayerBlocked(voice_packet->sender))
            {
                if (const auto iter  = _stream_table.find(voice_packet->stream);
                               iter != _stream_table.end())
                {
                    iter->second->Push(*voice_packet);
                }
            }
        }

        for (const auto& stream : _stream_table)
        {
            stream.second->Tick();
        }

        Playback::Tick();
        Record::Tick();

        for (KeyEvent key_event; KeyFilter::PopEvent(key_event))
        {
            if (key_event.is_pressed)
            {
                Logger::LogToFile("[sv:dbg:plugin] : activation key(%hhx) pressed", key_event.key_id);

                if (Record::GetMicroEnable())
                {
                    if (!_record_busy && !_record_status && key_event.active_keys == 1)
                    {
                        if (!_mute_status)
                        {
                            _record_status = true;
                            MicroIcon::SwitchToActiveIcon();
                            Record::StartRecording();
                        }
                        else
                        {
                            MicroIcon::SwitchToMutedIcon();
                        }
                    }
                    if (!_mute_status)
                    {
                        SV::PressKeyPacket press_key_packet {};

                        press_key_packet.keyId = key_event.key_id;

                        if (!Network::SendControlPacket(SV::ControlPacketType::pressKey, &press_key_packet, sizeof(press_key_packet)))
                            Logger::LogToFile("[sv:err:main:HookWndProc] : failed to send PressKey packet");
                    }
                }
            }
            else
            {
                Logger::LogToFile("[sv:dbg:plugin] : activation key(%hhx) released", key_event.key_id);

                if (Record::GetMicroEnable())
                {
                    if (!_record_busy && _record_status && !key_event.active_keys)
                    {
                        MicroIcon::SwitchToPassiveIcon();
                        _record_status = false;
                    }
                    if (!_mute_status)
                    {
                        SV::ReleaseKeyPacket release_key_packet {};

                        release_key_packet.keyId = key_event.key_id;

                        if (!Network::SendControlPacket(SV::ControlPacketType::releaseKey, &release_key_packet, sizeof(release_key_packet)))
                            Logger::LogToFile("[sv:err:main:HookWndProc] : failed to send ReleaseKey packet");
                    }
                }
            }
        }

        BYTE frame_buffer[Network::kMaxVoiceDataSize];

        if (const auto frame_size = Record::GetFrame(frame_buffer, sizeof(frame_buffer)))
        {
            if (!Network::SendVoicePacket(frame_buffer, frame_size))
                Logger::LogToFile("[sv:err:plugin] : failed to send voice packet");

            if (!_record_status)
            {
                Record::StopRecording();
                Network::EndSequence();
            }
        }
    }
}

void Plugin::ConnectHandler(const std::string& host, const WORD port)
{
    _blacklist_file_path = _sv_directory + "\\" "svblacklist_"
        + host + "_" + std::to_string(port) + ".txt";

    if (!BlackList::Load(_blacklist_file_path))
        Logger::LogToFile("[sv:err:plugin] : failed to open blacklist file");
    if (!BlackList::Init(_addresses))
        Logger::LogToFile("[sv:err:plugin] : failed to init blacklist");
}

void Plugin::PluginConnectHandler(SV::ConnectPacket& connect_data) noexcept
{
    connect_data.signature = SV::kSignature;
    connect_data.version = SV::kVersion;
    connect_data.micro = Record::HasMicro();
}

bool Plugin::PluginInitHandler(const SV::PluginInitPacket& plugin_data)
{
    _mute_status = plugin_data.mute;

    if (!Record::Init(plugin_data.bitrate))
    {
        Logger::LogToFile("[sv:inf:plugin:packet:init] : failed init record");
    }

    return true;
}

void Plugin::ControlPacketHandler(const ControlPacket& control_packet)
{
    switch (control_packet.packet)
    {
        case SV::ControlPacketType::muteEnable:
        {
            if (control_packet.length != 0) break;

            Logger::LogToFile("[sv:dbg:plugin:muteenable]");

            _mute_status   = true;
            _record_status = false;
            _record_busy   = false;

            KeyFilter::ReleaseAllKeys();
        }
        break;
        case SV::ControlPacketType::muteDisable:
        {
            if (control_packet.length != 0) break;

            Logger::LogToFile("[sv:dbg:plugin:mutedisable]");

            _mute_status = false;
        }
        break;
        case SV::ControlPacketType::startRecord:
        {
            if (control_packet.length != 0) break;

            Logger::LogToFile("[sv:dbg:plugin:startrecord]");

            if (_mute_status) break;

            _record_busy   = true;
            _record_status = true;

            Record::StartRecording();
        }
        break;
        case SV::ControlPacketType::stopRecord:
        {
            if (control_packet.length != 0) break;

            Logger::LogToFile("[sv:dbg:plugin:stoprecord]");

            if (_mute_status) break;

            _record_status = false;
            _record_busy   = false;
        }
        break;
        case SV::ControlPacketType::addKey:
        {
            const auto& content = *reinterpret_cast<const SV::AddKeyPacket*>(control_packet.data);
            if (control_packet.length != sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:addkey] : keyid(0x%hhx)", content.keyId);

            KeyFilter::AddKey(content.keyId);
        }
        break;
        case SV::ControlPacketType::removeKey:
        {
            const auto& content = *reinterpret_cast<const SV::RemoveKeyPacket*>(control_packet.data);
            if (control_packet.length != sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:removekey] : keyid(0x%hhx)", content.keyId);

            KeyFilter::RemoveKey(content.keyId);
        }
        break;
        case SV::ControlPacketType::removeAllKeys:
        {
            if (control_packet.length) break;

            Logger::LogToFile("[sv:dbg:plugin:removeallkeys]");

            KeyFilter::RemoveAllKeys();
        }
        break;
        case SV::ControlPacketType::createGStream:
        {
            const auto& content = *reinterpret_cast<const SV::CreateGStreamPacket*>(control_packet.data);
            if (control_packet.length < sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:creategstream] : stream(%p), color(0x%x), name(%s)",
                content.stream, content.color, content.color ? content.name : "");

            const auto& streamPtr = _stream_table[content.stream] =
                GlobalStream(content.color, content.name);

            streamPtr->AddPlayCallback(SpeakerList::OnSpeakerPlay);
            streamPtr->AddStopCallback(SpeakerList::OnSpeakerStop);
        }
        break;
        case SV::ControlPacketType::createLPStream:
        {
            const auto& content = *reinterpret_cast<const SV::CreateLPStreamPacket*>(control_packet.data);
            if (control_packet.length < sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:createlpstream] : "
                "stream(%p), dist(%.2f), pos(%.2f;%.2f;%.2f), color(0x%x), name(%s)",
                content.stream, content.distance, content.position.x, content.position.y, content.position.z,
                content.color, content.color ? content.name : "");

            const auto& streamPtr = _stream_table[content.stream] =
                MakeStreamAtPoint(content.color, content.name, content.distance, content.position);

            streamPtr->AddPlayCallback(SpeakerList::OnSpeakerPlay);
            streamPtr->AddStopCallback(SpeakerList::OnSpeakerStop);
        }
        break;
        case SV::ControlPacketType::createLStreamAtVehicle:
        {
            const auto& content = *reinterpret_cast<const SV::CreateLStreamAtPacket*>(control_packet.data);
            if (control_packet.length < sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:createlstreamatvehicle] : "
                "stream(%p), dist(%.2f), vehicle(%hu), color(0x%x), name(%s)",
                content.stream, content.distance, content.target,
                content.color, content.color ? content.name : "");

            const auto& streamPtr = _stream_table[content.stream] =
                MakeStreamAtVehicle(content.color, content.name, content.distance, content.target);

            streamPtr->AddPlayCallback(SpeakerList::OnSpeakerPlay);
            streamPtr->AddStopCallback(SpeakerList::OnSpeakerStop);
        }
        break;
        case SV::ControlPacketType::createLStreamAtPlayer:
        {
            const auto& content = *reinterpret_cast<const SV::CreateLStreamAtPacket*>(control_packet.data);
            if (control_packet.length < sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:createlstreamatplayer] : "
                "stream(%p), dist(%.2f), player(%hu), color(0x%x), name(%s)",
                content.stream, content.distance, content.target,
                content.color, content.color ? content.name : "");

            const auto& streamPtr = _stream_table[content.stream] =
                MakeStreamAtPlayer(content.color, content.name, content.distance, content.target);

            streamPtr->AddPlayCallback(SpeakerList::OnSpeakerPlay);
            streamPtr->AddStopCallback(SpeakerList::OnSpeakerStop);
        }
        break;
        case SV::ControlPacketType::createLStreamAtObject:
        {
            const auto& content = *reinterpret_cast<const SV::CreateLStreamAtPacket*>(control_packet.data);
            if (control_packet.length < sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:createlstreamatobject] : "
                "stream(%p), dist(%.2f), object(%hu), color(0x%x), name(%s)",
                content.stream, content.distance, content.target,
                content.color, content.color ? content.name : "");

            const auto& streamPtr = _stream_table[content.stream] =
                MakeStreamAtObject(content.color, content.name, content.distance, content.target);

            streamPtr->AddPlayCallback(SpeakerList::OnSpeakerPlay);
            streamPtr->AddStopCallback(SpeakerList::OnSpeakerStop);
        }
        break;
        case SV::ControlPacketType::updateLStreamDistance:
        {
            const auto& content = *reinterpret_cast<const SV::UpdateLStreamDistancePacket*>(control_packet.data);
            if (control_packet.length != sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:updatelpstreamdistance] : stream(%p), dist(%.2f)",
                content.stream, content.distance);

            const auto iter = _stream_table.find(content.stream);
            if (iter == _stream_table.end()) break;

            static_cast<LocalStream*>(iter->second.get())->SetDistance(content.distance);
        }
        break;
        case SV::ControlPacketType::updateLPStreamPosition:
        {
            const auto& content = *reinterpret_cast<const SV::UpdateLPStreamPositionPacket*>(control_packet.data);
            if (control_packet.length != sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:updatelpstreamcoords] : stream(%p), pos(%.2f;%.2f;%.2f)",
                content.stream, content.position.x, content.position.y, content.position.z);

            const auto iter = _stream_table.find(content.stream);
            if (iter == _stream_table.end()) break;

            static_cast<StreamAtPoint*>(iter->second.get())->SetPosition(content.position);
        }
        break;
        case SV::ControlPacketType::deleteStream:
        {
            const auto& content = *reinterpret_cast<const SV::DeleteStreamPacket*>(control_packet.data);
            if (control_packet.length != sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:deletestream] : stream(%p)", content.stream);

            _stream_table.erase(content.stream);
        }
        break;
        case SV::ControlPacketType::setStreamParameter:
        {
            const auto& content = *reinterpret_cast<const SV::SetStreamParameterPacket*>(control_packet.data);
            if (control_packet.length != sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:streamsetparameter] : stream(%p), parameter(%hhu), value(%.2f)",
                content.stream, content.parameter, content.value);

            const auto iter = _stream_table.find(content.stream);
            if (iter == _stream_table.end()) break;

            iter->second->SetParameter(content.parameter, content.value);
        }
        break;
        case SV::ControlPacketType::slideStreamParameter:
        {
            const auto& content = *reinterpret_cast<const SV::SlideStreamParameterPacket*>(control_packet.data);
            if (control_packet.length != sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:streamslideparameter] : "
                "stream(%p), parameter(%hhu), startvalue(%.2f), endvalue(%.2f), time(%u)",
                content.stream, content.parameter, content.startvalue, content.endvalue, content.time);

            const auto iter = _stream_table.find(content.stream);
            if (iter == _stream_table.end()) break;

            iter->second->SlideParameter(content.parameter, content.startvalue, content.endvalue, content.time);
        }
        break;
        case SV::ControlPacketType::createEffect:
        {
            const auto& content = *reinterpret_cast<const SV::CreateEffectPacket*>(control_packet.data);
            if (control_packet.length < sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:effectcreate] : "
                "stream(%p), effect(%p), number(%hhu), priority(%d)",
                content.stream, content.effect, content.number, content.priority);

            const auto iter = _stream_table.find(content.stream);
            if (iter == _stream_table.end()) break;

            iter->second->EffectCreate(content.effect, content.number, content.priority,
                content.params, control_packet.length - sizeof(content));
        }
        break;
        case SV::ControlPacketType::deleteEffect:
        {
            const auto& content = *reinterpret_cast<const SV::DeleteEffectPacket*>(control_packet.data);
            if (control_packet.length != sizeof(content)) break;

            Logger::LogToFile("[sv:dbg:plugin:effectdelete] : stream(%p), effect(%p)",
                content.stream, content.effect);

            const auto iter = _stream_table.find(content.stream);
            if (iter == _stream_table.end()) break;

            iter->second->EffectDelete(content.effect);
        }
        break;
    }
}

void Plugin::DisconnectHandler()
{
    _stream_table.clear();

    _mute_status = false;
    _record_status = false;
    _record_busy = false;

    if (!BlackList::Save(_blacklist_file_path))
    {
        Logger::LogToFile("[sv:err:plugin] : failed to save blacklist file");
    }

    BlackList::Free();
    Record::Free();
}

LRESULT CALLBACK Plugin::WindowProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
    if (PluginMenu::WindowProc(hWnd, uMsg, wParam, lParam))
        return TRUE;

    switch (uMsg)
    {
        case WM_KEYDOWN: KeyFilter::PushPressEvent(static_cast<BYTE>(wParam)); break;
        case WM_KEYUP: KeyFilter::PushReleaseEvent(static_cast<BYTE>(wParam)); break;
    }

    return CallWindowProc(reinterpret_cast<WNDPROC>(_orig_wnd_proc), hWnd, uMsg, wParam, lParam);
}

void Plugin::OnDeviceInit(IDirect3D9* const, IDirect3DDevice9* const device, const D3DPRESENT_PARAMETERS& parameters)
{
    assert(device != nullptr);

    MicroIcon::Init(device,
        { _module, PassiveMicroIconResource },
        { _module, ActiveMicroIconResource },
        { _module, MutedMicroIconResource });

    ImGuiUtil::Init(device);

    SpeakerList::Init(device, _addresses,
        { _module, SpeakerIconResource },
        { _module, FontResource });

    PluginMenu::Init(device, _addresses,
        { _module, BlurShaderResource },
        { _module, LogoIconResource },
        { _module, FontResource });

    _orig_wnd_handle = parameters.hDeviceWindow;
    _orig_wnd_proc = GetWindowLong(_orig_wnd_handle, GWL_WNDPROC);
    SetWindowLong(_orig_wnd_handle, GWL_WNDPROC, reinterpret_cast<LONG>(&WindowProc));

    _game_status = GameUtil::IsGameActive();
}

void Plugin::OnBeforeReset()
{
    PluginMenu::Free();
    SpeakerList::Free();
    ImGuiUtil::Free();
    MicroIcon::Free();
}

void Plugin::OnRender()
{
    Timer::Tick();
    MainLoop();
    MicroIcon::Update();
    PluginMenu::Update();
    PluginMenu::Render();
}

void Plugin::OnAfterReset(IDirect3DDevice9* const device, const D3DPRESENT_PARAMETERS&)
{
    assert(device != nullptr);

    MicroIcon::Init(device,
        { _module, PassiveMicroIconResource },
        { _module, ActiveMicroIconResource },
        { _module, MutedMicroIconResource });

    ImGuiUtil::Init(device);

    SpeakerList::Init(device, _addresses,
        { _module, SpeakerIconResource },
        { _module, FontResource });

    PluginMenu::Init(device, _addresses,
        { _module, BlurShaderResource },
        { _module, LogoIconResource },
        { _module, FontResource });

    _game_status = GameUtil::IsGameActive();
}

void Plugin::OnDeviceFree()
{
    SetWindowLong(_orig_wnd_handle, GWL_WNDPROC, _orig_wnd_proc);

    PluginMenu::Free();
    SpeakerList::Free();
    ImGuiUtil::Free();
    MicroIcon::Free();
}

void Plugin::DrawRadarHook()
{
    static_cast<void(*)()>(_draw_radar_hook->GetCallFuncAddr())();

    SpeakerList::Render();
    MicroIcon::Render();
}

HMODULE       Plugin::_module = NULL;
AddressesBase Plugin::_addresses;
std::string   Plugin::_sv_directory;

bool Plugin::_mute_status = false;
bool Plugin::_record_status = false;
bool Plugin::_record_busy = false;

std::map<DWORD, std::unique_ptr<Stream>>
            Plugin::_stream_table;
std::string Plugin::_blacklist_file_path;
bool        Plugin::_game_status = false;

LONG Plugin::_orig_wnd_proc = NULL;
HWND Plugin::_orig_wnd_handle = NULL;

Memory::CallHook Plugin::_draw_radar_hook;
