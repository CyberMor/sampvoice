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

    Worker(const Worker&) = delete;
    Worker(Worker&&) = delete;
    Worker& operator=(const Worker&) = delete;
    Worker& operator=(Worker&&) = delete;

public:

    explicit Worker()
        : status(std::make_shared<std::atomic_bool>(true))
        , thread(std::make_unique<std::thread>(Worker::ThreadFunc, status))
    {}

    ~Worker()
    {
        if (this->thread->joinable())
            this->thread->detach();

        this->status->store(false);
    }

private:

    static void ThreadFunc(const std::shared_ptr<std::atomic_bool> status)
    {
        while (status->load(std::memory_order_relaxed))
        {
            const auto voicePacket = Network::ReceiveVoicePacket();
            if (voicePacket == nullptr) continue;

            auto& voicePacketRef = *voicePacket;

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(voicePacketRef->sender);

            if (pPlayerInfo != nullptr && !pPlayerInfo->muteStatus.load(std::memory_order_relaxed) &&
                (pPlayerInfo->recordStatus.load(std::memory_order_relaxed) || !pPlayerInfo->keys.empty()))
            {
                for (const auto stream : pPlayerInfo->speakerStreams)
                    stream->SendVoicePacket(*&voicePacketRef);
            }

            PlayerStore::ReleasePlayerWithSharedAccess(voicePacketRef->sender);
        }
    }

private:

    const std::shared_ptr<std::atomic_bool> status;
    const std::unique_ptr<std::thread> thread;

};

using WorkerPtr = std::unique_ptr<Worker>;
#define MakeWorker std::make_unique<Worker>
