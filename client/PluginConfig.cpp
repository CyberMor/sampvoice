/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "PluginConfig.h"

#include <fstream>

#include <util/Render.h>

bool PluginConfig::loadStatus(false);

bool PluginConfig::playbackLoaded(false);
bool PluginConfig::speakerLoaded(false);
bool PluginConfig::recordLoaded(false);
bool PluginConfig::microLoaded(false);

bool PluginConfig::soundEnable(PluginConfig::DefVal_SoundEnable);
int PluginConfig::soundVolume(PluginConfig::DefVal_SoundVolume);
bool PluginConfig::soundBalancer(PluginConfig::DefVal_SoundBalancer);
bool PluginConfig::soundFilter(PluginConfig::DefVal_SoundFilter);

float PluginConfig::speakerIconScale(PluginConfig::DefVal_SpeakerIconScale);
int PluginConfig::speakerIconOffsetX(PluginConfig::DefVal_SpeakerIconOffsetX);
int PluginConfig::speakerIconOffsetY(PluginConfig::DefVal_SpeakerIconOffsetY);

bool PluginConfig::microEnable(PluginConfig::DefVal_MicroEnable);
int PluginConfig::microVolume(PluginConfig::DefVal_MicroVolume);
std::string PluginConfig::deviceName;

float PluginConfig::microIconScale(PluginConfig::DefVal_MicroIconScale);
int PluginConfig::microIconPositionX(PluginConfig::DefVal_MicroIconPositionX);
int PluginConfig::microIconPositionY(PluginConfig::DefVal_MicroIconPositionY);
D3DCOLOR PluginConfig::microIconColor(PluginConfig::DefVal_MicroIconColor);
float PluginConfig::microIconAngle(PluginConfig::DefVal_MicroIconAngle);

bool PluginConfig::Load(const std::string& configFilePath) {

	std::ifstream configFile(configFilePath, std::ios::in | std::ios::binary);

	if (!configFile || !configFile.is_open()) return false;

	PluginConfig::Reset();

	int configVersion = -1;

	if (!configFile.read((char*)(&configVersion), sizeof(configVersion))) return false;

	if (configVersion != ConfigVersion) return false;

	return PluginConfig::loadStatus =

		configFile.read((char*)(&PluginConfig::playbackLoaded), sizeof(PluginConfig::playbackLoaded)) &&
		configFile.read((char*)(&PluginConfig::speakerLoaded), sizeof(PluginConfig::speakerLoaded)) &&
		configFile.read((char*)(&PluginConfig::recordLoaded), sizeof(PluginConfig::recordLoaded)) &&
		configFile.read((char*)(&PluginConfig::microLoaded), sizeof(PluginConfig::microLoaded)) &&

		configFile.read((char*)(&PluginConfig::soundEnable), sizeof(PluginConfig::soundEnable)) &&
		configFile.read((char*)(&PluginConfig::soundVolume), sizeof(PluginConfig::soundVolume)) &&
		configFile.read((char*)(&PluginConfig::soundBalancer), sizeof(PluginConfig::soundBalancer)) &&
		configFile.read((char*)(&PluginConfig::soundFilter), sizeof(PluginConfig::soundFilter)) &&

		configFile.read((char*)(&PluginConfig::speakerIconScale), sizeof(PluginConfig::speakerIconScale)) &&
		configFile.read((char*)(&PluginConfig::speakerIconOffsetX), sizeof(PluginConfig::speakerIconOffsetX)) &&
		configFile.read((char*)(&PluginConfig::speakerIconOffsetY), sizeof(PluginConfig::speakerIconOffsetY)) &&

		configFile.read((char*)(&PluginConfig::microEnable), sizeof(PluginConfig::microEnable)) &&
		configFile.read((char*)(&PluginConfig::microVolume), sizeof(PluginConfig::microVolume)) &&
		std::getline(configFile, PluginConfig::deviceName, '\0') &&

		configFile.read((char*)(&PluginConfig::microIconScale), sizeof(PluginConfig::microIconScale)) &&
		configFile.read((char*)(&PluginConfig::microIconPositionX), sizeof(PluginConfig::microIconPositionX)) &&
		configFile.read((char*)(&PluginConfig::microIconPositionY), sizeof(PluginConfig::microIconPositionY)) &&
		configFile.read((char*)(&PluginConfig::microIconColor), sizeof(PluginConfig::microIconColor)) &&
		configFile.read((char*)(&PluginConfig::microIconAngle), sizeof(PluginConfig::microIconAngle));

}

