/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <set>
#include <vector>
#include <cstdint>

#include "LocalStream.h"

class DynamicStream : public virtual LocalStream {
protected:

	struct PlayerSortInfo {

		const float distance;
		const uint32_t playerId;

		PlayerSortInfo(float distance, uint32_t playerId) :
			distance(distance), playerId(playerId) {}

		bool operator<(const PlayerSortInfo& object) const {
			return this->distance < object.distance;
		}

	};

	using PlayerSortList = std::multiset<PlayerSortInfo>;

protected:

	const uint32_t maxPlayers;

protected:

	DynamicStream(const float distance, const uint32_t maxPlayers);

public:

	DynamicStream() = delete;
	DynamicStream(const DynamicStream& object) = delete;
	DynamicStream(DynamicStream&& object) = delete;

	DynamicStream& operator=(const DynamicStream& object) = delete;
	DynamicStream& operator=(DynamicStream&& object) = delete;

	virtual void Tick() = 0;

	bool AttachListener(const uint16_t playerId) override;
	bool DetachListener(const uint16_t playerId) override;
	void DetachAllListeners(std::vector<uint16_t>& detachedListeners) override;

	virtual ~DynamicStream();

};
