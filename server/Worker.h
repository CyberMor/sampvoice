/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "Network.h"
#include "VoicePacket.h"
#include "PlayerStore.h"
#include "Header.h"

class Worker {
private:

	const std::shared_ptr<std::atomic_bool> status;
	const std::shared_ptr<std::thread> thread;

	static void ThreadFunc(
		std::shared_ptr<std::atomic_bool> status
	) {

		while (status->load()) {

			const auto voicePacket = Network::ReceiveVoicePacket();

			if (!voicePacket) continue;

			const auto voicePacketRef = *voicePacket;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(voicePacketRef->sender);

			if (pPlayerInfo && !pPlayerInfo->muteStatus.load() && (pPlayerInfo->recordStatus.load() || !pPlayerInfo->keys.empty()))
				for (const auto pStream : pPlayerInfo->speakerStreams) pStream->PushVoicePacket(*&voicePacketRef);

			PlayerStore::ReleasePlayerWithSharedAccess(voicePacketRef->sender);

		}

	}

private:

	Worker(
		const std::shared_ptr<std::atomic_bool>& status,
		const std::shared_ptr<std::thread>& thread
	) :
		status(status),
		thread(thread)
	{}

public:

	Worker() = delete;
	Worker(const Worker& object) = delete;
	Worker(Worker&& object) = delete;

	Worker& operator=(const Worker& object) = delete;
	Worker& operator=(Worker&& object) = delete;

	static std::shared_ptr<Worker> Create() {

		const auto status = std::make_shared<std::atomic_bool>(true);

		if (!status) return nullptr;

		const auto thread = std::make_shared<std::thread>(ThreadFunc, status);

		if (!thread) return nullptr;

		struct WorkerHack : Worker {

			WorkerHack(
				const std::shared_ptr<std::atomic_bool>& status,
				const std::shared_ptr<std::thread>& thread
			) : Worker(status, thread) {}

		};

		return std::make_shared<WorkerHack>(status, thread);

	}

	void Stop() const {

		this->status->store(false);

	}

	void Wait() const {

		if (this->thread->joinable())
			this->thread->join();

	}

	~Worker() {

		if (this->thread->joinable())
			this->thread->detach();

		this->status->store(false);

	}

};

using WorkerPtr = std::shared_ptr<Worker>;
