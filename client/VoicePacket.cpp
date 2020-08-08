/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "VoicePacket.h"

static inline DWORD CalcCrc32cHash(BYTE* buffer, DWORD length, DWORD crc = 0) {

	crc = ~crc;

	while (length--) {

		crc ^= *buffer++;

		for (int k = 0; k < 8; ++k) crc = crc & 1 ? (crc >> 1) ^ 0x82f63b78 : crc >> 1;

	}

	return ~crc;

}

DWORD VoicePacket::GetFullSize() const {

	return sizeof(*this) + this->length;

}

bool VoicePacket::CheckHeader() const {

	return this->hash == CalcCrc32cHash(
		(BYTE*)(this) + sizeof(this->hash),
		sizeof(*this) - sizeof(this->hash)
	);

}

void VoicePacket::CalcHash() {

	this->hash = CalcCrc32cHash(
		(BYTE*)(this) + sizeof(this->hash),
		sizeof(*this) - sizeof(this->hash)
	);

}
