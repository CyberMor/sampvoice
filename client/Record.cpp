/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Record.h"

#include <util/Logger.h>

#include "PluginConfig.h"

bool Record::initStatus(false);
bool Record::checkStatus(false);
bool Record::recordStatus(false);

HRECORD Record::recordChannel(NULL);
OpusEncoder* Record::encoder(nullptr);

opus_int16 Record::encBuffer[SV::FrameSizeInSamples];

HSTREAM Record::checkChannel(NULL);

int Record::usedDeviceIndex(-1);
std::vector<std::string> Record::deviceNamesList;
std::vector<int> Record::deviceNumbersList;

bool Record::Init(const DWORD bitrate) {

	if (Record::initStatus) return false;

	if (!BASS_IsStarted()) return false;

	Logger::LogToFile("[sv:dbg:record:init] : module initializing...");

	Record::deviceNamesList.clear();
	Record::deviceNumbersList.clear();

	BASS_DEVICEINFO devInfo = {};

	for (int devNumber = 0; BASS_RecordGetDeviceInfo(devNumber, &devInfo); ++devNumber) {

		const bool deviceEnabled = devInfo.flags & BASS_DEVICE_ENABLED;
		const bool deviceLoopback = devInfo.flags & BASS_DEVICE_LOOPBACK;

		const DWORD deviceType = devInfo.flags & BASS_DEVICE_TYPE_MASK;

		Logger::LogToFile(
			"[sv:dbg:record:init] : device detect [ id(%d) enabled(%u) loopback(%u) name(%s) type(0x%x) ]",
			devNumber, deviceEnabled, deviceLoopback, devInfo.name ? devInfo.name : "none", deviceType
		);

		if (deviceEnabled && !deviceLoopback && devInfo.name) {

			Record::deviceNumbersList.emplace_back(devNumber);
			Record::deviceNamesList.emplace_back(devInfo.name);

		}

	}

	if (Record::deviceNamesList.empty() || Record::deviceNumbersList.empty()) {

		Logger::LogToFile("[sv:inf:record:init] : failed to find microphone");

		return false;

	}

	if (int error = -1; !(Record::encoder = opus_encoder_create(SV::Frequency, 1, OPUS_APPLICATION_VOIP, &error)) || error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to create encoder (code:%d)", error);

		return false;

	}

	if (const int error = opus_encoder_ctl(Record::encoder, OPUS_SET_BITRATE(bitrate)); error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to set bitrate for encoder (code:%d)", error);

		opus_encoder_destroy(Record::encoder);

		return false;

	}

	if (const int error = opus_encoder_ctl(Record::encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE)); error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to set audiosignal type for encoder (code:%d)", error);

		opus_encoder_destroy(Record::encoder);

		return false;

	}

	if (const int error = opus_encoder_ctl(Record::encoder, OPUS_SET_COMPLEXITY(10)); error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to set complexity for encoder (code:%d)", error);

		opus_encoder_destroy(Record::encoder);

		return false;

	}

	if (const int error = opus_encoder_ctl(Record::encoder, OPUS_SET_PREDICTION_DISABLED(FALSE)); error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to enable prediction for encoder (code:%d)", error);

		opus_encoder_destroy(Record::encoder);

		return false;

	}

	if (const int error = opus_encoder_ctl(Record::encoder, OPUS_SET_LSB_DEPTH(16)); error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to set lsb depth for encoder (code:%d)", error);

		opus_encoder_destroy(Record::encoder);

		return false;

	}

	if (const int error = opus_encoder_ctl(Record::encoder, OPUS_SET_FORCE_CHANNELS(TRUE)); error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to set count channels for encoder (code:%d)", error);

		opus_encoder_destroy(Record::encoder);

		return false;

	}

	if (const int error = opus_encoder_ctl(Record::encoder, OPUS_SET_DTX(FALSE)); error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to set dtx for encoder (code:%d)", error);

		opus_encoder_destroy(Record::encoder);

		return false;

	}

	if (const int error = opus_encoder_ctl(Record::encoder, OPUS_SET_INBAND_FEC(TRUE)); error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to set inband fec for encoder (code:%d)", error);

		opus_encoder_destroy(Record::encoder);

		return false;

	}

	if (const int error = opus_encoder_ctl(Record::encoder, OPUS_SET_PACKET_LOSS_PERC(10)); error < 0) {

		Logger::LogToFile("[sv:err:record:init] : failed to set packet loss percent for encoder (code:%d)", error);

		opus_encoder_destroy(Record::encoder);

		return false;

	}

	Record::usedDeviceIndex = -1;

	if (PluginConfig::IsRecordLoaded() && !PluginConfig::GetDeviceName().empty()) for (size_t i = 0; i < Record::deviceNamesList.size(); ++i)
		if (Record::deviceNamesList[i] == PluginConfig::GetDeviceName()) { Record::usedDeviceIndex = i; break; }

	bool initRecordStatus = BASS_RecordInit(Record::usedDeviceIndex != -1 ? Record::deviceNumbersList[Record::usedDeviceIndex] : -1);
	if (!initRecordStatus && Record::usedDeviceIndex != -1) initRecordStatus = BASS_RecordInit(Record::usedDeviceIndex = -1);

	if (initRecordStatus && Record::usedDeviceIndex == -1) for (size_t i = 0; i < Record::deviceNumbersList.size(); ++i)
		if (Record::deviceNumbersList[i] == BASS_RecordGetDevice()) { Record::usedDeviceIndex = i; break; }

	if (!initRecordStatus || Record::usedDeviceIndex == -1) {

		Logger::LogToFile("[sv:err:record:init] : failed to init device (code:%d)", BASS_ErrorGetCode());

		opus_encoder_destroy(Record::encoder);
		if (initRecordStatus) BASS_RecordFree();

		return false;

	}

	if (!PluginConfig::IsRecordLoaded()) PluginConfig::SetDeviceName(Record::deviceNamesList[Record::usedDeviceIndex]);

	if (!(Record::recordChannel = BASS_RecordStart(SV::Frequency, 1, BASS_RECORD_PAUSE, nullptr, nullptr))) {

		Logger::LogToFile("[sv:err:record:init] : failed to create record stream (code:%d)", BASS_ErrorGetCode());

		opus_encoder_destroy(Record::encoder);
		BASS_RecordFree();

		return false;

	}

	if (!(Record::checkChannel = BASS_StreamCreate(SV::Frequency, 1, NULL, STREAMPROC_PUSH, nullptr))) {

		Logger::LogToFile("[sv:err:record:init] : failed to create check stream (code:%d)", BASS_ErrorGetCode());

		BASS_ChannelStop(Record::recordChannel);
		opus_encoder_destroy(Record::encoder);
		BASS_RecordFree();

		return false;

	}

	BASS_ChannelSetAttribute(Record::checkChannel, BASS_ATTRIB_VOL, 4.f);

	if (!PluginConfig::IsRecordLoaded()) {

		PluginConfig::SetRecordLoaded(true);
		Record::ResetConfigs();

	}

	Logger::LogToFile("[sv:dbg:record:init] : module initialized");

	Record::initStatus = true;

	Record::SyncConfigs();

	return true;

}