bool PluginConfig::Save(const std::string& configFilePath) {

	std::ofstream configFile(configFilePath, std::ios::out | std::ios::binary | std::ios::trunc);

	if (!configFile || !configFile.is_open()) return false;

	const int configVersion = ConfigVersion;

	return

		configFile.write((char*)(&configVersion), sizeof(configVersion)) &&

		configFile.write((char*)(&PluginConfig::playbackLoaded), sizeof(PluginConfig::playbackLoaded)) &&
		configFile.write((char*)(&PluginConfig::speakerLoaded), sizeof(PluginConfig::speakerLoaded)) &&
		configFile.write((char*)(&PluginConfig::recordLoaded), sizeof(PluginConfig::recordLoaded)) &&
		configFile.write((char*)(&PluginConfig::microLoaded), sizeof(PluginConfig::microLoaded)) &&

		configFile.write((char*)(&PluginConfig::soundEnable), sizeof(PluginConfig::soundEnable)) &&
		configFile.write((char*)(&PluginConfig::soundVolume), sizeof(PluginConfig::soundVolume)) &&
		configFile.write((char*)(&PluginConfig::soundBalancer), sizeof(PluginConfig::soundBalancer)) &&
		configFile.write((char*)(&PluginConfig::soundFilter), sizeof(PluginConfig::soundFilter)) &&

		configFile.write((char*)(&PluginConfig::speakerIconScale), sizeof(PluginConfig::speakerIconScale)) &&
		configFile.write((char*)(&PluginConfig::speakerIconOffsetX), sizeof(PluginConfig::speakerIconOffsetX)) &&
		configFile.write((char*)(&PluginConfig::speakerIconOffsetY), sizeof(PluginConfig::speakerIconOffsetY)) &&

		configFile.write((char*)(&PluginConfig::microEnable), sizeof(PluginConfig::microEnable)) &&
		configFile.write((char*)(&PluginConfig::microVolume), sizeof(PluginConfig::microVolume)) &&
		configFile.write(PluginConfig::deviceName.data(), PluginConfig::deviceName.size() + 1) &&

		configFile.write((char*)(&PluginConfig::microIconScale), sizeof(PluginConfig::microIconScale)) &&
		configFile.write((char*)(&PluginConfig::microIconPositionX), sizeof(PluginConfig::microIconPositionX)) &&
		configFile.write((char*)(&PluginConfig::microIconPositionY), sizeof(PluginConfig::microIconPositionY)) &&
		configFile.write((char*)(&PluginConfig::microIconColor), sizeof(PluginConfig::microIconColor)) &&
		configFile.write((char*)(&PluginConfig::microIconAngle), sizeof(PluginConfig::microIconAngle));

}

void PluginConfig::Reset() {

	PluginConfig::loadStatus = false;

	PluginConfig::playbackLoaded = false;
	PluginConfig::speakerLoaded = false;
	PluginConfig::recordLoaded = false;
	PluginConfig::microLoaded = false;

	PluginConfig::soundEnable = DefVal_SoundEnable;
	PluginConfig::soundVolume = DefVal_SoundVolume;
	PluginConfig::soundBalancer = DefVal_SoundBalancer;
	PluginConfig::soundFilter = DefVal_SoundFilter;

	PluginConfig::speakerIconScale = DefVal_SpeakerIconScale;
	PluginConfig::speakerIconOffsetX = DefVal_SpeakerIconOffsetX;
	PluginConfig::speakerIconOffsetY = DefVal_SpeakerIconOffsetY;

	PluginConfig::microEnable = DefVal_MicroEnable;
	PluginConfig::microVolume = DefVal_MicroVolume;
	PluginConfig::deviceName.clear();

	PluginConfig::microIconScale = DefVal_MicroIconScale;
	PluginConfig::microIconPositionX = DefVal_MicroIconPositionX;
	PluginConfig::microIconPositionY = DefVal_MicroIconPositionY;
	PluginConfig::microIconColor = DefVal_MicroIconColor;
	PluginConfig::microIconAngle = DefVal_MicroIconAngle;

}

bool PluginConfig::IsLoaded() {

	return PluginConfig::loadStatus;

}

bool PluginConfig::IsPlaybackLoaded() {

	return PluginConfig::playbackLoaded;

}

bool PluginConfig::IsSpeakerLoaded() {

	return PluginConfig::speakerLoaded;

}

bool PluginConfig::IsRecordLoaded() {

	return PluginConfig::recordLoaded;

}

bool PluginConfig::IsMicroLoaded() {

	return PluginConfig::microLoaded;

}

void PluginConfig::SetPlaybackLoaded(const bool status) {

	PluginConfig::playbackLoaded = status;

}

void PluginConfig::SetSpeakerLoaded(const bool status) {

	PluginConfig::speakerLoaded = status;

}

void PluginConfig::SetRecordLoaded(const bool status) {

	PluginConfig::recordLoaded = status;

}

void PluginConfig::SetMicroLoaded(const bool status) {

	PluginConfig::microLoaded = status;

}

bool PluginConfig::GetSoundEnable() {

	return PluginConfig::soundEnable;

}

