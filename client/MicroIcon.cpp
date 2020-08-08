/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "MicroIcon.h"

#include <assert.h>

#include <util/Logger.h>
#include <util/GameUtil.h>
#include <util/Render.h>

#include "PluginConfig.h"

bool MicroIcon::initStatus(false);
bool MicroIcon::showStatus(false);

int MicroIcon::alphaLevelPassiveIcon(0);
int MicroIcon::alphaLevelDeviationPassiveIcon(0);
TexturePtr MicroIcon::tPassiveIcon(nullptr);

int MicroIcon::alphaLevelActiveIcon(0);
int MicroIcon::alphaLevelDeviationActiveIcon(0);
TexturePtr MicroIcon::tActiveIcon(nullptr);

int MicroIcon::alphaLevelMutedIcon(0);
int MicroIcon::alphaLevelDeviationMutedIcon(0);
TexturePtr MicroIcon::tMutedIcon(nullptr);

bool MicroIcon::Init(
	IDirect3DDevice9* pDevice,
	const Resource& rPassiveIcon,
	const Resource& rActiveIcon,
	const Resource& rMutedIcon
) {

	assert(pDevice);

	if (MicroIcon::initStatus) return false;

	if (!(MicroIcon::tPassiveIcon = MakeTexture(pDevice, rPassiveIcon))) {

		Logger::LogToFile("[sv:err:microicon:init] : failed to create passive icon");

		return false;

	}

	if (!(MicroIcon::tActiveIcon = MakeTexture(pDevice, rActiveIcon))) {

		Logger::LogToFile("[sv:err:microicon:init] : failed to create active icon");

		MicroIcon::tPassiveIcon.reset();

		return false;

	}

	if (!(MicroIcon::tMutedIcon = MakeTexture(pDevice, rMutedIcon))) {

		Logger::LogToFile("[sv:err:microicon:init] : failed to create muted icon");

		MicroIcon::tPassiveIcon.reset();
		MicroIcon::tActiveIcon.reset();

		return false;

	}

	MicroIcon::SetPassiveIcon();

	if (!PluginConfig::IsMicroLoaded()) {

		PluginConfig::SetMicroLoaded(true);
		MicroIcon::ResetConfigs();

	}

	MicroIcon::initStatus = true;

	MicroIcon::SyncConfigs();

	return true;

}

int MicroIcon::GetMicroIconPositionX() {

	return PluginConfig::GetMicroIconPositionX();

}

int MicroIcon::GetMicroIconPositionY() {

	return PluginConfig::GetMicroIconPositionY();

}

D3DCOLOR MicroIcon::GetMicroIconColor() {

	return PluginConfig::GetMicroIconColor();

}

float MicroIcon::GetMicroIconAngle() {

	return PluginConfig::GetMicroIconAngle();

}

float MicroIcon::GetMicroIconScale() {

	return PluginConfig::GetMicroIconScale();

}

void MicroIcon::SetMicroIconPositionX(int x) {

	PluginConfig::SetMicroIconPositionX(x);

}

void MicroIcon::SetMicroIconPositionY(int y) {

	PluginConfig::SetMicroIconPositionY(y);

}

void MicroIcon::SetMicroIconPosition(int x, int y) {

	PluginConfig::SetMicroIconPositionX(x);
	PluginConfig::SetMicroIconPositionY(y);

}

void MicroIcon::SetMicroIconColor(D3DCOLOR color) {

	PluginConfig::SetMicroIconColor(color);

}

void MicroIcon::SetMicroIconAngle(float angle) {

	PluginConfig::SetMicroIconAngle(angle);

}

void MicroIcon::SetMicroIconScale(float scale) {

	if (scale > 2.f) scale = 2.f;
	if (scale < 0.2f) scale = 0.2f;

	PluginConfig::SetMicroIconScale(scale);

}