void Record::Tick() {

	if (!Record::initStatus) return;
	if (!Record::checkStatus) return;

	const DWORD bufferSize = BASS_ChannelGetData(Record::recordChannel, nullptr, BASS_DATA_AVAILABLE);

	if (bufferSize == -1 || !bufferSize) return;

	const DWORD readDataSize = BASS_ChannelGetData(Record::recordChannel, Record::encBuffer, min(bufferSize, SV::FrameSizeInBytes));

	if (readDataSize == -1 || !readDataSize) return;

	BASS_StreamPutData(Record::checkChannel, Record::encBuffer, readDataSize);

}

DWORD Record::GetFrame(BYTE* bufferPtr, DWORD bufferSize) {

	if (!Record::initStatus) return NULL;
	if (!Record::recordStatus || Record::checkStatus) return NULL;

	const auto cBufferSize = BASS_ChannelGetData(Record::recordChannel, nullptr, BASS_DATA_AVAILABLE);

	if (cBufferSize == -1 || cBufferSize < SV::FrameSizeInBytes) return NULL;
	if (BASS_ChannelGetData(Record::recordChannel, Record::encBuffer, SV::FrameSizeInBytes) != SV::FrameSizeInBytes) return NULL;

	const auto encDataLength = opus_encode(Record::encoder, Record::encBuffer, SV::FrameSizeInSamples, bufferPtr, bufferSize);

	return encDataLength > 0 ? encDataLength : NULL;

}

bool Record::HasMicro() {

	BASS_DEVICEINFO devInfo = {};

	for (int devNumber = 0; BASS_RecordGetDeviceInfo(devNumber, &devInfo); ++devNumber) {

		const bool deviceEnabled = devInfo.flags & BASS_DEVICE_ENABLED;
		const bool deviceLoopback = devInfo.flags & BASS_DEVICE_LOOPBACK;

		if (deviceEnabled && !deviceLoopback && devInfo.name) return true;

	}

	return false;

}

bool Record::StartRecording() {

	if (!Record::initStatus) return false;
	if (Record::recordStatus || Record::checkStatus) return false;
	if (!PluginConfig::GetMicroEnable()) return false;

	Logger::LogToFile("[sv:dbg:record:startrecording] : channel recording starting...");

	BASS_ChannelPlay(Record::recordChannel, FALSE);

	return Record::recordStatus = true;

}

