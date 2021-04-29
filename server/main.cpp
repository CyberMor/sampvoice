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

#include <util/timer.h>
#include <util/logger.h>

#ifndef _WIN32
#define __forceinline __attribute__((always_inline))
#endif

namespace SV
{
    uint32_t bitrate { SV::kDefaultBitrate };
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
            if (pPlayerInfo != nullptr) playerPluginVersion = pPlayerInfo->pluginVersion;
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return playerPluginVersion;
        }

        // -------------------------------------------------------------------------------------

        bool SvHasMicro(const uint16_t playerId) override
        {
            bool playerHasMicroStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) playerHasMicroStatus = pPlayerInfo->microStatus;
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return playerHasMicroStatus;
        }

        bool SvStartRecord(const uint16_t playerId) override
        {
            bool prevRecordStatus { true };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) prevRecordStatus = pPlayerInfo->recordStatus.exchange(true);
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
            if (pPlayerInfo != nullptr) prevRecordStatus = pPlayerInfo->recordStatus.exchange(false);
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
            if (pPlayerInfo != nullptr) addKeyStatus = pPlayerInfo->keys.insert(keyId).second;
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
            if (pPlayerInfo != nullptr) hasKeyStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return hasKeyStatus;
        }

        bool SvRemoveKey(const uint16_t playerId, const uint8_t keyId) override
        {
            bool removeKeyStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo != nullptr) removeKeyStatus = pPlayerInfo->keys.erase(keyId);
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
            if (pPlayerInfo != nullptr) pPlayerInfo->keys.clear();
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            if (pPlayerInfo == nullptr) return;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::removeAllKeys, NULL);

            Network::SendControlPacket(playerId, *controlPacket);
        }

        // -------------------------------------------------------------------------------------

        bool SvMutePlayerStatus(const uint16_t playerId) override
        {
            bool mutePlayerStatus { false };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) mutePlayerStatus = pPlayerInfo->muteStatus.load();
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return mutePlayerStatus;
        }

        void SvMutePlayerEnable(const uint16_t playerId) override
        {
            bool prevMutePlayerStatus { true };

            const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
            if (pPlayerInfo != nullptr) prevMutePlayerStatus = pPlayerInfo->muteStatus.exchange(true);
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
            if (pPlayerInfo != nullptr) prevMutePlayerStatus = pPlayerInfo->muteStatus.exchange(false);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            if (!prevMutePlayerStatus) return;

            ControlPacket* controlPacket { nullptr };

            PackAlloca(controlPacket, SV::ControlPacketType::muteDisable, NULL);

            Network::SendControlPacket(playerId, *controlPacket);
        }

        // -------------------------------------------------------------------------------------

        Stream* SvCreateGStream(const uint32_t color, const std::string& name) override
        {
            const auto stream = new (std::nothrow) GlobalStream(color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

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
            const auto stream = new (std::nothrow) StaticLocalStreamAtPoint(distance, CVector { posx, posy, posz }, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

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
            if (pNetGame->pVehiclePool->pVehicle[vehicleId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) StaticLocalStreamAtVehicle(distance, vehicleId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

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
            if (pNetGame->pPlayerPool->pPlayer[playerId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) StaticLocalStreamAtPlayer(distance, playerId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

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
            if (pNetGame->pObjectPool->pObjects[objectId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) StaticLocalStreamAtObject(distance, objectId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

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
            const auto stream = new (std::nothrow) DynamicLocalStreamAtPoint(distance, maxPlayers, CVector { posx, posy, posz }, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

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
            if (pNetGame->pVehiclePool->pVehicle[vehicleId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) DynamicLocalStreamAtVehicle(distance, maxPlayers, vehicleId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

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
            if (pNetGame->pPlayerPool->pPlayer[playerId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) DynamicLocalStreamAtPlayer(distance, maxPlayers, playerId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

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
            if (pNetGame->pObjectPool->pObjects[objectId] == nullptr)
                return nullptr;

            const auto stream = new (std::nothrow) DynamicLocalStreamAtObject(distance, maxPlayers, objectId, color, name);
            if (stream == nullptr) return nullptr;

            const auto baseStream = static_cast<Stream*>(stream);

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
            if (pPlayerInfo != nullptr) pPlayerInfo->listenerStreams.insert(stream);
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
            if (pPlayerInfo != nullptr) pPlayerInfo->listenerStreams.erase(stream);
            PlayerStore::ReleasePlayerWithSharedAccess(playerId);

            return stream->DetachListener(playerId);
        }

        void SvDetachAllListenersFromStream(Stream* const stream) override
        {
            const auto detachedListeners = stream->DetachAllListeners();

            for (const auto playerId : detachedListeners)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
                if (pPlayerInfo != nullptr) pPlayerInfo->listenerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithSharedAccess(playerId);
            }
        }

        // -------------------------------------------------------------------------------------

        bool SvAttachSpeakerToStream(Stream* const stream, const uint16_t playerId) override
        {
            const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
            if (pPlayerInfo != nullptr) pPlayerInfo->speakerStreams.insert(stream);
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
            if (pPlayerInfo != nullptr) pPlayerInfo->speakerStreams.erase(stream);
            PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

            return stream->DetachSpeaker(playerId);
        }

        void SvDetachAllSpeakersFromStream(Stream* const stream) override
        {
            const auto detachedSpeakers = stream->DetachAllSpeakers();

            for (const auto playerId : detachedSpeakers)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
                if (pPlayerInfo != nullptr) pPlayerInfo->speakerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithUniqueAccess(playerId);
            }
        }

        // -------------------------------------------------------------------------------------

        void SvStreamParameterSet(Stream* const stream, const uint8_t parameter, const float value) override
        {
            stream->SetParameter(parameter, value);
        }

        void SvStreamParameterReset(Stream* const stream, const uint8_t parameter) override
        {
            stream->ResetParameter(parameter);
        }

        bool SvStreamParameterHas(Stream* const stream, const uint8_t parameter) override
        {
            return stream->HasParameter(parameter);
        }

        float SvStreamParameterGet(Stream* const stream, const uint8_t parameter) override
        {
            return stream->GetParameter(parameter);
        }

        void SvStreamParameterSlideFromTo(Stream* const stream, const uint8_t parameter, const float startvalue, const float endvalue, const uint32_t time) override
        {
            stream->SlideParameterFromTo(parameter, startvalue, endvalue, time);
        }

        void SvStreamParameterSlideTo(Stream* const stream, const uint8_t parameter, const float endvalue, const uint32_t time) override
        {
            stream->SlideParameterTo(parameter, endvalue, time);
        }

        void SvStreamParameterSlide(Stream* const stream, const uint8_t parameter, const float deltavalue, const uint32_t time) override
        {
            stream->SlideParameter(parameter, deltavalue, time);
        }

        // -------------------------------------------------------------------------------------

        void SvDeleteStream(Stream* const stream) override
        {
            const auto detachedSpeakers = stream->DetachAllSpeakers();

            for (const auto playerId : detachedSpeakers)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
                if (pPlayerInfo != nullptr) pPlayerInfo->speakerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithUniqueAccess(playerId);
            }

            const auto detachedListeners = stream->DetachAllListeners();

            for (const auto playerId : detachedListeners)
            {
                const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
                if (pPlayerInfo != nullptr) pPlayerInfo->listenerStreams.erase(stream);
                PlayerStore::ReleasePlayerWithSharedAccess(playerId);
            }

            SV::streamTable.erase(reinterpret_cast<uint32_t>(stream));
            if (const auto dlStream = dynamic_cast<DynamicStream*>(stream))
                SV::dlstreamList.erase(dlStream);

            delete stream;
        }

        // -------------------------------------------------------------------------------------

        Effect* SvEffectCreateChorus(const int priority, const float wetdrymix, const float depth, const float feedback, const float frequency, const uint32_t waveform, const float delay, const uint32_t phase) override
        {
            return new (std::nothrow) Effect(SV::EffectType::chorus, priority, ChorusParameters { wetdrymix, depth, feedback, frequency, waveform, delay, phase });
        }

        Effect* SvEffectCreateCompressor(const int priority, const float gain, const float attack, const float release, const float threshold, const float ratio, const float predelay) override
        {
            return new (std::nothrow) Effect(SV::EffectType::compressor, priority, CompressorParameters { gain, attack, release, threshold, ratio, predelay });
        }

        Effect* SvEffectCreateDistortion(const int priority, const float gain, const float edge, const float posteqcenterfrequency, const float posteqbandwidth, const float prelowpasscutoff) override
        {
            return new (std::nothrow) Effect(SV::EffectType::distortion, priority, DistortionParameters { gain, edge, posteqcenterfrequency, posteqbandwidth, prelowpasscutoff });
        }

        Effect* SvEffectCreateEcho(const int priority, const float wetdrymix, const float feedback, const float leftdelay, const float rightdelay, const bool pandelay) override
        {
            return new (std::nothrow) Effect(SV::EffectType::echo, priority, EchoParameters { wetdrymix, feedback, leftdelay, rightdelay, pandelay });
        }

        Effect* SvEffectCreateFlanger(const int priority, const float wetdrymix, const float depth, const float feedback, const float frequency, const uint32_t waveform, const float delay, const uint32_t phase) override
        {
            return new (std::nothrow) Effect(SV::EffectType::flanger, priority, FlangerParameters { wetdrymix, depth, feedback, frequency, waveform, delay, phase });
        }

        Effect* SvEffectCreateGargle(const int priority, const uint32_t ratehz, const uint32_t waveshape) override
        {
            return new (std::nothrow) Effect(SV::EffectType::gargle, priority, GargleParameters { ratehz, waveshape });
        }

        Effect* SvEffectCreateI3dl2reverb(const int priority, const int room, const int roomhf, const float roomrollofffactor, const float decaytime, const float decayhfratio, const int reflections, const float reflectionsdelay, const int reverb, const float reverbdelay, const float diffusion, const float density, const float hfreference) override
        {
            return new (std::nothrow) Effect(SV::EffectType::i3dl2reverb, priority, I3dl2reverbParameters { room, roomhf, roomrollofffactor, decaytime, decayhfratio, reflections, reflectionsdelay, reverb, reverbdelay, diffusion, density, hfreference });
        }

        Effect* SvEffectCreateParameq(const int priority, const float center, const float bandwidth, const float gain) override
        {
            return new (std::nothrow) Effect(SV::EffectType::parameq, priority, ParameqParameters { center, bandwidth, gain });
        }

        Effect* SvEffectCreateReverb(const int priority, const float ingain, const float reverbmix, const float reverbtime, const float highfreqrtratio) override
        {
            return new (std::nothrow) Effect(SV::EffectType::reverb, priority, ReverbParameters { ingain, reverbmix, reverbtime, highfreqrtratio });
        }

        void SvEffectAttachStream(Effect* const effect, Stream* const stream) override
        {
            effect->AttachStream(stream);
        }

        void SvEffectDetachStream(Effect* const effect, Stream* const stream) override
        {
            effect->DetachStream(stream);
        }

        void SvEffectDelete(Effect* const effect) override
        {
            delete effect;
        }

    };

    void ConnectHandler(const uint16_t playerId, const SV::ConnectPacket& connectStruct) noexcept
    {
        PlayerStore::AddPlayerToStore(playerId, connectStruct.version, connectStruct.micro);
    }

    void PlayerInitHandler(const uint16_t playerId, SV::PluginInitPacket& initStruct) noexcept
    {
        initStruct.bitrate = SV::bitrate;

        const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
        if (pPlayerInfo != nullptr) initStruct.mute = pPlayerInfo->muteStatus.load(std::memory_order_relaxed);
        PlayerStore::ReleasePlayerWithSharedAccess(playerId);
    }

    void DisconnectHandler(const uint16_t playerId) noexcept
    {
        PlayerStore::RemovePlayerFromStore(playerId);
    }

    static __forceinline void Tick() noexcept
    {
        for (const auto dlStream : SV::dlstreamList)
            dlStream->Tick();

        uint16_t senderId { SV::kNonePlayer };

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
                    if (pPlayerInfo != nullptr) pressKeyAllowStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
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
                    if (pPlayerInfo != nullptr) releaseKeyAllowStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
                    PlayerStore::ReleasePlayerWithSharedAccess(senderId);

                    if (!releaseKeyAllowStatus) break;

                    Pawn::OnPlayerActivationKeyReleaseForAll(senderId, keyId);
                } break;
            }
        }

        Network::Process();
    }
}

// --------------------------------------------------------------------

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() noexcept
{
    Timer::Tick();
    SV::Tick();
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() noexcept
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
BOOL WINAPI WinExitHandler(DWORD) noexcept
{
    Unload();
    return FALSE;
}
#endif

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** const ppData) noexcept
{
#ifdef _WIN32
    SetConsoleCtrlHandler(&WinExitHandler, TRUE);
#endif

    ppPluginData = ppData;
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
    logprintf = (logprintf_t)(ppData[PLUGIN_DATA_LOGPRINTF]);

    if (!Logger::Init(SV::kLogFileName, logprintf))
    {
        logprintf("[sv:err:main:Load] : failed to init logger");
        return false;
    }

    if (!Network::Init(logprintf))
    {
        Logger::Log("[sv:err:main:Load] : failed to init network");
        Logger::Free();
        return false;
    }

    Network::AddConnectCallback(SV::ConnectHandler);
    Network::AddPlayerInitCallback(SV::PlayerInitHandler);
    Network::AddDisconnectCallback(SV::DisconnectHandler);

    if (!Pawn::Init(std::make_unique<SV::PawnHandler>()))
    {
        Logger::Log("[sv:err:main:Load] : failed to init pawn");
        Network::Free();
        Logger::Free();
        return false;
    }

    {
        auto nprocs = std::thread::hardware_concurrency();

        if (!nprocs || nprocs > SV::kVoiceThreadsCount)
            nprocs = SV::kVoiceThreadsCount;

        Logger::Log("[sv:dbg:main:Load] : creating %u work threads...", nprocs);

        SV::workers.reserve(nprocs); for (auto i { nprocs }; i > 0; --i)
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
    Logger::Log("             edited by ZTzTopia                 ");
    Logger::Log(" -------------------------------------------    ");

    return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* const amx) noexcept
{
    if (pNetGame == nullptr && (pNetGame = reinterpret_cast<CNetGame*(*)()>(ppPluginData[PLUGIN_DATA_NETGAME])()) != nullptr)
        Logger::Log("[sv:dbg:main:AmxLoad] : net game pointer (value:%p) received", pNetGame);

    if (!Network::Bind()) Logger::Log("[sv:dbg:main:AmxLoad] : failed to bind voice server");

    Pawn::RegisterScript(amx);

    return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX*) noexcept
{
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() noexcept
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}
