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

class SpeakerList {

    SpeakerList() = delete;
    ~SpeakerList() = delete;
    SpeakerList(const SpeakerList&) = delete;
    SpeakerList(SpeakerList&&) = delete;
    SpeakerList& operator=(const SpeakerList&) = delete;
    SpeakerList& operator=(SpeakerList&&) = delete;

private:

    static constexpr int kBaseLinesCount = 12;
    static constexpr float kBaseLeftIndent = 37.f;
    static constexpr float kBaseIconSize = 36.f;
    static constexpr float kBaseFontSize = 7.5f;

public:

    static bool Init(IDirect3DDevice9* pDevice, const AddressesBase& addrBase,
        const Resource& rSpeakerIcon, const Resource& rSpeakerFont) noexcept;
    static void Free() noexcept;

    static void Show() noexcept;
    static bool IsShowed() noexcept;
    static void Hide() noexcept;

    static void Render();

    static int GetSpeakerIconOffsetX() noexcept;
    static int GetSpeakerIconOffsetY() noexcept;
    static float GetSpeakerIconScale() noexcept;

    static void SetSpeakerIconOffsetX(int speakerIconOffsetX) noexcept;
    static void SetSpeakerIconOffsetY(int speakerIconOffsetY) noexcept;
    static void SetSpeakerIconScale(float speakerIconScale) noexcept;

    static void SyncConfigs() noexcept;
    static void ResetConfigs() noexcept;

public:

    static void OnSpeakerPlay(const Stream& stream, WORD speaker) noexcept;
    static void OnSpeakerStop(const Stream& stream, WORD speaker) noexcept;

private:

    static bool initStatus;
    static bool showStatus;

    static ImFont* pSpeakerFont;
    static TexturePtr tSpeakerIcon;

    static std::array<std::unordered_map<Stream*, StreamInfo>, MAX_PLAYERS> playerStreams;

};