void MicroIcon::ResetConfigs() {

	PluginConfig::SetMicroIconScale(PluginConfig::DefVal_MicroIconScale);
	PluginConfig::SetMicroIconPositionX(PluginConfig::DefVal_MicroIconPositionX);
	PluginConfig::SetMicroIconPositionY(PluginConfig::DefVal_MicroIconPositionY);
	PluginConfig::SetMicroIconColor(PluginConfig::DefVal_MicroIconColor);
	PluginConfig::SetMicroIconAngle(PluginConfig::DefVal_MicroIconAngle);

}

void MicroIcon::SyncConfigs() {

	MicroIcon::SetMicroIconScale(PluginConfig::GetMicroIconScale());
	MicroIcon::SetMicroIconPositionX(PluginConfig::GetMicroIconPositionX());
	MicroIcon::SetMicroIconPositionY(PluginConfig::GetMicroIconPositionY());
	MicroIcon::SetMicroIconColor(PluginConfig::GetMicroIconColor());
	MicroIcon::SetMicroIconAngle(PluginConfig::GetMicroIconAngle());

}

void MicroIcon::SetPassiveIcon() {

	MicroIcon::alphaLevelActiveIcon = 0;
	MicroIcon::alphaLevelDeviationActiveIcon = 0;
	MicroIcon::alphaLevelMutedIcon = 0;
	MicroIcon::alphaLevelDeviationMutedIcon = 0;

	MicroIcon::alphaLevelPassiveIcon = 255;
	MicroIcon::alphaLevelDeviationPassiveIcon = 0;

}

void MicroIcon::SetActiveIcon() {

	MicroIcon::alphaLevelPassiveIcon = 0;
	MicroIcon::alphaLevelDeviationPassiveIcon = 0;
	MicroIcon::alphaLevelMutedIcon = 0;
	MicroIcon::alphaLevelDeviationMutedIcon = 0;

	MicroIcon::alphaLevelActiveIcon = 255;
	MicroIcon::alphaLevelDeviationActiveIcon = 0;

}

void MicroIcon::SetMutedIcon() {

	MicroIcon::alphaLevelPassiveIcon = 0;
	MicroIcon::alphaLevelDeviationPassiveIcon = 0;
	MicroIcon::alphaLevelActiveIcon = 0;
	MicroIcon::alphaLevelDeviationActiveIcon = 0;

	MicroIcon::alphaLevelMutedIcon = 255;
	MicroIcon::alphaLevelDeviationMutedIcon = 0;

}

void MicroIcon::SwitchToPassiveIcon() {

	MicroIcon::alphaLevelDeviationActiveIcon = AlphaLevelDecrementDeviation;
	MicroIcon::alphaLevelDeviationMutedIcon = AlphaLevelDecrementDeviation;

	MicroIcon::alphaLevelDeviationPassiveIcon = AlphaLevelIncrementDeviation;

}

void MicroIcon::SwitchToActiveIcon() {

	MicroIcon::alphaLevelDeviationPassiveIcon = AlphaLevelDecrementDeviation;
	MicroIcon::alphaLevelDeviationMutedIcon = AlphaLevelDecrementDeviation;

	MicroIcon::alphaLevelDeviationActiveIcon = AlphaLevelIncrementDeviation;

}

void MicroIcon::SwitchToMutedIcon() {

	MicroIcon::alphaLevelDeviationPassiveIcon = AlphaLevelDecrementDeviation;
	MicroIcon::alphaLevelDeviationActiveIcon = AlphaLevelDecrementDeviation;

	MicroIcon::alphaLevelDeviationMutedIcon = AlphaLevelIncrementDeviation;

}

