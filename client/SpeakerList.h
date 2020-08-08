/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <list>
#include <array>

#include <d3d9.h>

#include <imgui/imgui.h>
#include <samp/CNetGame.h>
#include <samp/CPlayerTags.h>

#include <util/Memory.hpp>
#include <util/AddressesBase.h>
#include <util/Resource.h>
#include <util/Texture.h>

#include "Stream.h"

class SpeakerList {
public:

	static constexpr int BaseLinesCount = 12;
	static constexpr float BaseLeftIndent = 37.f;
	static constexpr float BaseIconSize = 36.f;
	static constexpr float BaseFontSize = 7.5f;

private:

	static bool initStatus;
	static bool showStatus;

	static ImFont* pSpeakerFont;
	static TexturePtr tSpeakerIcon;

	static std::array<std::list<StreamInfoPtr>, MAX_PLAYERS> playerStreams;

public:

	static void OnSpeakerPlay(const Stream& stream, const WORD speaker);
	static void OnSpeakerStop(const Stream& stream, const WORD speaker);

public:

	static bool Init(
		IDirect3DDevice9* pDevice,
		const AddressesBase& addrBase,
		const Resource& rSpeakerIcon,
		const Resource& rSpeakerFont
	);

	static int GetSpeakerIconOffsetX();
	static int GetSpeakerIconOffsetY();
	static float GetSpeakerIconScale();

	static void SetSpeakerIconOffsetX(int speakerIconOffsetX);
	static void SetSpeakerIconOffsetY(int speakerIconOffsetY);
	static void SetSpeakerIconScale(float speakerIconScale);

	static void SyncConfigs();
	static void ResetConfigs();

	static void Render();

	static void Show();
	static bool IsShowed();
	static void Hide();

	static void Free();

};
