/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <functional>

#include <audio/bass.h>
#include <audio/opus.h>

#include <util/Logger.h>

#include "Header.h"

class Channel {
public:

	using PlayHandlerType = std::function<void(const Channel& channel)>;
	using StopHandlerType = std::function<void(const Channel& channel)>;

public:

	const HSTREAM handle;
	WORD speaker = SV::NonePlayer;

private:

	const PlayHandlerType playHandler;
	const StopHandlerType stopHandler;

	OpusDecoder* const decoder;
	opus_int16 decBuffer[SV::FrameSizeInSamples];

	DWORD expectedPacketNumber = 0;
	bool initialized = false;
	bool playing = false;

	int opusErrorCode = -1;

public:

	Channel() = delete;
	Channel(const Channel& object) = delete;
	Channel(Channel&& object) = delete;

	Channel& operator=(const Channel& object) = delete;
	Channel& operator=(Channel&& object) = delete;

	Channel(
		const DWORD channelFlags,
		const PlayHandlerType& playHandler,
		const StopHandlerType& stopHandler,
		const WORD speaker = SV::NonePlayer
	) :

		handle(BASS_StreamCreate(SV::Frequency, 1, channelFlags, STREAMPROC_PUSH, nullptr)),
		decoder(opus_decoder_create(SV::Frequency, 1, &opusErrorCode)),

		playHandler(playHandler),
		stopHandler(stopHandler),
		speaker(speaker)

	{

		if (!this->handle) Logger::LogToFile("[sv:err:channel_%p] : failed to create bass channel (code:%d)", this, BASS_ErrorGetCode());
		if (!this->decoder) Logger::LogToFile("[sv:err:channel_%p] : failed to create opus decoder (code:%d)", this, this->opusErrorCode);

		if (!this->handle || !this->decoder) {

			if (this->decoder) opus_decoder_destroy(this->decoder);
			if (this->handle) BASS_StreamFree(this->handle);

			throw std::bad_alloc();

		}

	}

	bool IsActive() const {

		const auto bufferSize = BASS_ChannelGetData(this->handle, nullptr, BASS_DATA_AVAILABLE);

		return bufferSize != -1 && bufferSize;

	}

	void Reset() {

		BASS_ChannelPause(this->handle);
		BASS_ChannelSetPosition(this->handle, 0, BASS_POS_BYTE);
		opus_decoder_ctl(this->decoder, OPUS_RESET_STATE);

		if (this->playing && this->stopHandler) {

			this->stopHandler(*this);
			this->playing = false;

		}

		this->speaker = SV::NonePlayer;
		this->expectedPacketNumber = 0;
		this->initialized = false;

	}

	void Push(
		const DWORD packetNumber,
		const BYTE* packetDataPtr,
		const DWORD packetDataSize
	) {

		if (!this->initialized || !packetNumber) {

			Logger::LogToFile("[sv:dbg:channel_%p:push] : init channel", this);

			BASS_ChannelPause(this->handle);
			BASS_ChannelSetPosition(this->handle, 0, BASS_POS_BYTE);
			opus_decoder_ctl(this->decoder, OPUS_RESET_STATE);

			if (this->playing && this->stopHandler) {

				this->stopHandler(*this);
				this->playing = false;

			}

			this->initialized = true;

		} else if (packetNumber < this->expectedPacketNumber) {

			Logger::LogToFile(
				"[sv:dbg:channel_%p:push] : late packet to channel (pack:%u;expPack:%u)",
				this, packetNumber, this->expectedPacketNumber
			);

			return;

		} else if (packetNumber > this->expectedPacketNumber) {

			Logger::LogToFile(
				"[sv:dbg:channel_%p:push] : lost packet to channel (pack:%u;expPack:%u)",
				this, packetNumber, this->expectedPacketNumber
			);

			int length = opus_decode(this->decoder, packetDataPtr, packetDataSize, this->decBuffer, SV::FrameSizeInSamples, true);
			if (length == SV::FrameSizeInSamples) BASS_StreamPutData(this->handle, this->decBuffer, SV::FrameSizeInBytes);

		}

		int length = opus_decode(this->decoder, packetDataPtr, packetDataSize, this->decBuffer, SV::FrameSizeInSamples, false);
		if (length == SV::FrameSizeInSamples) BASS_StreamPutData(this->handle, this->decBuffer, SV::FrameSizeInBytes);

		const auto channelStatus = BASS_ChannelIsActive(this->handle);
		const auto bufferSize = BASS_ChannelGetData(this->handle, nullptr, BASS_DATA_AVAILABLE);

		if ((channelStatus == BASS_ACTIVE_PAUSED || channelStatus == BASS_ACTIVE_STOPPED) &&
			bufferSize != -1 && bufferSize >= SV::ChannelPreBufferFramesCount * SV::FrameSizeInBytes
		) {

			Logger::LogToFile("[sv:dbg:channel_%p:push] : playing channel", this);

			BASS_ChannelPlay(this->handle, FALSE);

			if (!this->playing && this->playHandler) {

				this->playHandler(*this);
				this->playing = true;

			}

		}

		this->expectedPacketNumber = packetNumber + 1;

	}

	~Channel() {

		if (this->playing && this->stopHandler)
			this->stopHandler(*this);

		opus_decoder_destroy(this->decoder);
		BASS_StreamFree(this->handle);

	}

};

using ChannelPtr = std::shared_ptr<Channel>;
#define MakeChannel std::make_shared<Channel>
