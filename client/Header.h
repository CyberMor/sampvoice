/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <chrono>
#include <thread>

#include <game/CVector.h>

#define GetTimestamp() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define SleepForMilliseconds(mscount) std::this_thread::sleep_for(std::chrono::milliseconds(mscount))

namespace SV {

	// Strings
	// --------------------------------------------

	constexpr const char* LogFileName = "svlog.txt";
	constexpr const char* ConfigFileName = "svconfig.bin";

	// Constants
	// --------------------------------------------

	constexpr WORD NonePlayer = 0xffff;

	constexpr BYTE Version = 10;
	constexpr DWORD Signature = 0xDeadBeef;

	constexpr DWORD AudioUpdateThreads = 4;
	constexpr DWORD AudioUpdatePeriod = 10;

	constexpr DWORD VoiceRate = 100;
	constexpr DWORD Frequency = 48000;
	constexpr DWORD FrameSizeInSamples = (Frequency / 1000) * VoiceRate;
	constexpr DWORD FrameSizeInBytes = FrameSizeInSamples * sizeof(WORD);

	constexpr DWORD ChannelPreBufferFramesCount = 3;
	constexpr DWORD ChannelPreBufferSizeInMs = ChannelPreBufferFramesCount * VoiceRate;
	constexpr DWORD ChannelBufferSizeInMs = 3 * ChannelPreBufferSizeInMs;

	struct ControlPacketType {
		enum : BYTE {

			serverInfo,
			pluginInit,

			muteEnable,
			muteDisable,
			startRecord,
			stopRecord,
			addKey,
			removeKey,
			removeAllKeys,
			createGStream,
			createLPStream,
			createLStreamAtVehicle,
			createLStreamAtPlayer,
			createLStreamAtObject,
			updateLPStreamDistance,
			updateLPStreamPosition,
			deleteStream,

			pressKey,
			releaseKey

		};
	};

	struct VoicePacketType {
		enum : BYTE {

			keepAlive,

			voicePacket

		};
	};

	// Packets
	// --------------------------------------------

#pragma pack(push, 1)

	struct ConnectPacket {

		DWORD signature;
		BYTE version;
		BYTE micro;

	};

	struct ServerInfoPacket {

		DWORD serverKey;
		WORD serverPort;

	};

	struct PluginInitPacket {

		DWORD bitrate;
		BYTE mute;

	};

	struct AddKeyPacket {

		BYTE keyId;

	};

	struct RemoveKeyPacket {

		BYTE keyId;

	};

	struct CreateGStreamPacket {

		DWORD stream;
		DWORD color;
		char name[];

	};

	struct CreateLPStreamPacket {

		DWORD stream;
		float distance;
		CVector position;
		DWORD color;
		char name[];

	};

	struct CreateLStreamAtPacket {

		DWORD stream;
		float distance;
		WORD target;
		DWORD color;
		char name[];

	};

	struct UpdateLPStreamDistancePacket {

		DWORD stream;
		float distance;

	};

	struct UpdateLPStreamPositionPacket {

		DWORD stream;
		CVector position;

	};

	struct DeleteStreamPacket {

		DWORD stream;

	};

	struct PressKeyPacket {

		BYTE keyId;

	};

	struct ReleaseKeyPacket {

		BYTE keyId;

	};

#pragma pack(pop)

}
