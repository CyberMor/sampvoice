/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "StaticLocalStreamAtPlayer.h"

#include <string.h>

#include "ControlPacket.h"
#include "Header.h"

StaticLocalStreamAtPlayer::StaticLocalStreamAtPlayer(
	const float distance, const uint16_t playerId,
	const uint32_t color, const std::string& name
) :
	LocalStream(distance)
{

	const auto nameString = name.c_str();
	const auto nameLength = name.size() + 1;

	if (PackWrap(this->packetCreateStream, SV::ControlPacketType::createLStreamAtPlayer, sizeof(SV::CreateLStreamAtPacket) + nameLength)) {

		PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->stream = (uint32_t)(static_cast<Stream*>(this));
		memcpy(PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->name, nameString, nameLength);
		PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->distance = distance;
		PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->target = playerId;
		PackGetStruct(&*this->packetCreateStream, SV::CreateLStreamAtPacket)->color = color;

	}

}
