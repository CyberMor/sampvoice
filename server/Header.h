/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <thread>

#include <ysf/utils/cvector.h>

#define GetTimestamp() (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
#define SleepForMilliseconds(mscount) (std::this_thread::sleep_for(std::chrono::milliseconds(mscount)))

namespace SV {

	// Strings
	// --------------------------------------------

	constexpr const char*		LogFileName			= "svlog.txt";

	// Constants
	// --------------------------------------------

	constexpr uint16_t			NonePlayer			= 0xffff;

	static constexpr uint32_t	VoiceThreadsCount	= 8;
	static constexpr uint32_t	DefaultBitrate		= 24000;

	constexpr uint8_t			Version				= 10;

	constexpr uint32_t			Signature			= 0xDeadBeef;
	constexpr const char*		SignaturePattern	= "\xef\xbe\xad\xde";
	constexpr const char*		SignatureMask		= "xxxx";

	struct ControlPacketType {
		enum : uint8_t {

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
		enum : uint8_t {
			
			keepAlive,

			voicePacket

		};
	};

	// Packets
	// --------------------------------------------

#pragma pack(push, 1)

	struct ConnectPacket {

		uint32_t	signature;
		uint8_t		version;
		uint8_t		micro;

	};

	struct ServerInfoPacket {

		uint32_t	serverKey;
		uint16_t	serverPort;

	};

	struct PluginInitPacket {

		uint32_t	bitrate;
		uint8_t		mute;

	};

	struct AddKeyPacket {

		uint8_t		keyId;

	};

	struct RemoveKeyPacket {

		uint8_t		keyId;

	};

	struct CreateGStreamPacket {

		uint32_t	stream;
		uint32_t	color;
		char		name[];

	};

	struct CreateLPStreamPacket {

		uint32_t	stream;
		float		distance;
		CVector		position;
		uint32_t	color;
		char		name[];

	};

	struct CreateLStreamAtPacket {

		uint32_t	stream;
		float		distance;
		uint16_t	target;
		uint32_t	color;
		char		name[];

	};

	struct UpdateLPStreamDistancePacket {

		uint32_t	stream;
		float		distance;

	};

	struct UpdateLPStreamPositionPacket {

		uint32_t	stream;
		CVector		position;

	};

	struct DeleteStreamPacket {

		uint32_t	stream;

	};

	struct PressKeyPacket {

		uint8_t		keyId;

	};

	struct ReleaseKeyPacket {

		uint8_t		keyId;

	};

#pragma pack(pop)

}
