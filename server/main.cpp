/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include <pawn/amx/amx.h>
#include <pawn/plugincommon.h>

#include "Header.h"
#include "ControlPacket.h"
#include "VoicePacket.h"

#include "Pawn.h"
#include "Network.h"
#include "PlayerStore.h"
#include "Worker.h"

#include "Stream.h"
#include "GlobalStream.h"
#include "LocalStream.h"
#include "PointStream.h"
#include "DynamicStream.h"
#include "StaticLocalStreamAtPoint.h"
#include "StaticLocalStreamAtVehicle.h"
#include "StaticLocalStreamAtPlayer.h"
#include "StaticLocalStreamAtObject.h"
#include "DynamicLocalStreamAtPoint.h"
#include "DynamicLocalStreamAtVehicle.h"
#include "DynamicLocalStreamAtPlayer.h"
#include "DynamicLocalStreamAtObject.h"

#include <atomic>
#include <ctime>
#include <vector>
#include <map>
#include <set>

#include <util/logger.h>

#ifndef _WIN32
#define __forceinline __attribute__((always_inline))
#endif

namespace SV
{
    uint32_t bitrate { SV::DefaultBitrate };
    std::map<uint32_t, Stream*> streamTable;
    std::set<DynamicStream*> dlstreamList;
    std::vector<WorkerPtr> workers;

    class PawnHandler : public PawnInterface {
    public:

        void SvInit(const uint32_t bitrate) override
        {
            SV::bitrate = bitrate;
        }

        uint8_t SvGetVersion(const uint16_t playerId) override
        {
            uint8_t playerPluginVersion { NULL };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) playerPluginVersion = pPlayerInfo->pluginVersion;
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return playerPluginVersion;
        }

        // -------------------------------------------------------------------------------------

