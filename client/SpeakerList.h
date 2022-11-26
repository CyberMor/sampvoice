/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <list>
#include <array>
#include <unordered_map>

#include <Windows.h>
#include <d3d9.h>

#include <imgui/imgui.h>
#include <samp/CNetGame.h>
#include <util/AddressesBase.h>
#include <util/Resource.h>
#include <util/Texture.h>

#include "Stream.h"

struct SpeakerList {

    SpeakerList() = delete;
    ~SpeakerList() = delete;
    SpeakerList(const SpeakerList&) = delete;
    SpeakerList(SpeakerList&&) = delete;
    SpeakerList& operator=(const SpeakerList&) = delete;
    SpeakerList& operator=(SpeakerList&&) = delete;

private:

    static constexpr int   kBaseLinesCount = 12;
    static constexpr float kBaseLeftIndent = 37.f;
    static constexpr float kBaseIconSize   = 36.f;
    static constexpr float kBaseFontSize   = 7.5f;

public:

    static bool Init(IDirect3DDevice9* device, const Resource& resource_speaker_icon,
        const Resource& resource_speaker_font) noexcept;
    static void Free() noexcept;

    static void Show() noexcept;
    static bool IsShowed() noexcept;
    static void Hide() noexcept;

    static void Render();

    static int GetSpeakerIconOffsetX() noexcept;
    static int GetSpeakerIconOffsetY() noexcept;
    static float GetSpeakerIconScale() noexcept;

    static void SetSpeakerIconOffsetX(int speaker_icon_offset_x) noexcept;
    static void SetSpeakerIconOffsetY(int speaker_icon_offset_y) noexcept;
    static void SetSpeakerIconScale(float speaker_icon_scale) noexcept;

    static void SyncConfigs() noexcept;
    static void ResetConfigs() noexcept;

public:

    static void OnSpeakerPlay(const Stream& stream, WORD speaker) noexcept;
    static void OnSpeakerStop(const Stream& stream, WORD speaker) noexcept;

private:

    static bool    _init_status;
    static bool    _show_status;

    static ImFont* _speaker_font;
    static Texture _speaker_icon;

    static std::array<std::unordered_map<Stream*, StreamInfo>, MAX_PLAYERS>
                   _player_streams;

};