int PluginConfig::GetSoundVolume() {

	return PluginConfig::soundVolume;

}

bool PluginConfig::GetSoundBalancer() {

	return PluginConfig::soundBalancer;

}

bool PluginConfig::GetSoundFilter() {

	return PluginConfig::soundFilter;

}

float PluginConfig::GetSpeakerIconScale() {

	return PluginConfig::speakerIconScale;

}

int PluginConfig::GetSpeakerIconOffsetX() {

	float screenValue = 0.f;

	return (Render::ConvertBaseXValueToScreenXValue(PluginConfig::speakerIconOffsetX, screenValue)) ?
		std::roundf(screenValue) : DefVal_SpeakerIconOffsetX;

}

int PluginConfig::GetSpeakerIconOffsetY() {

	float screenValue = 0.f;

	return (Render::ConvertBaseYValueToScreenYValue(PluginConfig::speakerIconOffsetY, screenValue)) ?
		std::roundf(screenValue) : DefVal_SpeakerIconOffsetY;

}

bool PluginConfig::GetMicroEnable() {

	return PluginConfig::microEnable;

}

int PluginConfig::GetMicroVolume() {

	return PluginConfig::microVolume;

}

const std::string& PluginConfig::GetDeviceName() {

	return PluginConfig::deviceName;

}

float PluginConfig::GetMicroIconScale() {

	return PluginConfig::microIconScale;

}

int PluginConfig::GetMicroIconPositionX() {

	float screenValue = 0.f;

	return (Render::ConvertBaseXValueToScreenXValue(PluginConfig::microIconPositionX, screenValue)) ?
		std::roundf(screenValue) : DefVal_MicroIconPositionX;

}

int PluginConfig::GetMicroIconPositionY() {

	float screenValue = 0.f;

	return (Render::ConvertBaseYValueToScreenYValue(PluginConfig::microIconPositionY, screenValue)) ?
		std::roundf(screenValue) : DefVal_MicroIconPositionY;

}

D3DCOLOR PluginConfig::GetMicroIconColor() {

	return PluginConfig::microIconColor;

}

float PluginConfig::GetMicroIconAngle() {

	return PluginConfig::microIconAngle;

}

void PluginConfig::SetSoundEnable(const bool soundEnable) {

	PluginConfig::soundEnable = soundEnable;

}

void PluginConfig::SetSoundVolume(const int soundVolume) {

	PluginConfig::soundVolume = soundVolume;

}

void PluginConfig::SetSoundBalancer(const bool soundBalancer) {

	PluginConfig::soundBalancer = soundBalancer;

}

void PluginConfig::SetSoundFilter(const bool soundFilter) {

	PluginConfig::soundFilter = soundFilter;

}

void PluginConfig::SetSpeakerIconScale(const float speakerIconScale) {

	PluginConfig::speakerIconScale = speakerIconScale;

}

void PluginConfig::SetSpeakerIconOffsetX(const int speakerIconOffsetX) {

	float baseValue = 0.f;

	if (Render::ConvertScreenXValueToBaseXValue(speakerIconOffsetX, baseValue))
		PluginConfig::speakerIconOffsetX = std::roundf(baseValue);

}

void PluginConfig::SetSpeakerIconOffsetY(const int speakerIconOffsetY) {

	float baseValue = 0.f;

	if (Render::ConvertScreenYValueToBaseYValue(speakerIconOffsetY, baseValue))
		PluginConfig::speakerIconOffsetY = std::roundf(baseValue);

}

void PluginConfig::SetMicroEnable(const bool microEnable) {

	PluginConfig::microEnable = microEnable;

}

void PluginConfig::SetMicroVolume(const int microVolume) {

	PluginConfig::microVolume = microVolume;

}

void PluginConfig::SetDeviceName(const std::string& deviceName) {

	PluginConfig::deviceName = deviceName;

}

void PluginConfig::SetMicroIconScale(const float microIconScale) {

	PluginConfig::microIconScale = microIconScale;

}

void PluginConfig::SetMicroIconPositionX(const int microIconPositionX) {

	float baseValue = 0.f;

	if (Render::ConvertScreenXValueToBaseXValue(microIconPositionX, baseValue))
		PluginConfig::microIconPositionX = std::roundf(baseValue);

}

void PluginConfig::SetMicroIconPositionY(const int microIconPositionY) {

	float baseValue = 0.f;

	if (Render::ConvertScreenYValueToBaseYValue(microIconPositionY, baseValue))
		PluginConfig::microIconPositionY = std::roundf(baseValue);

}

void PluginConfig::SetMicroIconColor(const D3DCOLOR microIconColor) {

	PluginConfig::microIconColor = microIconColor;

}

void PluginConfig::SetMicroIconAngle(const float microIconAngle) {

	PluginConfig::microIconAngle = microIconAngle;

}