        bool SvHasMicro(const uint16_t playerId) override
        {
            bool playerHasMicroStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) playerHasMicroStatus = pPlayerInfo->microStatus;
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return playerHasMicroStatus;
        }

        bool SvStartRecord(const uint16_t playerId) override
        {
            bool prevRecordStatus { true };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) prevRecordStatus = pPlayerInfo->recordStatus.exchange(true);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            if (prevRecordStatus) return false;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::startRecord, NULL);

            return Network::SendControlPacket(playerId, *controlPacket);
        }

        bool SvStopRecord(const uint16_t playerId) override
        {
            bool prevRecordStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) prevRecordStatus = pPlayerInfo->recordStatus.exchange(false);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            if (!prevRecordStatus) return false;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::stopRecord, NULL);

            return Network::SendControlPacket(playerId, *controlPacket);
        }

        // -------------------------------------------------------------------------------------

        bool SvAddKey(const uint16_t playerId, const uint8_t keyId) override
        {
            bool addKeyStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo) addKeyStatus = pPlayerInfo->keys.insert(keyId).second;
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            if (!addKeyStatus) return false;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::addKey, sizeof(SV::AddKeyPacket));
            PackGetStruct(controlPacket, SV::AddKeyPacket)->keyId = keyId;

            return Network::SendControlPacket(playerId, *controlPacket);
        }

        bool SvHasKey(const uint16_t playerId, const uint8_t keyId) override
        {
            bool hasKeyStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) hasKeyStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return hasKeyStatus;
        }

        bool SvRemoveKey(const uint16_t playerId, const uint8_t keyId) override
        {
            bool removeKeyStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo) removeKeyStatus = pPlayerInfo->keys.erase(keyId);
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            if (!removeKeyStatus) return false;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::removeKey, sizeof(SV::RemoveKeyPacket));
            PackGetStruct(controlPacket, SV::RemoveKeyPacket)->keyId = keyId;

            return Network::SendControlPacket(playerId, *controlPacket);
        }

        void SvRemoveAllKeys(const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo) pPlayerInfo->keys.clear();
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            if (!pPlayerInfo) return;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::removeAllKeys, NULL);

            Network::SendControlPacket(playerId, *controlPacket);
        }

        // -------------------------------------------------------------------------------------

        bool SvMutePlayerStatus(const uint16_t playerId) override
        {
            bool mutePlayerStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) mutePlayerStatus = pPlayerInfo->muteStatus.load();
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return mutePlayerStatus;
        }

        void SvMutePlayerEnable(const uint16_t playerId) override
        {
            bool prevMutePlayerStatus { true };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) prevMutePlayerStatus = pPlayerInfo->muteStatus.exchange(true);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            if (prevMutePlayerStatus) return;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::muteEnable, NULL);

            Network::SendControlPacket(playerId, *controlPacket);
        }

        void SvMutePlayerDisable(const uint16_t playerId) override
        {
            bool prevMutePlayerStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) prevMutePlayerStatus = pPlayerInfo->muteStatus.exchange(false);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            if (!prevMutePlayerStatus) return;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::muteDisable, NULL);

            Network::SendControlPacket(playerId, *controlPacket);
        }

        // -------------------------------------------------------------------------------------

        Stream* SvCreateGStream(const uint32_t color, const std::string& name) override
        {
            auto stream = new GlobalStream(color, name);
            if (!stream) return nullptr;

            auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        // -------------------------------------------------------------------------------------

        Stream* SvCreateSLStreamAtPoint(
            const float distance,
            const float posx,
            const float posy,
            const float posz,
            const uint32_t color,
            const std::string& name
        ) override
        {
            auto stream = new StaticLocalStreamAtPoint(distance, CVector(posx, posy, posz), color, name);
            if (!stream) return nullptr;

            auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateSLStreamAtVehicle(
            const float distance,
            const uint16_t vehicleId,
            const uint32_t color,
            const std::string& name
        ) override
        {
            if (!pNetGame->pVehiclePool->pVehicle[vehicleId])
                return nullptr;

            auto stream = new StaticLocalStreamAtVehicle(distance, vehicleId, color, name);
            if (!stream) return nullptr;

            auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateSLStreamAtPlayer(
            const float distance,
            const uint16_t playerId,
            const uint32_t color,
            const std::string& name
        ) override
        {
            if (!pNetGame->pPlayerPool->pPlayer[playerId])
                return nullptr;

            auto stream = new StaticLocalStreamAtPlayer(distance, playerId, color, name);
            if (!stream) return nullptr;

            auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateSLStreamAtObject(
            const float distance,
            const uint16_t objectId,
            const uint32_t color,
            const std::string& name
        ) override
        {
            if (!pNetGame->pObjectPool->pObjects[objectId])
                return nullptr;

            auto stream = new StaticLocalStreamAtObject(distance, objectId, color, name);
            if (!stream) return nullptr;

            auto baseStream = static_cast<Stream*>(stream);

            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        // -------------------------------------------------------------------------------------

        Stream* SvCreateDLStreamAtPoint(
            const float distance,
            const uint32_t maxPlayers,
            const float posx,
            const float posy,
            const float posz,
            const uint32_t color,
            const std::string& name
        ) override
        {
            auto stream = new DynamicLocalStreamAtPoint(distance, maxPlayers, CVector(posx, posy, posz), color, name);
            if (!stream) return nullptr;

            auto baseStream = static_cast<Stream*>(stream);

            SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateDLStreamAtVehicle(
            const float distance,
            const uint32_t maxPlayers,
            const uint16_t vehicleId,
            const uint32_t color,
            const std::string& name
        ) override
        {
            if (!pNetGame->pVehiclePool->pVehicle[vehicleId])
                return nullptr;

            auto stream = new DynamicLocalStreamAtVehicle(distance, maxPlayers, vehicleId, color, name);
            if (!stream) return nullptr;

            auto baseStream = static_cast<Stream*>(stream);

            SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateDLStreamAtPlayer(
            const float distance,
            const uint32_t maxPlayers,
            const uint16_t playerId,
            const uint32_t color,
            const std::string& name
        ) override
        {
            if (!pNetGame->pPlayerPool->pPlayer[playerId])
                return nullptr;

            auto stream = new DynamicLocalStreamAtPlayer(distance, maxPlayers, playerId, color, name);
            if (!stream) return nullptr;

            auto baseStream = static_cast<Stream*>(stream);

            SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        Stream* SvCreateDLStreamAtObject(
            const float distance,
            const uint32_t maxPlayers,
            const uint16_t objectId,
            const uint32_t color,
            const std::string& name
        ) override
        {
            if (!pNetGame->pObjectPool->pObjects[objectId])
                return nullptr;

            auto stream = new DynamicLocalStreamAtObject(distance, maxPlayers, objectId, color, name);
            if (!stream) return nullptr;

            auto baseStream = static_cast<Stream*>(stream);

            SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
            SV::streamTable.emplace(reinterpret_cast<uint32_t>(baseStream), baseStream);

            return baseStream;
        }

        // -------------------------------------------------------------------------------------

        void SvUpdatePositionForLPStream(PointStream* const lpStream, const float posx, const float posy, const float posz) override
        {
            lpStream->UpdatePosition(CVector(posx, posy, posz));
        }

        void SvUpdateDistanceForLStream(LocalStream* const lStream, const float distance) override
        {
            lStream->UpdateDistance(distance);
        }

        // -------------------------------------------------------------------------------------

        bool SvAttachListenerToStream(Stream* const stream, const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) pPlayerInfo->listenerStreams.insert(stream);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return stream->AttachListener(playerId);
        }

        bool SvHasListenerInStream(Stream* const stream, const uint16_t playerId) override
        {
            return stream->HasListener(playerId);
        }

        bool SvDetachListenerFromStream(Stream* const stream, const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo) pPlayerInfo->listenerStreams.erase(stream);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return stream->DetachListener(playerId);
        }

        void SvDetachAllListenersFromStream(Stream* const stream) override
        {
            const auto detachedListeners = stream->DetachAllListeners();

            for (const auto playerId : detachedListeners)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
                if (pPlayerInfo) pPlayerInfo->listenerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithSharedAccess(playerId);
            }
        }

        // -------------------------------------------------------------------------------------

        bool SvAttachSpeakerToStream(Stream* const stream, const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo) pPlayerInfo->speakerStreams.insert(stream);
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            return stream->AttachSpeaker(playerId);
        }

        bool SvHasSpeakerInStream(Stream* const stream, const uint16_t playerId) override
        {
            return stream->HasSpeaker(playerId);
        }

        bool SvDetachSpeakerFromStream(Stream* const stream, const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo) pPlayerInfo->speakerStreams.erase(stream);
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            return stream->DetachSpeaker(playerId);
        }

        void SvDetachAllSpeakersFromStream(Stream* const stream) override
        {
            const auto detachedSpeakers = stream->DetachAllSpeakers();

            for (const auto playerId : detachedSpeakers)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
                if (pPlayerInfo) pPlayerInfo->speakerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithUniqueAccess(playerId);
            }
        }

        // -------------------------------------------------------------------------------------

        void SvDeleteStream(Stream* const stream) override
        {
            const auto detachedSpeakers = stream->DetachAllSpeakers();

            for (const auto playerId : detachedSpeakers)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
                if (pPlayerInfo) pPlayerInfo->speakerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithUniqueAccess(playerId);
            }

            const auto detachedListeners = stream->DetachAllListeners();

            for (const auto playerId : detachedListeners)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
                if (pPlayerInfo) pPlayerInfo->listenerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithSharedAccess(playerId);
            }

            SV::streamTable.erase((uint32_t)(stream));
            if (const auto dlStream = dynamic_cast<DynamicStream*>(stream))
                SV::dlstreamList.erase(dlStream);

            delete stream;
        }

    };

    void ConnectHandler(const uint16_t playerId, const SV::ConnectPacket& connectStruct)
    {
        PlayerStore::AddPlayerToStore(playerId, connectStruct.version, connectStruct.micro);
    }

    void PlayerInitHandler(const uint16_t playerId, SV::PluginInitPacket& initStruct)
    {
        initStruct.mute = false;
        initStruct.bitrate = SV::bitrate;

        const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
        if (pPlayerInfo) initStruct.mute = pPlayerInfo->muteStatus.load();
        PlayerStore::ReleasePlayerWithSharedAccess(playerId);
    }

    void DisconnectHandler(const uint16_t playerId)
    {
        PlayerStore::RemovePlayerFromStore(playerId);
    }

    static __forceinline void Tick(const int64_t curTime)
    {
        for (const auto dlStream : SV::dlstreamList)
            dlStream->Tick();

        uint16_t senderId { SV::NonePlayer };

        while (const auto controlPacket = Network::ReceiveControlPacket(senderId))
        {
            const auto& controlPacketRef = *controlPacket;

            switch (controlPacketRef->packet)
            {
                case SV::ControlPacketType::pressKey:
                {
                    const auto stData = PackGetStruct(&controlPacketRef, SV::PressKeyPacket);
                    if (controlPacketRef->length != sizeof(*stData)) break;

                    const auto keyId = stData->keyId;
                    bool pressKeyAllowStatus { false };

                    const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(senderId);
                    if (pPlayerInfo) pressKeyAllowStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
                    PlayerStore::ReleasePlayerWithSharedAccess(senderId);

                    if (!pressKeyAllowStatus) break;

                    Pawn::OnPlayerActivationKeyPressForAll(senderId, keyId);
                } break;
                case SV::ControlPacketType::releaseKey:
                {
                    const auto stData = PackGetStruct(&controlPacketRef, SV::ReleaseKeyPacket);
                    if (controlPacketRef->length != sizeof(*stData)) break;

                    const auto keyId = stData->keyId;
                    bool releaseKeyAllowStatus { false };

                    const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(senderId);
                    if (pPlayerInfo) releaseKeyAllowStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
                    PlayerStore::ReleasePlayerWithSharedAccess(senderId);

                    if (!releaseKeyAllowStatus) break;

                    Pawn::OnPlayerActivationKeyReleaseForAll(senderId, keyId);
                } break;
            }
        }

        Network::Process(curTime);
    }
}

