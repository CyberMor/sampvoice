/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <vector>

#include <ysf/structs.h>

#include "ControlPacket.h"
#include "VoicePacket.h"

class Stream {
protected:

	int attachedSpeakersCount = 0;
	int attachedListenersCount = 0;

	std::array<std::atomic_bool, MAX_PLAYERS> attachedSpeakers = {};
	std::array<std::atomic_bool, MAX_PLAYERS> attachedListeners = {};

	ControlPacketContainerPtr packetCreateStream = nullptr;
	ControlPacketContainerPtr packetDeleteStream = nullptr;

protected:

	Stream();

public:

	Stream(const Stream& object) = delete;
	Stream(Stream&& object) = delete;

	Stream& operator=(const Stream& object) = delete;
	Stream& operator=(Stream&& object) = delete;

	void PushVoicePacket(VoicePacket& packet);

	virtual bool AttachListener(const uint16_t playerId);
	bool HasListener(const uint16_t playerId);
	virtual bool DetachListener(const uint16_t playerId);
	virtual void DetachAllListeners(std::vector<uint16_t>& detachedListeners);

	bool AttachSpeaker(const uint16_t playerId);
	bool HasSpeaker(const uint16_t playerId);
	bool DetachSpeaker(const uint16_t playerId);
	void DetachAllSpeakers(std::vector<uint16_t>& detachedSpeakers);

	virtual ~Stream();

};
