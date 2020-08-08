/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <string>

#include <pawn/amx/amx.h>
#include <pawn/plugincommon.h>

#include "Stream.h"
#include "LocalStream.h"
#include "PointStream.h"

class Pawn {
public:

	using InitHandlerType							= std::function<void(const uint32_t)>;
	using GetVersionHandlerType						= std::function<uint8_t(const uint16_t)>;

	using HasMicroHandlerType						= std::function<bool(const uint16_t)>;
	using StartRecordHandlerType					= std::function<bool(const uint16_t)>;
	using StopRecordHandlerType						= std::function<bool(const uint16_t)>;

	using AddKeyHandlerType							= std::function<bool(const uint16_t, const uint8_t)>;
	using HasKeyHandlerType							= std::function<bool(const uint16_t, const uint8_t)>;
	using RemoveKeyHandlerType						= std::function<bool(const uint16_t, const uint8_t)>;
	using RemoveAllKeysHandlerType					= std::function<void(const uint16_t)>;

	using MutePlayerStatusHandlerType				= std::function<bool(const uint16_t)>;
	using MutePlayerEnableHandlerType				= std::function<void(const uint16_t)>;
	using MutePlayerDisableHandlerType				= std::function<void(const uint16_t)>;

	using CreateGStreamHandlerType					= std::function<Stream* (const uint32_t, const std::string&)>;
	using CreateSLStreamAtPointHandlerType			= std::function<Stream* (const float, const float, const float, const float, const uint32_t, const std::string&)>;
	using CreateSLStreamAtVehicleHandlerType		= std::function<Stream* (const float, const uint16_t, const uint32_t, const std::string&)>;
	using CreateSLStreamAtPlayerHandlerType			= std::function<Stream* (const float, const uint16_t, const uint32_t, const std::string&)>;
	using CreateSLStreamAtObjectHandlerType			= std::function<Stream* (const float, const uint16_t, const uint32_t, const std::string&)>;
	using CreateDLStreamAtPointHandlerType			= std::function<Stream* (const float, const uint32_t, const float, const float, const float, const uint32_t, const std::string&)>;
	using CreateDLStreamAtVehicleHandlerType		= std::function<Stream* (const float, const uint32_t, const uint16_t, const uint32_t, const std::string&)>;
	using CreateDLStreamAtPlayerHandlerType			= std::function<Stream* (const float, const uint32_t, const uint16_t, const uint32_t, const std::string&)>;
	using CreateDLStreamAtObjectHandlerType			= std::function<Stream* (const float, const uint32_t, const uint16_t, const uint32_t, const std::string&)>;

	using UpdatePositionForLPStreamHandlerType		= std::function<void(PointStream* const, const float, const float, const float)>;
	using UpdateDistanceForLStreamHandlerType		= std::function<void(LocalStream* const, const float)>;

	using AttachListenerToStreamHandlerType			= std::function<bool(Stream* const, const uint16_t)>;
	using HasListenerInStreamHandlerType			= std::function<bool(Stream* const, const uint16_t)>;
	using DetachListenerFromStreamHandlerType		= std::function<bool(Stream* const, const uint16_t)>;
	using DetachAllListenersFromStreamHandlerType	= std::function<void(Stream* const)>;

	using AttachSpeakerToStreamHandlerType			= std::function<bool(Stream* const, const uint16_t)>;
	using HasSpeakerInStreamHandlerType				= std::function<bool(Stream* const, const uint16_t)>;
	using DetachSpeakerFromStreamHandlerType		= std::function<bool(Stream* const, const uint16_t)>;
	using DetachAllSpeakersFromStreamHandlerType	= std::function<void(Stream* const)>;

	using DeleteStreamHandlerType					= std::function<void(Stream* const)>;

private:

	static InitHandlerType							initHandler;
	static GetVersionHandlerType					getVersionHandler;

	static HasMicroHandlerType						hasMicroHandler;
	static StartRecordHandlerType					startRecordHandler;
	static StopRecordHandlerType					stopRecordHandler;

	static AddKeyHandlerType						addKeyHandler;
	static HasKeyHandlerType						hasKeyHandler;
	static RemoveKeyHandlerType						removeKeyHandler;
	static RemoveAllKeysHandlerType					removeAllKeysHandler;

	static MutePlayerStatusHandlerType				mutePlayerStatusHandler;
	static MutePlayerEnableHandlerType				mutePlayerEnableHandler;
	static MutePlayerDisableHandlerType				mutePlayerDisableHandler;

	static CreateGStreamHandlerType					createGStreamHandler;
	static CreateSLStreamAtPointHandlerType			createSLStreamAtPointHandler;
	static CreateSLStreamAtVehicleHandlerType		createSLStreamAtVehicleHandler;
	static CreateSLStreamAtPlayerHandlerType		createSLStreamAtPlayerHandler;
	static CreateSLStreamAtObjectHandlerType		createSLStreamAtObjectHandler;
	static CreateDLStreamAtPointHandlerType			createDLStreamAtPointHandler;
	static CreateDLStreamAtVehicleHandlerType		createDLStreamAtVehicleHandler;
	static CreateDLStreamAtPlayerHandlerType		createDLStreamAtPlayerHandler;
	static CreateDLStreamAtObjectHandlerType		createDLStreamAtObjectHandler;

	static UpdatePositionForLPStreamHandlerType		updatePositionForLPStreamHandler;
	static UpdateDistanceForLStreamHandlerType		updateDistanceForLStreamHandler;

