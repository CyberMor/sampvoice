/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "DynamicStream.h"

DynamicStream::DynamicStream(const float distance, const uint32_t maxPlayers) :
	LocalStream(distance), maxPlayers(maxPlayers) {}

bool DynamicStream::AttachListener(const uint16_t playerId) { return false; }
bool DynamicStream::DetachListener(const uint16_t playerId) { return false; }
void DynamicStream::DetachAllListeners(std::vector<uint16_t>& detachedListeners) { detachedListeners.clear(); }

DynamicStream::~DynamicStream() {}