// --------------------------------------------------------------------

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
    SV::Tick(GetTimestamp());
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    static bool unloadStatus { false };
    if (unloadStatus) return;
    unloadStatus = true;

    Logger::Log(" -------------------------------------------");
    Logger::Log("           SampVoice unloading...           ");
    Logger::Log(" -------------------------------------------");

    SV::workers.clear();

    PlayerStore::ClearStore();

    Pawn::Free();
    RakNet::Free();
    Network::Free();
    Logger::Free();
}

#ifdef _WIN32
BOOL WINAPI WinExitHandler(DWORD CtrlType) { Unload(); return FALSE; }
#endif

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData)
{
#ifdef _WIN32
    SetConsoleCtrlHandler(&WinExitHandler, TRUE);
#endif

    ppPluginData = ppData;
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t)(ppData[PLUGIN_DATA_LOGPRINTF]);

    if (!Logger::Init(SV::LogFileName, logprintf))
    {
        logprintf("[sv:err:main:Load] : failed to init logger");
        return false;
    }

    if (!Network::Init(logprintf, SV::ConnectHandler, SV::PlayerInitHandler, SV::DisconnectHandler))
    {
        Logger::Log("[sv:err:main:Load] : failed to init network");
        Logger::Free();
        return false;
    }

    if (!Pawn::Init(std::make_unique<SV::PawnHandler>()))
    {
        Logger::Log("[sv:err:main:Load] : failed to init pawn");
        Network::Free();
        Logger::Free();
        return false;
    }

    {
        auto nprocs = std::thread::hardware_concurrency();

        if (!nprocs || nprocs > SV::VoiceThreadsCount)
            nprocs = SV::VoiceThreadsCount;

        Logger::Log("[sv:dbg:main:Load] : creating %u work threads...", nprocs);

        SV::workers.reserve(nprocs); for (auto i = nprocs; i > 0; --i)
            SV::workers.emplace_back(MakeWorker());
    }

    Logger::Log(" -------------------------------------------    ");
    Logger::Log("   ___                __   __    _              ");
    Logger::Log("  / __| __ _ _ __  _ _\\ \\ / /__ (_) __ ___    ");
    Logger::Log("  \\__ \\/ _` | '  \\| '_ \\   / _ \\| |/ _/ -_)");
    Logger::Log("  |___/\\__,_|_|_|_| .__/\\_/\\___/|_|\\__\\___|");
    Logger::Log("                  |_|                           ");
    Logger::Log(" -------------------------------------------    ");
    Logger::Log("           SampVoice by MOR loaded              ");
    Logger::Log(" -------------------------------------------    ");

    return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx)
{
    if (!pNetGame && (pNetGame = static_cast<CNetGame*(*)()>(ppPluginData[PLUGIN_DATA_NETGAME])()))
        Logger::Log("[sv:dbg:main:AmxLoad] : net game pointer (value:%p) received", pNetGame);

    if (!Network::Bind()) Logger::Log("[sv:dbg:main:AmxLoad] : failed to bind voice server");

    Pawn::RegisterScript(amx);

    return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx)
{
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}
