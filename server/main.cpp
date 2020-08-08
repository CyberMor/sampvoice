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

#include <set>
#include <vector>
#include <atomic>
#include <ctime>
#include <map>

#include <util/logger.h>

#ifndef _WIN32
#define __forceinline __attribute__((always_inline))
#endif

namespace SV {

	uint32_t bitrate = SV::DefaultBitrate;
	std::map<uint32_t, Stream*> streamTable;
	std::set<DynamicStream*> dlstreamList;
	std::vector<WorkerPtr> workers;

	namespace PawnHandlers {

		static void InitHandler(const uint32_t bitrate) {

			SV::bitrate = bitrate;

		}

		uint8_t GetVersionHandler(const uint16_t playerId) {

			uint8_t playerPluginVersion = NULL;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) playerPluginVersion = pPlayerInfo->pluginVersion;
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			return playerPluginVersion;

		}



		bool HasMicroHandler(const uint16_t playerId) {

			bool playerHasMicroStatus = false;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) playerHasMicroStatus = pPlayerInfo->microStatus;
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			return playerHasMicroStatus;

		}

		bool StartRecordHandler(const uint16_t playerId) {

			bool prevRecordStatus = true;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) prevRecordStatus = pPlayerInfo->recordStatus.exchange(true);
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			if (prevRecordStatus) return false;

			ControlPacket* controlPacket = nullptr;

			PackAlloca(controlPacket, SV::ControlPacketType::startRecord, NULL);

			return Network::SendControlPacket(playerId, *controlPacket);

		}

		bool StopRecordHandler(const uint16_t playerId) {

			bool prevRecordStatus = false;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) prevRecordStatus = pPlayerInfo->recordStatus.exchange(false);
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			if (!prevRecordStatus) return false;

			ControlPacket* controlPacket = nullptr;

			PackAlloca(controlPacket, SV::ControlPacketType::stopRecord, NULL);

			return Network::SendControlPacket(playerId, *controlPacket);

		}



		bool AddKeyHandler(const uint16_t playerId, const uint8_t keyId) {

			bool addKeyStatus = false;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
			if (pPlayerInfo) addKeyStatus = pPlayerInfo->keys.insert(keyId).second;
			PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

			if (!addKeyStatus) return false;

			ControlPacket* controlPacket = nullptr;

			PackAlloca(controlPacket, SV::ControlPacketType::addKey, sizeof(SV::AddKeyPacket));
			PackGetStruct(controlPacket, SV::AddKeyPacket)->keyId = keyId;

			return Network::SendControlPacket(playerId, *controlPacket);

		}

		bool HasKeyHandler(const uint16_t playerId, const uint8_t keyId) {

			bool hasKeyStatus = false;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) hasKeyStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			return hasKeyStatus;

		}

		bool RemoveKeyHandler(const uint16_t playerId, const uint8_t keyId) {

			bool removeKeyStatus = false;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
			if (pPlayerInfo) removeKeyStatus = pPlayerInfo->keys.erase(keyId);
			PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

			if (!removeKeyStatus) return false;

			ControlPacket* controlPacket = nullptr;

			PackAlloca(controlPacket, SV::ControlPacketType::removeKey, sizeof(SV::RemoveKeyPacket));
			PackGetStruct(controlPacket, SV::RemoveKeyPacket)->keyId = keyId;

			return Network::SendControlPacket(playerId, *controlPacket);

		}

		void RemoveAllKeysHandler(const uint16_t playerId) {

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
			if (pPlayerInfo) pPlayerInfo->keys.clear();
			PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

			if (!pPlayerInfo) return;

			ControlPacket* controlPacket = nullptr;

			PackAlloca(controlPacket, SV::ControlPacketType::removeAllKeys, NULL);

			Network::SendControlPacket(playerId, *controlPacket);

		}



		bool MutePlayerStatusHandler(const uint16_t playerId) {

			bool mutePlayerStatus = false;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) mutePlayerStatus = pPlayerInfo->muteStatus.load();
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			return mutePlayerStatus;

		}

		void MutePlayerEnableHandler(const uint16_t playerId) {

			bool prevMutePlayerStatus = true;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) prevMutePlayerStatus = pPlayerInfo->muteStatus.exchange(true);
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			if (prevMutePlayerStatus) return;

			ControlPacket* controlPacket = nullptr;

			PackAlloca(controlPacket, SV::ControlPacketType::muteEnable, NULL);

			Network::SendControlPacket(playerId, *controlPacket);

		}

		void MutePlayerDisableHandler(const uint16_t playerId) {

			bool prevMutePlayerStatus = false;

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) prevMutePlayerStatus = pPlayerInfo->muteStatus.exchange(false);
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			if (!prevMutePlayerStatus) return;

			ControlPacket* controlPacket = nullptr;

			PackAlloca(controlPacket, SV::ControlPacketType::muteDisable, NULL);

			Network::SendControlPacket(playerId, *controlPacket);

		}



		Stream* CreateGStreamHandler(const uint32_t color, const std::string& name) {

			auto stream = new GlobalStream(color, name);
			if (!stream) return nullptr;

			auto baseStream = static_cast<Stream*>(stream);

			SV::streamTable.emplace((uint32_t)(baseStream), baseStream);

			return baseStream;

		}



		Stream* CreateSLStreamAtPointHandler(
			const float distance,
			const float posx,
			const float posy,
			const float posz,
			const uint32_t color,
			const std::string& name
		) {

			auto stream = new StaticLocalStreamAtPoint(distance, CVector(posx, posy, posz), color, name);
			if (!stream) return nullptr;

			auto baseStream = static_cast<Stream*>(stream);

			SV::streamTable.emplace((uint32_t)(baseStream), baseStream);

			return baseStream;

		}

		Stream* CreateSLStreamAtVehicleHandler(
			const float distance,
			const uint16_t vehicleId,
			const uint32_t color,
			const std::string& name
		) {

			if (!pNetGame->pVehiclePool->pVehicle[vehicleId]) return nullptr;

			auto stream = new StaticLocalStreamAtVehicle(distance, vehicleId, color, name);
			if (!stream) return nullptr;

			auto baseStream = static_cast<Stream*>(stream);

			SV::streamTable.emplace((uint32_t)(baseStream), baseStream);

			return baseStream;

		}

		Stream* CreateSLStreamAtPlayerHandler(
			const float distance,
			const uint16_t playerId,
			const uint32_t color,
			const std::string& name
		) {

			if (!pNetGame->pPlayerPool->pPlayer[playerId]) return nullptr;

			auto stream = new StaticLocalStreamAtPlayer(distance, playerId, color, name);
			if (!stream) return nullptr;

			auto baseStream = static_cast<Stream*>(stream);

			SV::streamTable.emplace((uint32_t)(baseStream), baseStream);

			return baseStream;

		}

		Stream* CreateSLStreamAtObjectHandler(
			const float distance,
			const uint16_t objectId,
			const uint32_t color,
			const std::string& name
		) {

			if (!pNetGame->pObjectPool->pObjects[objectId]) return nullptr;

			auto stream = new StaticLocalStreamAtObject(distance, objectId, color, name);
			if (!stream) return nullptr;

			auto baseStream = static_cast<Stream*>(stream);

			SV::streamTable.emplace((uint32_t)(baseStream), baseStream);

			return baseStream;

		}



		Stream* CreateDLStreamAtPointHandler(
			const float distance,
			const uint32_t maxPlayers,
			const float posx,
			const float posy,
			const float posz,
			const uint32_t color,
			const std::string& name
		) {

			auto stream = new DynamicLocalStreamAtPoint(distance, maxPlayers, CVector(posx, posy, posz), color, name);
			if (!stream) return nullptr;

			auto baseStream = static_cast<Stream*>(stream);

			SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
			SV::streamTable.emplace((uint32_t)(baseStream), baseStream);

			return baseStream;

		}

		Stream* CreateDLStreamAtVehicleHandler(
			const float distance,
			const uint32_t maxPlayers,
			const uint16_t vehicleId,
			const uint32_t color,
			const std::string& name
		) {

			if (!pNetGame->pVehiclePool->pVehicle[vehicleId]) return nullptr;

			auto stream = new DynamicLocalStreamAtVehicle(distance, maxPlayers, vehicleId, color, name);
			if (!stream) return nullptr;

			auto baseStream = static_cast<Stream*>(stream);

			SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
			SV::streamTable.emplace((uint32_t)(baseStream), baseStream);

			return baseStream;

		}

		Stream* CreateDLStreamAtPlayerHandler(
			const float distance,
			const uint32_t maxPlayers,
			const uint16_t playerId,
			const uint32_t color,
			const std::string& name
		) {

			if (!pNetGame->pPlayerPool->pPlayer[playerId]) return nullptr;

			auto stream = new DynamicLocalStreamAtPlayer(distance, maxPlayers, playerId, color, name);
			if (!stream) return nullptr;

			auto baseStream = static_cast<Stream*>(stream);

			SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
			SV::streamTable.emplace((uint32_t)(baseStream), baseStream);

			return baseStream;

		}

		Stream* CreateDLStreamAtObjectHandler(
			const float distance,
			const uint32_t maxPlayers,
			const uint16_t objectId,
			const uint32_t color,
			const std::string& name
		) {

			if (!pNetGame->pObjectPool->pObjects[objectId]) return nullptr;

			auto stream = new DynamicLocalStreamAtObject(distance, maxPlayers, objectId, color, name);
			if (!stream) return nullptr;

			auto baseStream = static_cast<Stream*>(stream);

			SV::dlstreamList.insert(static_cast<DynamicStream*>(stream));
			SV::streamTable.emplace((uint32_t)(baseStream), baseStream);

			return baseStream;

		}



		void UpdatePositionForLPStreamHandler(PointStream* const lpStream, const float posx, const float posy, const float posz) {

			lpStream->UpdatePosition(CVector(posx, posy, posz));

		}

		void UpdateDistanceForLStreamHandler(LocalStream* const lStream, const float distance) {

			lStream->UpdateDistance(distance);

		}



		bool AttachListenerToStreamHandler(Stream* const stream, const uint16_t playerId) {

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) pPlayerInfo->listenerStreams.insert(stream);
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			return stream->AttachListener(playerId);

		}

		bool HasListenerInStreamHandler(Stream* const stream, const uint16_t playerId) {

			return stream->HasListener(playerId);

		}

		bool DetachListenerFromStreamHandler(Stream* const stream, const uint16_t playerId) {

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
			if (pPlayerInfo) pPlayerInfo->listenerStreams.erase(stream);
			PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			return stream->DetachListener(playerId);

		}

		void DetachAllListenersFromStreamHandler(Stream* const stream) {

			std::vector<uint16_t> detachedListeners;
			stream->DetachAllListeners(detachedListeners);

			for (const auto playerId : detachedListeners) {

				const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
				if (pPlayerInfo) pPlayerInfo->listenerStreams.erase(stream);
				PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			}

		}



		bool AttachSpeakerToStreamHandler(Stream* const stream, const uint16_t playerId) {

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
			if (pPlayerInfo) pPlayerInfo->speakerStreams.insert(stream);
			PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

			return stream->AttachSpeaker(playerId);

		}

		bool HasSpeakerInStreamHandler(Stream* const stream, const uint16_t playerId) {

			return stream->HasSpeaker(playerId);

		}

		bool DetachSpeakerFromStreamHandler(Stream* const stream, const uint16_t playerId) {

			const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
			if (pPlayerInfo) pPlayerInfo->speakerStreams.erase(stream);
			PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

			return stream->DetachSpeaker(playerId);

		}

		void DetachAllSpeakersFromStreamHandler(Stream* const stream) {

			std::vector<uint16_t> detachedSpeakers;
			stream->DetachAllSpeakers(detachedSpeakers);

			for (const auto playerId : detachedSpeakers) {

				const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
				if (pPlayerInfo) pPlayerInfo->speakerStreams.erase(stream);
				PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

			}

		}



		void DeleteStreamHandler(Stream* const stream) {

			std::vector<uint16_t> detachedSpeakers;
			stream->DetachAllSpeakers(detachedSpeakers);

			for (const auto playerId : detachedSpeakers) {

				const auto pPlayerInfo = PlayerStore::RequestPlayerWithUniqueAccess(playerId);
				if (pPlayerInfo) pPlayerInfo->speakerStreams.erase(stream);
				PlayerStore::ReleasePlayerWithUniqueAccess(playerId);

			}

			std::vector<uint16_t> detachedListeners;
			stream->DetachAllListeners(detachedListeners);

			for (const auto playerId : detachedListeners) {

				const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
				if (pPlayerInfo) pPlayerInfo->listenerStreams.erase(stream);
				PlayerStore::ReleasePlayerWithSharedAccess(playerId);

			}

			SV::streamTable.erase((uint32_t)(stream));
			if (const auto dlStream = dynamic_cast<DynamicStream*>(stream))
				SV::dlstreamList.erase(dlStream);

			delete stream;

		}

	}

	void ConnectHandler(const uint16_t playerId, const SV::ConnectPacket& connectStruct) {

		PlayerStore::AddPlayerToStore(playerId, connectStruct.version, connectStruct.micro);

	}

	void PlayerInitHandler(const uint16_t playerId, SV::PluginInitPacket& initStruct) {

		initStruct.mute = false;
		initStruct.bitrate = SV::bitrate;

		const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(playerId);
		if (pPlayerInfo) initStruct.mute = pPlayerInfo->muteStatus.load();
		PlayerStore::ReleasePlayerWithSharedAccess(playerId);

	}

	void DisconnectHandler(const uint16_t playerId) {

		PlayerStore::RemovePlayerFromStore(playerId);

	}

	static __forceinline void Tick(const int64_t curTime) {
		
		for (const auto dlStream : SV::dlstreamList)
			dlStream->Tick();

		uint16_t senderId = SV::NonePlayer;

		while (const auto controlPacket = Network::ReceiveControlPacket(senderId)) {

			const ControlPacketContainer& controlPacketRef = *controlPacket;

			switch (controlPacketRef->packet) {
			case SV::ControlPacketType::pressKey: {

				const auto stData = PackGetStruct(&controlPacketRef, SV::PressKeyPacket);
				if (controlPacketRef->length != sizeof(*stData)) break;

				const auto keyId = stData->keyId;
				bool pressKeyAllowStatus = false;

				const auto pPlayerInfo = PlayerStore::RequestPlayerWithSharedAccess(senderId);
				if (pPlayerInfo) pressKeyAllowStatus = pPlayerInfo->keys.find(keyId) != pPlayerInfo->keys.end();
				PlayerStore::ReleasePlayerWithSharedAccess(senderId);

				if (!pressKeyAllowStatus) break;

				Pawn::OnPlayerActivationKeyPressForAll(senderId, keyId);

			} break;
			case SV::ControlPacketType::releaseKey: {

				const auto stData = PackGetStruct(&controlPacketRef, SV::ReleaseKeyPacket);
				if (controlPacketRef->length != sizeof(*stData)) break;

				const auto keyId = stData->keyId;
				bool releaseKeyAllowStatus = false;

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

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick() {
	
	SV::Tick(GetTimestamp());

}

PLUGIN_EXPORT void PLUGIN_CALL Unload() {

	static std::atomic_bool unloadStatus(false);
	if (unloadStatus.exchange(true)) return;

	Logger::Log(" -------------------------------------------");
	Logger::Log("           SampVoice unloading...           ");
	Logger::Log(" -------------------------------------------");

	SV::workers.clear();

	Pawn::Free();
	RakNet::Free();
	Network::Free();
	Logger::Free();

}

#ifdef _WIN32
BOOL WINAPI WinExitHandler(DWORD CtrlType) { Unload(); return FALSE; }
#endif

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData) {

#ifdef _WIN32
	SetConsoleCtrlHandler(&WinExitHandler, TRUE);
#endif

	ppPluginData = ppData;
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)(ppData[PLUGIN_DATA_LOGPRINTF]);

	if (!Logger::Init(SV::LogFileName, logprintf)) {

		logprintf("[sv:err:main:Load] : failed to init logger");
		return false;

	}

	if (!Network::Init(logprintf, SV::ConnectHandler, SV::PlayerInitHandler, SV::DisconnectHandler)) {

		Logger::Log("[sv:err:main:Load] : failed to init network");
		Logger::Free();
		return false;

	}

	if (!Pawn::Init(
		SV::PawnHandlers::InitHandler,
		SV::PawnHandlers::GetVersionHandler,
		SV::PawnHandlers::HasMicroHandler,
		SV::PawnHandlers::StartRecordHandler,
		SV::PawnHandlers::StopRecordHandler,
		SV::PawnHandlers::AddKeyHandler,
		SV::PawnHandlers::HasKeyHandler,
		SV::PawnHandlers::RemoveKeyHandler,
		SV::PawnHandlers::RemoveAllKeysHandler,
		SV::PawnHandlers::MutePlayerStatusHandler,
		SV::PawnHandlers::MutePlayerEnableHandler,
		SV::PawnHandlers::MutePlayerDisableHandler,
		SV::PawnHandlers::CreateGStreamHandler,
		SV::PawnHandlers::CreateSLStreamAtPointHandler,
		SV::PawnHandlers::CreateSLStreamAtVehicleHandler,
		SV::PawnHandlers::CreateSLStreamAtPlayerHandler,
		SV::PawnHandlers::CreateSLStreamAtObjectHandler,
		SV::PawnHandlers::CreateDLStreamAtPointHandler,
		SV::PawnHandlers::CreateDLStreamAtVehicleHandler,
		SV::PawnHandlers::CreateDLStreamAtPlayerHandler,
		SV::PawnHandlers::CreateDLStreamAtObjectHandler,
		SV::PawnHandlers::UpdatePositionForLPStreamHandler,
		SV::PawnHandlers::UpdateDistanceForLStreamHandler,
		SV::PawnHandlers::AttachListenerToStreamHandler,
		SV::PawnHandlers::HasListenerInStreamHandler,
		SV::PawnHandlers::DetachListenerFromStreamHandler,
		SV::PawnHandlers::DetachAllListenersFromStreamHandler,
		SV::PawnHandlers::AttachSpeakerToStreamHandler,
		SV::PawnHandlers::HasSpeakerInStreamHandler,
		SV::PawnHandlers::DetachSpeakerFromStreamHandler,
		SV::PawnHandlers::DetachAllSpeakersFromStreamHandler,
		SV::PawnHandlers::DeleteStreamHandler
	)) {

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
			SV::workers.emplace_back(Worker::Create());
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

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx) {

	if (!pNetGame && (pNetGame = ((CNetGame*(*)())(ppPluginData[PLUGIN_DATA_NETGAME]))()))
		Logger::Log("[sv:dbg:main:AmxLoad] : net game pointer (value:%p) received", pNetGame);

	if (!Network::Bind()) Logger::Log("[sv:dbg:main:AmxLoad] : failed to bind voice server");

	Pawn::RegisterScript(amx);

	return AMX_ERR_NONE;

}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx) {

	return AMX_ERR_NONE;

}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {

	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;

}
