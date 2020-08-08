/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Stream.h"

#include <ysf/globals.h>

#include "Network.h"
#include "PlayerStore.h"
#include "Header.h"

Stream::Stream() {

	if (PackWrap(this->packetDeleteStream, SV::ControlPacketType::deleteStream, sizeof(SV::DeleteStreamPacket)))
		PackGetStruct(&*this->packetDeleteStream, SV::DeleteStreamPacket)->stream = (uint32_t)(this);

}

void Stream::PushVoicePacket(VoicePacket& voicePacket) {

	if (!this->HasSpeaker(voicePacket.sender)) return;

	voicePacket.stream = (uint32_t)(this);
	voicePacket.CalcHash();

	const auto playerPoolSize = pNetGame->pPlayerPool->dwPlayerPoolSize;
	const auto connectedPlayers = pNetGame->pPlayerPool->dwConnectedPlayers;

	if (connectedPlayers) for (uint16_t iPlayerId = 0; iPlayerId <= playerPoolSize; ++iPlayerId)
		if (this->attachedListeners[iPlayerId].load() && PlayerStore::IsPlayerHasPlugin(iPlayerId) &&
			iPlayerId != voicePacket.sender) Network::SendVoicePacket(iPlayerId, voicePacket);

}

bool Stream::AttachListener(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return false;

	if (!this->packetCreateStream || !this->packetDeleteStream) return false;

	if (!PlayerStore::IsPlayerHasPlugin(playerId)) return false;
	if (this->attachedListeners[playerId].exchange(true)) return false;

	Network::SendControlPacket(playerId, *&*this->packetCreateStream);

	++this->attachedListenersCount;

	return true;

}

bool Stream::HasListener(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return false;

	return this->attachedListeners[playerId].load();

}

bool Stream::DetachListener(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return false;

	if (!this->attachedListeners[playerId].exchange(false)) return false;

	if (PlayerStore::IsPlayerHasPlugin(playerId) && this->packetDeleteStream)
		Network::SendControlPacket(playerId, *&*this->packetDeleteStream);

	--this->attachedListenersCount;

	return true;

}

void Stream::DetachAllListeners(std::vector<uint16_t>& detachedListeners) {

	detachedListeners.clear();
	detachedListeners.reserve(this->attachedListenersCount);

	for (uint16_t iPlayerId = 0; iPlayerId < MAX_PLAYERS; ++iPlayerId)
		if (this->attachedListeners[iPlayerId].exchange(false)) {

			if (PlayerStore::IsPlayerHasPlugin(iPlayerId) && this->packetDeleteStream)
				Network::SendControlPacket(iPlayerId, *&*this->packetDeleteStream);

			detachedListeners.push_back(iPlayerId);

		}

	this->attachedListenersCount = 0;

}

bool Stream::AttachSpeaker(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return false;

	if (!PlayerStore::IsPlayerHasPlugin(playerId))
		return false;

	if (this->attachedSpeakers[playerId].exchange(true))
		return false;

	++this->attachedSpeakersCount;

	return true;

}

bool Stream::HasSpeaker(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return false;

	return this->attachedSpeakers[playerId].load();

}

bool Stream::DetachSpeaker(const uint16_t playerId) {

	if (playerId >= MAX_PLAYERS) return false;

	if (!this->attachedSpeakers[playerId].exchange(false))
		return false;

	--this->attachedSpeakersCount;

	return true;

}

void Stream::DetachAllSpeakers(std::vector<uint16_t>& detachedSpeakers) {

	detachedSpeakers.clear();
	detachedSpeakers.reserve(this->attachedSpeakersCount);

	for (uint16_t iPlayerId = 0; iPlayerId < MAX_PLAYERS; ++iPlayerId)
		if (this->attachedSpeakers[iPlayerId].exchange(false))
			detachedSpeakers.push_back(iPlayerId);

	this->attachedSpeakersCount = 0;

}

Stream::~Stream() {}
