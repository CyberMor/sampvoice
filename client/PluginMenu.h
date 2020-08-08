/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <Windows.h>
#include <d3d9.h>

#include <imgui/imgui.h>
#include <util/Memory.hpp>
#include <util/Resource.h>
#include <util/AddressesBase.h>
#include <util/BlurEffect.h>
#include <util/Texture.h>
#include <util/Render.h>

class PluginMenu {
public:

	static constexpr const char* TitleText = "Настройки голосового чата";

	static constexpr const char* Tab1_TitleText = "Общие";

	static constexpr const char* Tab1_Desc1_TitleText = "Звук";
	static constexpr const char* Tab1_Desc1_EnableSoundText = "Включить звук";
	static constexpr const char* Tab1_Desc1_VolumeSoundText = "Громкость звука";

	static constexpr const char* Tab1_Desc2_TitleText = "Эффекты";
	static constexpr const char* Tab1_Desc2_BalancerText = "Сглаживание громкости";
	static constexpr const char* Tab1_Desc2_FilterText = "Фильтр высоких частот";

	static constexpr const char* Tab1_Desc3_TitleText = "Иконка над игроками";
	static constexpr const char* Tab1_Desc3_SpeakerIconScaleText = "Масштаб";
	static constexpr const char* Tab1_Desc3_SpeakerIconOffsetXText = "Смещение по X";
	static constexpr const char* Tab1_Desc3_SpeakerIconOffsetYText = "Смещение по Y";

	static constexpr const char* Tab1_Desc4_TitleText = "Сброс";
	static constexpr const char* Tab1_Desc4_ConfigResetText = "Сбросить все настройки";

	static constexpr const char* Tab2_TitleText = "Микрофон";

	static constexpr const char* Tab2_Desc1_TitleText = "Устройство";
	static constexpr const char* Tab2_Desc1_EnableMicroText = "Включить микрофон";
	static constexpr const char* Tab2_Desc1_MicroVolumeText = "Громкость микрофона";
	static constexpr const char* Tab2_Desc1_DeviceNameText = "Устройство ввода";
	static constexpr const char* Tab2_Desc1_CheckDeviceText = "Проверить устройство";

	static constexpr const char* Tab2_Desc2_TitleText = "Иконка микрофона";
	static constexpr const char* Tab2_Desc2_MicroIconScaleText = "Масштаб";
	static constexpr const char* Tab2_Desc2_MicroIconPositionXText = "Позиция по X";
	static constexpr const char* Tab2_Desc2_MicroIconPositionYText = "Позиция по Y";
	static constexpr const char* Tab2_Desc2_MicroIconMoveText = "Переместить";

	static constexpr const char* Tab2_Desc3_MicroNotFoundText = "Нет доступных микрофонов";

	static constexpr const char* Tab3_TitleText = "Чёрный список";

	static constexpr const char* Tab3_Desc1_TitleText = "Фильтр";
	static constexpr const char* Tab3_Desc1_InputPlaceholderText = "Введите ID или Nickname игрока...";

	static constexpr const char* Tab3_Desc2_PlayerListText = "Игроки на сервере";

	static constexpr const char* Tab3_Desc3_BlackListText = "Заблокированные игроки";

	static constexpr float	BaseMenuWidth = 0.6f * Render::BaseWidth;
	static constexpr float	BaseMenuHeight = 0.7f * Render::BaseHeight;

	static constexpr float	BaseMenuPaddingX = 20.f;
	static constexpr float	BaseMenuPaddingY = 10.f;
	static constexpr float	BaseMenuFramePaddingX = 10.f;
	static constexpr float	BaseMenuFramePaddingY = 0.5f;

	static constexpr float	BaseMenuItemSpacingX = 20.f;
	static constexpr float	BaseMenuItemSpacingY = 2.f;
	static constexpr float	BaseMenuItemInnerSpacingX = 10.f;
	static constexpr float	BaseMenuItemInnerSpacingY = 10.f;

	static constexpr float	BaseMenuRounding = 10.f;

	static constexpr float	BaseFontTitleSize = 20.f;
	static constexpr float	BaseFontTabSize = 14.f;
	static constexpr float	BaseFontDescSize = 12.f;
	static constexpr float	BaseFontSize = 10.f;

	static constexpr int	TabsCount = 3;
	static constexpr float	BaseTabPadding = 4.f;
	static constexpr float	BaseTabWidth = (BaseMenuWidth - (2 * BaseMenuPaddingX + (TabsCount - 1) * BaseTabPadding)) / TabsCount;
	static constexpr float	BaseTabHeight = BaseTabWidth / 6.f;

	static constexpr float	BlurLevelIncrement = 5.f;
	static constexpr float	BlurLevelDecrement = -5.f;

private:

	static bool initStatus;
	static bool showStatus;

	static float blurLevel;
	static float blurLevelDeviation;
	static BlurEffectPtr blurEffect;

	static TexturePtr tLogo;

	static ImFont* pTitleFont;
	static ImFont* pTabFont;
	static ImFont* pDescFont;
	static ImFont* pDefFont;

	static Memory::PatchPtr openChatFuncPatch;
	static Memory::PatchPtr openScoreboardFuncPatch;
	static Memory::PatchPtr switchModeFuncPatch;

	static int prevChatMode;

	// Configs
	// ------------------------------------------------------------------------------------------

	static bool soundEnable;
	static int soundVolume;
	static bool soundBalancer;
	static bool soundFilter;

	static float speakerIconScale;
	static int speakerIconOffsetX;
	static int speakerIconOffsetY;

	static bool microEnable;
	static int microVolume;
	static int deviceIndex;

	static float microIconScale;
	static int microIconPositionX;
	static int microIconPositionY;
	static D3DCOLOR microIconColor;
	static float microIconAngle;

	// Internal options
	// ------------------------------------------------------------------------------------------

	static int iSelectedMenu;
	static bool bCheckDevice;
	static bool bMicroMovement;
	static char nBuffer[64];

	// ------------------------------------------------------------------------------------------

	static void SyncOptions();

public:

	static bool Init(
		IDirect3DDevice9* pDevice,
		D3DPRESENT_PARAMETERS* pParameters,
		const AddressesBase& addrBase,
		const Resource& rShader,
		const Resource& rLogo,
		const Resource& rFont
	);

	static void Render();
	static void Update();

	static bool Show();
	static bool IsShowed();
	static void Hide();

	static LRESULT OnWndMessage(
		HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam
	);

	static void Free();

};