	static AttachListenerToStreamHandlerType		attachListenerToStreamHandler;
	static HasListenerInStreamHandlerType			hasListenerInStreamHandler;
	static DetachListenerFromStreamHandlerType		detachListenerFromStreamHandler;
	static DetachAllListenersFromStreamHandlerType	detachAllListenersFromStreamHandler;

	static AttachSpeakerToStreamHandlerType			attachSpeakerToStreamHandler;
	static HasSpeakerInStreamHandlerType			hasSpeakerInStreamHandler;
	static DetachSpeakerFromStreamHandlerType		detachSpeakerFromStreamHandler;
	static DetachAllSpeakersFromStreamHandlerType	detachAllSpeakersFromStreamHandler;

	static DeleteStreamHandlerType					deleteStreamHandler;

	class AmxCallback {
	private:

		AMX* const amx;
		const int index;

	public:

		AmxCallback(AMX* const amx, const int index)
			: amx(amx), index(index) {}

		template<class... ARGS>
		inline cell Call(ARGS... args) const {

			cell returnValue = NULL;

			(amx_Push(this->amx, static_cast<cell>(args)), ...); // reverse order of arguments
			amx_Exec(this->amx, &returnValue, this->index);

			return returnValue;

		}

	};

	static bool initStatus;
	static bool debugStatus;

	static std::vector<AmxCallback> callbacksOnPlayerActivationKeyPress;
	static std::vector<AmxCallback> callbacksOnPlayerActivationKeyRelease;

	static cell AMX_NATIVE_CALL n_SvDebug(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL n_SvInit(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvGetVersion(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL n_SvHasMicro(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvStartRecord(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvStopRecord(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL n_SvAddKey(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvHasKey(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvRemoveKey(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvRemoveAllKeys(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL n_SvMutePlayerStatus(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvMutePlayerEnable(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvMutePlayerDisable(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL n_SvCreateGStream(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvCreateSLStreamAtPoint(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvCreateSLStreamAtVehicle(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvCreateSLStreamAtPlayer(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvCreateSLStreamAtObject(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvCreateDLStreamAtPoint(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvCreateDLStreamAtVehicle(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvCreateDLStreamAtPlayer(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvCreateDLStreamAtObject(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL n_SvUpdateDistanceForLStream(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvUpdatePositionForLPStream(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL n_SvAttachListenerToStream(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvHasListenerInStream(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvDetachListenerFromStream(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvDetachAllListenersFromStream(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL n_SvAttachSpeakerToStream(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvHasSpeakerInStream(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvDetachSpeakerFromStream(AMX* amx, cell* params);
	static cell AMX_NATIVE_CALL n_SvDetachAllSpeakersFromStream(AMX* amx, cell* params);

	static cell AMX_NATIVE_CALL n_SvDeleteStream(AMX* amx, cell* params);

public:

	static bool Init(
		const InitHandlerType& initHandler,
		const GetVersionHandlerType& getVersionHandler,
		const HasMicroHandlerType& hasMicroHandler,
		const StartRecordHandlerType& startRecordHandler,
		const StopRecordHandlerType& stopRecordHandler,
		const AddKeyHandlerType& addKeyHandler,
		const HasKeyHandlerType& hasKeyHandler,
		const RemoveKeyHandlerType& removeKeyHandler,
		const RemoveAllKeysHandlerType& removeAllKeysHandler,
		const MutePlayerStatusHandlerType& mutePlayerStatusHandler,
		const MutePlayerEnableHandlerType& mutePlayerEnableHandler,
		const MutePlayerDisableHandlerType& mutePlayerDisableHandler,
		const CreateGStreamHandlerType& createGStreamHandler,
		const CreateSLStreamAtPointHandlerType& createSLStreamAtPointHandler,
		const CreateSLStreamAtVehicleHandlerType& createSLStreamAtVehicleHandler,
		const CreateSLStreamAtPlayerHandlerType& createSLStreamAtPlayerHandler,
		const CreateSLStreamAtObjectHandlerType& createSLStreamAtObjectHandler,
		const CreateDLStreamAtPointHandlerType& createDLStreamAtPointHandler,
		const CreateDLStreamAtVehicleHandlerType& createDLStreamAtVehicleHandler,
		const CreateDLStreamAtPlayerHandlerType& createDLStreamAtPlayerHandler,
		const CreateDLStreamAtObjectHandlerType& createDLStreamAtObjectHandler,
		const UpdatePositionForLPStreamHandlerType& updatePositionForLPStreamHandler,
		const UpdateDistanceForLStreamHandlerType& updateDistanceForLStreamHandler,
		const AttachListenerToStreamHandlerType& attachListenerToStreamHandler,
		const HasListenerInStreamHandlerType& hasListenerInStreamHandler,
		const DetachListenerFromStreamHandlerType& detachListenerFromStreamHandler,
		const DetachAllListenersFromStreamHandlerType& detachAllListenersFromStreamHandler,
		const AttachSpeakerToStreamHandlerType& attachSpeakerToStreamHandler,
		const HasSpeakerInStreamHandlerType& hasSpeakerInStreamHandler,
		const DetachSpeakerFromStreamHandlerType& detachSpeakerFromStreamHandler,
		const DetachAllSpeakersFromStreamHandlerType& detachAllSpeakersFromStreamHandler,
		const DeleteStreamHandlerType& deleteStreamHandler
	);

	static void OnPlayerActivationKeyPressForAll(const uint16_t playerid, const uint8_t keyid);
	static void OnPlayerActivationKeyReleaseForAll(const uint16_t playerid, const uint8_t keyid);

	static void RegisterScript(AMX* const amx);

	static void Free();

};