void MicroIcon::Render() {

	if (!MicroIcon::initStatus) return;
	if (!MicroIcon::showStatus) return;

	float vIconSize;

	if (!Render::ConvertBaseYValueToScreenYValue(BaseIconSize, vIconSize)) return;

	vIconSize *= PluginConfig::GetMicroIconScale();

	float vIconX = PluginConfig::GetMicroIconPositionX();
	float vIconY = PluginConfig::GetMicroIconPositionY();

	if (vIconX < 0.f || vIconY < 0.f) {

		CRect radarRect;

		if (!GameUtil::GetRadarRect(radarRect)) return;

		float vIconPadding;

		if (!Render::ConvertBaseYValueToScreenYValue(BaseIconPadding, vIconPadding)) return;

		vIconX = radarRect.left + (radarRect.right - radarRect.left) / 2.f;
		vIconY = radarRect.top - (vIconPadding + vIconSize / 2.f);

	}

	vIconX -= vIconSize / 2.f;
	vIconY -= vIconSize / 2.f;

	if (MicroIcon::alphaLevelPassiveIcon) MicroIcon::tPassiveIcon->Draw(
		vIconX, vIconY, vIconSize, vIconSize,
		(PluginConfig::GetMicroIconColor() & 0x00ffffff) |
		(MicroIcon::alphaLevelPassiveIcon << 24),
		PluginConfig::GetMicroIconAngle()
	);

	if (MicroIcon::alphaLevelActiveIcon) MicroIcon::tActiveIcon->Draw(
		vIconX, vIconY, vIconSize, vIconSize,
		(PluginConfig::GetMicroIconColor() & 0x00ffffff) |
		(MicroIcon::alphaLevelActiveIcon << 24),
		PluginConfig::GetMicroIconAngle()
	);

	if (MicroIcon::alphaLevelMutedIcon) MicroIcon::tMutedIcon->Draw(
		vIconX, vIconY, vIconSize, vIconSize,
		(PluginConfig::GetMicroIconColor() & 0x00ffffff) |
		(MicroIcon::alphaLevelMutedIcon << 24),
		PluginConfig::GetMicroIconAngle()
	);

}

void MicroIcon::Update() {

	if (MicroIcon::alphaLevelDeviationPassiveIcon) {

		MicroIcon::alphaLevelPassiveIcon += MicroIcon::alphaLevelDeviationPassiveIcon;

		if (MicroIcon::alphaLevelPassiveIcon < 0) {

			MicroIcon::alphaLevelDeviationPassiveIcon = 0;
			MicroIcon::alphaLevelPassiveIcon = 0;

		} else if (MicroIcon::alphaLevelPassiveIcon > 255) {

			MicroIcon::alphaLevelDeviationPassiveIcon = 0;
			MicroIcon::alphaLevelPassiveIcon = 255;

		}

	}

	if (MicroIcon::alphaLevelDeviationActiveIcon) {

		MicroIcon::alphaLevelActiveIcon += MicroIcon::alphaLevelDeviationActiveIcon;

		if (MicroIcon::alphaLevelActiveIcon < 0) {

			MicroIcon::alphaLevelDeviationActiveIcon = 0;
			MicroIcon::alphaLevelActiveIcon = 0;

		} else if (MicroIcon::alphaLevelActiveIcon > 255) {

			MicroIcon::alphaLevelDeviationActiveIcon = 0;
			MicroIcon::alphaLevelActiveIcon = 255;

		}

	}

	if (MicroIcon::alphaLevelDeviationMutedIcon) {

		MicroIcon::alphaLevelMutedIcon += MicroIcon::alphaLevelDeviationMutedIcon;

		if (MicroIcon::alphaLevelMutedIcon < 0) {

			MicroIcon::alphaLevelDeviationMutedIcon = 0;
			MicroIcon::alphaLevelMutedIcon = 0;

		} else if (MicroIcon::alphaLevelMutedIcon > 255) {

			MicroIcon::alphaLevelDeviationMutedIcon = 0;
			MicroIcon::alphaLevelMutedIcon = 255;

		}

	}

}

void MicroIcon::Show() {

	MicroIcon::showStatus = true;

}

bool MicroIcon::IsShowed() {

	return MicroIcon::showStatus;

}

void MicroIcon::Hide() {

	MicroIcon::showStatus = false;

}

void MicroIcon::Free() {

	if (!MicroIcon::initStatus) return;

	MicroIcon::tPassiveIcon.reset();
	MicroIcon::tActiveIcon.reset();
	MicroIcon::tMutedIcon.reset();

	MicroIcon::initStatus = false;

}