bool Record::IsRecording() {

	return Record::recordStatus;

}

void Record::StopRecording() {

	if (!Record::initStatus) return;

	Record::recordStatus = false;

	if (Record::checkStatus) return;
	
	BASS_ChannelPause(Record::recordChannel);
	opus_encoder_ctl(Record::encoder, OPUS_RESET_STATE);

	Logger::LogToFile("[sv:dbg:record:stoprecording] : channel recording stoped");

	const DWORD bufferSize = BASS_ChannelGetData(Record::recordChannel, nullptr, BASS_DATA_AVAILABLE);

	if (bufferSize != -1 && bufferSize) BASS_ChannelGetData(Record::recordChannel, nullptr, bufferSize);

}

bool Record::GetMicroEnable() {

	return PluginConfig::GetMicroEnable();

}

int Record::GetMicroVolume() {

	return PluginConfig::GetMicroVolume();

}

int Record::GetMicroDevice() {

	return Record::usedDeviceIndex;

}

void Record::SetMicroEnable(bool microEnable) {

	if (!Record::initStatus) return;

	PluginConfig::SetMicroEnable(microEnable);

	if (!PluginConfig::GetMicroEnable()) {

		Record::StopRecording();
		Record::StopChecking();

	}

}

void Record::SetMicroVolume(int microVolume) {

	if (!Record::initStatus) return;

	if (microVolume < 0) microVolume = 0;
	if (microVolume > 100) microVolume = 100;

	PluginConfig::SetMicroVolume(microVolume);

	BASS_RecordSetInput(-1, BASS_INPUT_ON, (float)(PluginConfig::GetMicroVolume()) / 100.f);

}

void Record::SetMicroDevice(int deviceIndex) {

	if (!Record::initStatus) return;
	
	const auto devNamesCount = Record::deviceNamesList.size();
	const auto devNumbersCount = Record::deviceNumbersList.size();

	if (deviceIndex < 0 || deviceIndex >= min(devNamesCount, devNumbersCount)) return;
	if (deviceIndex == Record::usedDeviceIndex) return;

	BASS_ChannelStop(Record::recordChannel);
	BASS_RecordFree();

	const auto oldDevIndex = Record::usedDeviceIndex;

	if (!BASS_RecordInit(Record::deviceNumbersList[Record::usedDeviceIndex = deviceIndex]) &&
		!BASS_RecordInit(Record::deviceNumbersList[Record::usedDeviceIndex = oldDevIndex])) return;

	Record::recordChannel = BASS_RecordStart(SV::Frequency, 1, !Record::recordStatus &&
		!Record::checkStatus ? BASS_RECORD_PAUSE : NULL, nullptr, nullptr);

	PluginConfig::SetDeviceName(Record::deviceNamesList[Record::usedDeviceIndex]);

}

void Record::SyncConfigs() {

	Record::SetMicroEnable(PluginConfig::GetMicroEnable());
	Record::SetMicroVolume(PluginConfig::GetMicroVolume());

}

void Record::ResetConfigs() {

	PluginConfig::SetMicroEnable(PluginConfig::DefVal_MicroEnable);
	PluginConfig::SetMicroVolume(PluginConfig::DefVal_MicroVolume);

}

const std::vector<std::string>& Record::GetDeviceNamesList() {

	return Record::deviceNamesList;

}

const std::vector<int>& Record::GetDeviceNumbersList() {

	return Record::deviceNumbersList;

}

bool Record::StartChecking() {

	if (!Record::initStatus) return false;
	if (Record::checkStatus) return false;
	if (!PluginConfig::GetMicroEnable()) return false;

	Record::StopRecording();

	Logger::LogToFile("[sv:dbg:record:startchecking] : checking device starting...");

	BASS_ChannelPlay(Record::checkChannel, TRUE);
	BASS_ChannelPlay(Record::recordChannel, TRUE);

	return Record::checkStatus = true;

}

bool Record::IsChecking() {

	return Record::checkStatus;

}

void Record::StopChecking() {

	if (!Record::initStatus) return;
	if (!Record::checkStatus) return;

	BASS_ChannelStop(Record::checkChannel);

	Logger::LogToFile("[sv:dbg:record:stopchecking] : checking device stoped");

	Record::checkStatus = false;

}

void Record::Free() {

	if (!Record::initStatus) return;

	Logger::LogToFile("[sv:dbg:record:free] : module releasing...");

	Record::StopRecording();
	BASS_ChannelStop(Record::recordChannel);
	BASS_RecordFree();

	Record::StopChecking();
	BASS_StreamFree(Record::checkChannel);

	opus_encoder_destroy(Record::encoder);

	Record::deviceNumbersList.clear();
	Record::deviceNamesList.clear();

	Logger::LogToFile("[sv:dbg:record:free] : module released");

	Record::initStatus = false;

}
