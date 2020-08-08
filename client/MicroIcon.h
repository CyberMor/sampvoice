/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <d3d9.h>

#include <util/Resource.h>
#include <util/Texture.h>

class MicroIcon {
public:

	static constexpr float BaseIconSize = 35.f;
	static constexpr float BaseIconPadding = 15.f;

	static constexpr int AlphaLevelIncrementDeviation = 12;
	static constexpr int AlphaLevelDecrementDeviation = -12;

private:

	static bool initStatus;
	static bool showStatus;

	static int alphaLevelPassiveIcon;
	static int alphaLevelDeviationPassiveIcon;
	static TexturePtr tPassiveIcon;

	static int alphaLevelActiveIcon;
	static int alphaLevelDeviationActiveIcon;
	static TexturePtr tActiveIcon;

	static int alphaLevelMutedIcon;
	static int alphaLevelDeviationMutedIcon;
	static TexturePtr tMutedIcon;

public:

	static bool Init(
		IDirect3DDevice9* pDevice,
		const Resource& rPassiveIcon,
		const Resource& rActiveIcon,
		const Resource& rMutedIcon
	);

	static int GetMicroIconPositionX();
	static int GetMicroIconPositionY();
	static D3DCOLOR GetMicroIconColor();
	static float GetMicroIconAngle();
	static float GetMicroIconScale();

	static void SetMicroIconPositionX(int x);
	static void SetMicroIconPositionY(int y);
	static void SetMicroIconPosition(int x, int y);
	static void SetMicroIconColor(D3DCOLOR color);
	static void SetMicroIconAngle(float angle);
	static void SetMicroIconScale(float scale);

	static void ResetConfigs();
	static void SyncConfigs();

	static void SetPassiveIcon();
	static void SetActiveIcon();
	static void SetMutedIcon();

	static void SwitchToPassiveIcon();
	static void SwitchToActiveIcon();
	static void SwitchToMutedIcon();

	static void Render();
	static void Update();

	static void Show();
	static bool IsShowed();
	static void Hide();

	static void Free();

};
