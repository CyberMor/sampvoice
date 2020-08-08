/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Pawn.h"

#include <util/logger.h>

Pawn::InitHandlerType							Pawn::initHandler(nullptr);
Pawn::GetVersionHandlerType						Pawn::getVersionHandler(nullptr);
Pawn::HasMicroHandlerType						Pawn::hasMicroHandler(nullptr);
Pawn::StartRecordHandlerType					Pawn::startRecordHandler(nullptr);
Pawn::StopRecordHandlerType						Pawn::stopRecordHandler(nullptr);
Pawn::AddKeyHandlerType							Pawn::addKeyHandler(nullptr);
Pawn::HasKeyHandlerType							Pawn::hasKeyHandler(nullptr);
Pawn::RemoveKeyHandlerType						Pawn::removeKeyHandler(nullptr);
Pawn::RemoveAllKeysHandlerType					Pawn::removeAllKeysHandler(nullptr);
Pawn::MutePlayerStatusHandlerType				Pawn::mutePlayerStatusHandler(nullptr);
Pawn::MutePlayerEnableHandlerType				Pawn::mutePlayerEnableHandler(nullptr);
Pawn::MutePlayerDisableHandlerType				Pawn::mutePlayerDisableHandler(nullptr);
Pawn::CreateGStreamHandlerType					Pawn::createGStreamHandler(nullptr);
Pawn::CreateSLStreamAtPointHandlerType			Pawn::createSLStreamAtPointHandler(nullptr);
Pawn::CreateSLStreamAtVehicleHandlerType		Pawn::createSLStreamAtVehicleHandler(nullptr);
Pawn::CreateSLStreamAtPlayerHandlerType			Pawn::createSLStreamAtPlayerHandler(nullptr);
Pawn::CreateSLStreamAtObjectHandlerType			Pawn::createSLStreamAtObjectHandler(nullptr);
Pawn::CreateDLStreamAtPointHandlerType			Pawn::createDLStreamAtPointHandler(nullptr);
Pawn::CreateDLStreamAtVehicleHandlerType		Pawn::createDLStreamAtVehicleHandler(nullptr);
Pawn::CreateDLStreamAtPlayerHandlerType			Pawn::createDLStreamAtPlayerHandler(nullptr);
Pawn::CreateDLStreamAtObjectHandlerType			Pawn::createDLStreamAtObjectHandler(nullptr);
Pawn::UpdatePositionForLPStreamHandlerType		Pawn::updatePositionForLPStreamHandler(nullptr);
Pawn::UpdateDistanceForLStreamHandlerType		Pawn::updateDistanceForLStreamHandler(nullptr);
Pawn::AttachListenerToStreamHandlerType			Pawn::attachListenerToStreamHandler(nullptr);
Pawn::HasListenerInStreamHandlerType			Pawn::hasListenerInStreamHandler(nullptr);
Pawn::DetachListenerFromStreamHandlerType		Pawn::detachListenerFromStreamHandler(nullptr);
Pawn::DetachAllListenersFromStreamHandlerType	Pawn::detachAllListenersFromStreamHandler(nullptr);
Pawn::AttachSpeakerToStreamHandlerType			Pawn::attachSpeakerToStreamHandler(nullptr);
Pawn::HasSpeakerInStreamHandlerType				Pawn::hasSpeakerInStreamHandler(nullptr);
Pawn::DetachSpeakerFromStreamHandlerType		Pawn::detachSpeakerFromStreamHandler(nullptr);
Pawn::DetachAllSpeakersFromStreamHandlerType	Pawn::detachAllSpeakersFromStreamHandler(nullptr);
Pawn::DeleteStreamHandlerType					Pawn::deleteStreamHandler(nullptr);

bool Pawn::initStatus(false);
bool Pawn::debugStatus(false);

std::vector<Pawn::AmxCallback> Pawn::callbacksOnPlayerActivationKeyPress;
std::vector<Pawn::AmxCallback> Pawn::callbacksOnPlayerActivationKeyRelease;

cell AMX_NATIVE_CALL Pawn::n_SvDebug(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (1 * sizeof(cell))) return NULL;

	Pawn::debugStatus = static_cast<bool>(params[1]);

	Logger::Log("[sv:dbg:pawn:Debug] : mode(%hhu)", Pawn::debugStatus);

	return NULL;

}

cell AMX_NATIVE_CALL Pawn::n_SvInit(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (1 * sizeof(cell))) return NULL;
	if (!Pawn::initHandler) return NULL;

	const auto bitrate = static_cast<uint32_t>(params[1]);

	if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:Init] : bitrate(%u)", bitrate);

	Pawn::initHandler(bitrate);

	return NULL;

}

cell AMX_NATIVE_CALL Pawn::n_SvGetVersion(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (1 * sizeof(cell))) return NULL;
	if (!Pawn::getVersionHandler) return NULL;

	const auto playerid = static_cast<uint16_t>(params[1]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::getVersionHandler(playerid);
		Logger::Log("[sv:dbg:pawn:GetVersion] : playerid(%hu) : return(%hhu)", playerid, result);
		return result;

	}

	return Pawn::getVersionHandler(playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvHasMicro(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (1 * sizeof(cell))) return false;
	if (!Pawn::hasMicroHandler) return false;

	const auto playerid = static_cast<uint16_t>(params[1]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::hasMicroHandler(playerid);
		Logger::Log("[sv:dbg:pawn:HasMicro] : playerid(%hu) : return(%hhu)", playerid, result);
		return result;

	}

	return Pawn::hasMicroHandler(playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvStartRecord(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (1 * sizeof(cell))) return false;
	if (!Pawn::startRecordHandler) return false;

	const auto playerid = static_cast<uint16_t>(params[1]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::startRecordHandler(playerid);
		Logger::Log("[sv:dbg:pawn:StartRecord] : playerid(%hu) : return(%hhu)", playerid, result);
		return result;

	}

	return Pawn::startRecordHandler(playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvStopRecord(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (1 * sizeof(cell))) return false;
	if (!Pawn::stopRecordHandler) return false;

	const auto playerid = static_cast<uint16_t>(params[1]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::stopRecordHandler(playerid);
		Logger::Log("[sv:dbg:pawn:StopRecord] : playerid(%hu) : return(%hhu)", playerid, result);
		return result;

	}

	return Pawn::stopRecordHandler(playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvAddKey(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (2 * sizeof(cell))) return false;
	if (!Pawn::addKeyHandler) return false;

	const auto playerid = static_cast<uint16_t>(params[1]);
	const auto keyid = static_cast<uint8_t>(params[2]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::addKeyHandler(playerid, keyid);
		Logger::Log("[sv:dbg:pawn:AddKey] : playerid(%hu), keyid(0x%hhx) : return(%hhu)", playerid, keyid, result);
		return result;

	}

	return Pawn::addKeyHandler(playerid, keyid);

}

cell AMX_NATIVE_CALL Pawn::n_SvHasKey(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (2 * sizeof(cell))) return false;
	if (!Pawn::hasKeyHandler) return false;

	const auto playerid = static_cast<uint16_t>(params[1]);
	const auto keyid = static_cast<uint8_t>(params[2]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::hasKeyHandler(playerid, keyid);
		Logger::Log("[sv:dbg:pawn:HasKey] : playerid(%hu), keyid(0x%hhx) : return(%hhu)", playerid, keyid, result);
		return result;

	}

	return Pawn::hasKeyHandler(playerid, keyid);

}

cell AMX_NATIVE_CALL Pawn::n_SvRemoveKey(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (2 * sizeof(cell))) return false;
	if (!Pawn::removeKeyHandler) return false;

	const auto playerid = static_cast<uint16_t>(params[1]);
	const auto keyid = static_cast<uint8_t>(params[2]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::removeKeyHandler(playerid, keyid);
		Logger::Log("[sv:dbg:pawn:RemoveKey] : playerid(%hu), keyid(0x%hhx) : return(%hhu)", playerid, keyid, result);
		return result;

	}

	return Pawn::removeKeyHandler(playerid, keyid);

}

cell AMX_NATIVE_CALL Pawn::n_SvRemoveAllKeys(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (1 * sizeof(cell))) return NULL;
	if (!Pawn::removeAllKeysHandler) return NULL;

	const auto playerid = static_cast<uint16_t>(params[1]);

	if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:RemoveAllKeys] : playerid(%hu)", playerid);

	Pawn::removeAllKeysHandler(playerid);

	return NULL;

}

cell AMX_NATIVE_CALL Pawn::n_SvMutePlayerStatus(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (1 * sizeof(cell))) return false;
	if (!Pawn::mutePlayerStatusHandler) return false;

	const auto playerid = static_cast<uint16_t>(params[1]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::mutePlayerStatusHandler(playerid);
		Logger::Log("[sv:dbg:pawn:MutePlayerStatus] : playerid(%hu) : return(%hhu)", playerid, result);
		return result;

	}

	return Pawn::mutePlayerStatusHandler(playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvMutePlayerEnable(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (1 * sizeof(cell))) return NULL;
	if (!Pawn::mutePlayerEnableHandler) return NULL;

	const auto playerid = static_cast<uint16_t>(params[1]);

	if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:MutePlayerEnable] : playerid(%hu)", playerid);

	Pawn::mutePlayerEnableHandler(playerid);

	return NULL;

}

cell AMX_NATIVE_CALL Pawn::n_SvMutePlayerDisable(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (1 * sizeof(cell))) return NULL;
	if (!Pawn::mutePlayerDisableHandler) return NULL;

	const auto playerid = static_cast<uint16_t>(params[1]);

	if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:MutePlayerDisable] : playerid(%hu)", playerid);

	Pawn::mutePlayerDisableHandler(playerid);

	return NULL;

}

cell AMX_NATIVE_CALL Pawn::n_SvCreateGStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (2 * sizeof(cell))) return NULL;
	if (!Pawn::createGStreamHandler) return NULL;

	const auto color = static_cast<uint32_t>(params[1]);

	cell* phys_addr = nullptr; int tmp_len = 0;
	if (amx_GetAddr(amx, params[2], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
	std::string name(tmp_len + 1, '\0');
	if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

	if (Pawn::debugStatus) {

		const auto result = Pawn::createGStreamHandler(color, name);
		Logger::Log("[sv:dbg:pawn:CreateGStream] : color(0x%x), name(%s) : return(%p)", color, name.c_str(), result);
		return reinterpret_cast<cell>(result);

	}

	return reinterpret_cast<cell>(Pawn::createGStreamHandler(color, name));

}

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtPoint(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (6 * sizeof(cell))) return NULL;
	if (!Pawn::createSLStreamAtPointHandler) return NULL;

	const auto distance = amx_ctof(params[1]);
	const auto posx = amx_ctof(params[2]);
	const auto posy = amx_ctof(params[3]);
	const auto posz = amx_ctof(params[4]);
	const auto color = static_cast<uint32_t>(params[5]);

	cell* phys_addr = nullptr; int tmp_len = 0;
	if (amx_GetAddr(amx, params[6], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
	std::string name(tmp_len + 1, '\0');
	if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

	if (Pawn::debugStatus) {

		const auto result = Pawn::createSLStreamAtPointHandler(distance, posx, posy, posz, color, name);
		Logger::Log("[sv:dbg:pawn:CreateSLStreamAtPoint] : distance(%.2f), pos(%.2f;%.2f;%.2f), color(0x%x), name(%s) : return(%p)",
			distance, posx, posy, posz, color, name.c_str(), result);
		return reinterpret_cast<cell>(result);

	}

	return reinterpret_cast<cell>(Pawn::createSLStreamAtPointHandler(distance, posx, posy, posz, color, name));

}

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtVehicle(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (4 * sizeof(cell))) return NULL;
	if (!Pawn::createSLStreamAtVehicleHandler) return NULL;

	const auto distance = amx_ctof(params[1]);
	const auto vehicleid = static_cast<uint16_t>(params[2]);
	const auto color = static_cast<uint32_t>(params[3]);

	cell* phys_addr = nullptr; int tmp_len = 0;
	if (amx_GetAddr(amx, params[4], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
	std::string name(tmp_len + 1, '\0');
	if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

	if (Pawn::debugStatus) {

		const auto result = Pawn::createSLStreamAtVehicleHandler(distance, vehicleid, color, name);
		Logger::Log("[sv:dbg:pawn:CreateSLStreamAtVehicle] : distance(%.2f), vehicleid(%hu), color(0x%x), name(%s) : return(%p)",
			distance, vehicleid, color, name.c_str(), result);
		return reinterpret_cast<cell>(result);

	}

	return reinterpret_cast<cell>(Pawn::createSLStreamAtVehicleHandler(distance, vehicleid, color, name));

}

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtPlayer(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (4 * sizeof(cell))) return NULL;
	if (!Pawn::createSLStreamAtPlayerHandler) return NULL;

	const auto distance = amx_ctof(params[1]);
	const auto playerid = static_cast<uint16_t>(params[2]);
	const auto color = static_cast<uint32_t>(params[3]);

	cell* phys_addr = nullptr; int tmp_len = 0;
	if (amx_GetAddr(amx, params[4], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
	std::string name(tmp_len + 1, '\0');
	if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

	if (Pawn::debugStatus) {

		const auto result = Pawn::createSLStreamAtPlayerHandler(distance, playerid, color, name);
		Logger::Log("[sv:dbg:pawn:CreateSLStreamAtPlayer] : distance(%.2f), playerid(%hu), color(0x%x), name(%s) : return(%p)",
			distance, playerid, color, name.c_str(), result);
		return reinterpret_cast<cell>(result);

	}

	return reinterpret_cast<cell>(Pawn::createSLStreamAtPlayerHandler(distance, playerid, color, name));

}

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtObject(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (4 * sizeof(cell))) return NULL;
	if (!Pawn::createSLStreamAtObjectHandler) return NULL;

	const auto distance = amx_ctof(params[1]);
	const auto objectid = static_cast<uint16_t>(params[2]);
	const auto color = static_cast<uint32_t>(params[3]);

	cell* phys_addr = nullptr; int tmp_len = 0;
	if (amx_GetAddr(amx, params[4], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
	std::string name(tmp_len + 1, '\0');
	if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

	if (Pawn::debugStatus) {

		const auto result = Pawn::createSLStreamAtObjectHandler(distance, objectid, color, name);
		Logger::Log("[sv:dbg:pawn:CreateSLStreamAtObject] : distance(%.2f), objectid(%hu), color(0x%x), name(%s) : return(%p)",
			distance, objectid, color, name.c_str(), result);
		return reinterpret_cast<cell>(result);

	}

	return reinterpret_cast<cell>(Pawn::createSLStreamAtObjectHandler(distance, objectid, color, name));

}

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtPoint(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (7 * sizeof(cell))) return NULL;
	if (!Pawn::createDLStreamAtPointHandler) return NULL;

	const auto distance = amx_ctof(params[1]);
	const auto maxplayers = static_cast<uint32_t>(params[2]);
	const auto posx = amx_ctof(params[3]);
	const auto posy = amx_ctof(params[4]);
	const auto posz = amx_ctof(params[5]);
	const auto color = static_cast<uint32_t>(params[6]);

	cell* phys_addr = nullptr; int tmp_len = 0;
	if (amx_GetAddr(amx, params[7], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
	std::string name(tmp_len + 1, '\0');
	if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

	if (Pawn::debugStatus) {

		const auto result = Pawn::createDLStreamAtPointHandler(distance, maxplayers, posx, posy, posz, color, name);
		Logger::Log("[sv:dbg:pawn:CreateDLStreamAtPoint] : distance(%.2f), maxplayers(%u), pos(%.2f;%.2f;%.2f), color(0x%x), name(%s) : return(%p)",
			distance, maxplayers, posx, posy, posz, color, name.c_str(), result);
		return reinterpret_cast<cell>(result);

	}

	return reinterpret_cast<cell>(Pawn::createDLStreamAtPointHandler(distance, maxplayers, posx, posy, posz, color, name));

}

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtVehicle(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (5 * sizeof(cell))) return NULL;
	if (!Pawn::createDLStreamAtVehicleHandler) return NULL;

	const auto distance = amx_ctof(params[1]);
	const auto maxplayers = static_cast<uint32_t>(params[2]);
	const auto vehicleid = static_cast<uint16_t>(params[3]);
	const auto color = static_cast<uint32_t>(params[4]);

	cell* phys_addr = nullptr; int tmp_len = 0;
	if (amx_GetAddr(amx, params[5], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
	std::string name(tmp_len + 1, '\0');
	if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

	if (Pawn::debugStatus) {

		const auto result = Pawn::createDLStreamAtVehicleHandler(distance, maxplayers, vehicleid, color, name);
		Logger::Log("[sv:dbg:pawn:CreateDLStreamAtVehicle] : distance(%.2f), maxplayers(%u), vehicleid(%hu), color(0x%x), name(%s) : return(%p)",
			distance, maxplayers, vehicleid, color, name.c_str(), result);
		return reinterpret_cast<cell>(result);

	}

	return reinterpret_cast<cell>(Pawn::createDLStreamAtVehicleHandler(distance, maxplayers, vehicleid, color, name));

}

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtPlayer(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (5 * sizeof(cell))) return NULL;
	if (!Pawn::createDLStreamAtPlayerHandler) return NULL;

	const auto distance = amx_ctof(params[1]);
	const auto maxplayers = static_cast<uint32_t>(params[2]);
	const auto playerid = static_cast<uint16_t>(params[3]);
	const auto color = static_cast<uint32_t>(params[4]);

	cell* phys_addr = nullptr; int tmp_len = 0;
	if (amx_GetAddr(amx, params[5], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
	std::string name(tmp_len + 1, '\0');
	if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

	if (Pawn::debugStatus) {

		const auto result = Pawn::createDLStreamAtPlayerHandler(distance, maxplayers, playerid, color, name);
		Logger::Log("[sv:dbg:pawn:CreateDLStreamAtPlayer] : distance(%.2f), maxplayers(%u), playerid(%hu), color(0x%x), name(%s) : return(%p)",
			distance, maxplayers, playerid, color, name.c_str(), result);
		return reinterpret_cast<cell>(result);

	}

	return reinterpret_cast<cell>(Pawn::createDLStreamAtPlayerHandler(distance, maxplayers, playerid, color, name));

}

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtObject(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (5 * sizeof(cell))) return NULL;
	if (!Pawn::createDLStreamAtObjectHandler) return NULL;

	const auto distance = amx_ctof(params[1]);
	const auto maxplayers = static_cast<uint32_t>(params[2]);
	const auto objectid = static_cast<uint16_t>(params[3]);
	const auto color = static_cast<uint32_t>(params[4]);

	cell* phys_addr = nullptr; int tmp_len = 0;
	if (amx_GetAddr(amx, params[5], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
	std::string name(tmp_len + 1, '\0');
	if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

	if (Pawn::debugStatus) {

		const auto result = Pawn::createDLStreamAtObjectHandler(distance, maxplayers, objectid, color, name);
		Logger::Log("[sv:dbg:pawn:CreateDLStreamAtObject] : distance(%.2f), maxplayers(%u), objectid(%hu), color(0x%x), name(%s) : return(%p)",
			distance, maxplayers, objectid, color, name.c_str(), result);
		return reinterpret_cast<cell>(result);

	}

	return reinterpret_cast<cell>(Pawn::createDLStreamAtObjectHandler(distance, maxplayers, objectid, color, name));

}

cell AMX_NATIVE_CALL Pawn::n_SvUpdateDistanceForLStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (2 * sizeof(cell))) return NULL;
	if (!Pawn::updateDistanceForLStreamHandler) return NULL;

	const auto lstream = dynamic_cast<LocalStream*>(reinterpret_cast<Stream*>(params[1]));

	if (!lstream) return NULL;

	const auto distance = amx_ctof(params[2]);

	if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:UpdateDistanceForLStream] : lstream(%p), distance(%.2f)", lstream, distance);

	Pawn::updateDistanceForLStreamHandler(lstream, distance);

	return NULL;

}

cell AMX_NATIVE_CALL Pawn::n_SvUpdatePositionForLPStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (4 * sizeof(cell))) return NULL;
	if (!Pawn::updatePositionForLPStreamHandler) return NULL;

	const auto lpstream = dynamic_cast<PointStream*>(reinterpret_cast<Stream*>(params[1]));

	if (!lpstream) return NULL;

	const auto posx = amx_ctof(params[2]);
	const auto posy = amx_ctof(params[3]);
	const auto posz = amx_ctof(params[4]);

	if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:UpdatePositionForLPStream] : lpstream(%p), pos(%.2f;%.2f;%.2f)", lpstream, posx, posy, posz);

	Pawn::updatePositionForLPStreamHandler(lpstream, posx, posy, posz);

	return NULL;

}

cell AMX_NATIVE_CALL Pawn::n_SvAttachListenerToStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (2 * sizeof(cell))) return false;
	if (!Pawn::attachListenerToStreamHandler) return false;

	const auto stream = reinterpret_cast<Stream*>(params[1]);
	const auto playerid = static_cast<uint16_t>(params[2]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::attachListenerToStreamHandler(stream, playerid);
		Logger::Log("[sv:dbg:pawn:AttachListenerToStream] : stream(%p), playerid(%hu) : return(%hhu)", stream, playerid, result);
		return result;

	}

	return Pawn::attachListenerToStreamHandler(stream, playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvHasListenerInStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (2 * sizeof(cell))) return false;
	if (!Pawn::hasListenerInStreamHandler) return false;

	const auto stream = reinterpret_cast<Stream*>(params[1]);
	const auto playerid = static_cast<uint16_t>(params[2]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::hasListenerInStreamHandler(stream, playerid);
		Logger::Log("[sv:dbg:pawn:HasListenerInStream] : stream(%p), playerid(%hu) : return(%hhu)", stream, playerid, result);
		return result;

	}

	return Pawn::hasListenerInStreamHandler(stream, playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvDetachListenerFromStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (2 * sizeof(cell))) return false;
	if (!Pawn::detachListenerFromStreamHandler) return false;

	const auto stream = reinterpret_cast<Stream*>(params[1]);
	const auto playerid = static_cast<uint16_t>(params[2]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::detachListenerFromStreamHandler(stream, playerid);
		Logger::Log("[sv:dbg:pawn:DetachListenerFromStream] : stream(%p), playerid(%hu) : return(%hhu)", stream, playerid, result);
		return result;

	}

	return Pawn::detachListenerFromStreamHandler(stream, playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvDetachAllListenersFromStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (1 * sizeof(cell))) return NULL;
	if (!Pawn::detachAllListenersFromStreamHandler) return NULL;

	const auto stream = reinterpret_cast<Stream*>(params[1]);

	if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:DetachAllListenersFromStream] : stream(%p)", stream);

	Pawn::detachAllListenersFromStreamHandler(stream);

	return NULL;

}

cell AMX_NATIVE_CALL Pawn::n_SvAttachSpeakerToStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (2 * sizeof(cell))) return false;
	if (!Pawn::attachSpeakerToStreamHandler) return false;

	const auto stream = reinterpret_cast<Stream*>(params[1]);
	const auto playerid = static_cast<uint16_t>(params[2]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::attachSpeakerToStreamHandler(stream, playerid);
		Logger::Log("[sv:dbg:pawn:AttachSpeakerToStream] : stream(%p), playerid(%hu) : return(%hhu)", stream, playerid, result);
		return result;

	}

	return Pawn::attachSpeakerToStreamHandler(stream, playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvHasSpeakerInStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (2 * sizeof(cell))) return false;
	if (!Pawn::hasSpeakerInStreamHandler) return false;

	const auto stream = reinterpret_cast<Stream*>(params[1]);
	const auto playerid = static_cast<uint16_t>(params[2]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::hasSpeakerInStreamHandler(stream, playerid);
		Logger::Log("[sv:dbg:pawn:HasSpeakerInStream] : stream(%p), playerid(%hu) : return(%hhu)", stream, playerid, result);
		return result;

	}

	return Pawn::hasSpeakerInStreamHandler(stream, playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvDetachSpeakerFromStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return false;
	if (params[0] != (2 * sizeof(cell))) return false;
	if (!Pawn::detachSpeakerFromStreamHandler) return false;

	const auto stream = reinterpret_cast<Stream*>(params[1]);
	const auto playerid = static_cast<uint16_t>(params[2]);

	if (Pawn::debugStatus) {

		const auto result = Pawn::detachSpeakerFromStreamHandler(stream, playerid);
		Logger::Log("[sv:dbg:pawn:DetachSpeakerFromStream] : stream(%p), playerid(%hu) : return(%hhu)", stream, playerid, result);
		return result;

	}

	return Pawn::detachSpeakerFromStreamHandler(stream, playerid);

}

cell AMX_NATIVE_CALL Pawn::n_SvDetachAllSpeakersFromStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (1 * sizeof(cell))) return NULL;
	if (!Pawn::detachAllSpeakersFromStreamHandler) return NULL;

	const auto stream = reinterpret_cast<Stream*>(params[1]);

	if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:DetachAllSpeakersFromStream] : stream(%p)", stream);

	Pawn::detachAllSpeakersFromStreamHandler(stream);

	return NULL;

}

cell AMX_NATIVE_CALL Pawn::n_SvDeleteStream(AMX* amx, cell* params) {

	if (!Pawn::initStatus) return NULL;
	if (params[0] != (1 * sizeof(cell))) return NULL;
	if (!Pawn::deleteStreamHandler) return NULL;

	const auto stream = reinterpret_cast<Stream*>(params[1]);

	if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:DeleteStream] : stream(%p)", stream);

	Pawn::deleteStreamHandler(stream);

	return NULL;

}

bool Pawn::Init(
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
) {

	if (Pawn::initStatus) return false;

	Logger::LogToFile("[sv:dbg:pawn:init] : module initializing...");

	Pawn::initHandler = initHandler;
	Pawn::getVersionHandler = getVersionHandler;

	Pawn::hasMicroHandler = hasMicroHandler;
	Pawn::startRecordHandler = startRecordHandler;
	Pawn::stopRecordHandler = stopRecordHandler;

	Pawn::addKeyHandler = addKeyHandler;
	Pawn::hasKeyHandler = hasKeyHandler;
	Pawn::removeKeyHandler = removeKeyHandler;
	Pawn::removeAllKeysHandler = removeAllKeysHandler;

	Pawn::mutePlayerStatusHandler = mutePlayerStatusHandler;
	Pawn::mutePlayerEnableHandler = mutePlayerEnableHandler;
	Pawn::mutePlayerDisableHandler = mutePlayerDisableHandler;

	Pawn::createGStreamHandler = createGStreamHandler;
	Pawn::createSLStreamAtPointHandler = createSLStreamAtPointHandler;
	Pawn::createSLStreamAtVehicleHandler = createSLStreamAtVehicleHandler;
	Pawn::createSLStreamAtPlayerHandler = createSLStreamAtPlayerHandler;
	Pawn::createSLStreamAtObjectHandler = createSLStreamAtObjectHandler;
	Pawn::createDLStreamAtPointHandler = createDLStreamAtPointHandler;
	Pawn::createDLStreamAtVehicleHandler = createDLStreamAtVehicleHandler;
	Pawn::createDLStreamAtPlayerHandler = createDLStreamAtPlayerHandler;
	Pawn::createDLStreamAtObjectHandler = createDLStreamAtObjectHandler;

	Pawn::updatePositionForLPStreamHandler = updatePositionForLPStreamHandler;
	Pawn::updateDistanceForLStreamHandler = updateDistanceForLStreamHandler;

	Pawn::attachListenerToStreamHandler = attachListenerToStreamHandler;
	Pawn::hasListenerInStreamHandler = hasListenerInStreamHandler;
	Pawn::detachListenerFromStreamHandler = detachListenerFromStreamHandler;
	Pawn::detachAllListenersFromStreamHandler = detachAllListenersFromStreamHandler;

	Pawn::attachSpeakerToStreamHandler = attachSpeakerToStreamHandler;
	Pawn::hasSpeakerInStreamHandler = hasSpeakerInStreamHandler;
	Pawn::detachSpeakerFromStreamHandler = detachSpeakerFromStreamHandler;
	Pawn::detachAllSpeakersFromStreamHandler = detachAllSpeakersFromStreamHandler;

	Pawn::deleteStreamHandler = deleteStreamHandler;

	Logger::LogToFile("[sv:dbg:pawn:init] : module initialized");

	return Pawn::initStatus = true;

}

void Pawn::OnPlayerActivationKeyPressForAll(const uint16_t playerid, const uint8_t keyid) {

	if (!Pawn::initStatus) return;

	for (const auto& iCallback : Pawn::callbacksOnPlayerActivationKeyPress)
		iCallback.Call(keyid, playerid);

}

void Pawn::OnPlayerActivationKeyReleaseForAll(const uint16_t playerid, const uint8_t keyid) {

	if (!Pawn::initStatus) return;

	for (const auto& iCallback : Pawn::callbacksOnPlayerActivationKeyRelease)
		iCallback.Call(keyid, playerid);

}

void Pawn::RegisterScript(AMX* const amx) {

	if (!Pawn::initStatus) return;

	const AMX_NATIVE_INFO nativesList[] = {

#define DefineNativeFunction(function) { #function, &Pawn::n_##function }

		DefineNativeFunction(SvDebug),

		DefineNativeFunction(SvInit),
		DefineNativeFunction(SvGetVersion),

		DefineNativeFunction(SvHasMicro),
		DefineNativeFunction(SvStartRecord),
		DefineNativeFunction(SvStopRecord),

		DefineNativeFunction(SvAddKey),
		DefineNativeFunction(SvHasKey),
		DefineNativeFunction(SvRemoveKey),
		DefineNativeFunction(SvRemoveAllKeys),

		DefineNativeFunction(SvMutePlayerStatus),
		DefineNativeFunction(SvMutePlayerEnable),
		DefineNativeFunction(SvMutePlayerDisable),

		DefineNativeFunction(SvCreateGStream),
		DefineNativeFunction(SvCreateSLStreamAtPoint),
		DefineNativeFunction(SvCreateSLStreamAtVehicle),
		DefineNativeFunction(SvCreateSLStreamAtPlayer),
		DefineNativeFunction(SvCreateSLStreamAtObject),
		DefineNativeFunction(SvCreateDLStreamAtPoint),
		DefineNativeFunction(SvCreateDLStreamAtVehicle),
		DefineNativeFunction(SvCreateDLStreamAtPlayer),
		DefineNativeFunction(SvCreateDLStreamAtObject),

		DefineNativeFunction(SvUpdateDistanceForLStream),
		DefineNativeFunction(SvUpdatePositionForLPStream),

		DefineNativeFunction(SvAttachListenerToStream),
		DefineNativeFunction(SvHasListenerInStream),
		DefineNativeFunction(SvDetachListenerFromStream),
		DefineNativeFunction(SvDetachAllListenersFromStream),

		DefineNativeFunction(SvAttachSpeakerToStream),
		DefineNativeFunction(SvHasSpeakerInStream),
		DefineNativeFunction(SvDetachSpeakerFromStream),
		DefineNativeFunction(SvDetachAllSpeakersFromStream),

		DefineNativeFunction(SvDeleteStream)

#undef	DefineNativeFunction

	};

	amx_Register(amx, nativesList, sizeof(nativesList) / sizeof(*nativesList));

	int tmpIndex = -1;

	Logger::LogToFile("[sv:dbg:pawn:register] : finding 'OnPlayerActivationKeyPress' callback function...");

	if (!amx_FindPublic(amx, "OnPlayerActivationKeyPress", &tmpIndex) && tmpIndex >= 0) {

		Logger::LogToFile("[sv:dbg:pawn:register] : finded 'OnPlayerActivationKeyPress' callback function (index:%d)", tmpIndex);
		Pawn::callbacksOnPlayerActivationKeyPress.emplace_back(amx, tmpIndex);

	}

	Logger::LogToFile("[sv:dbg:pawn:register] : finding 'OnPlayerActivationKeyRelease' callback function...");

	if (!amx_FindPublic(amx, "OnPlayerActivationKeyRelease", &tmpIndex) && tmpIndex >= 0) {

		Logger::LogToFile("[sv:dbg:pawn:register] : finded 'OnPlayerActivationKeyRelease' callback function (index:%d)", tmpIndex);
		Pawn::callbacksOnPlayerActivationKeyRelease.emplace_back(amx, tmpIndex);

	}

}

void Pawn::Free() {

	if (!Pawn::initStatus) return;

	Logger::LogToFile("[sv:dbg:pawn:free] : module releasing...");

	Pawn::debugStatus = false;

	Pawn::initHandler = nullptr;
	Pawn::getVersionHandler = nullptr;

	Pawn::hasMicroHandler = nullptr;
	Pawn::startRecordHandler = nullptr;
	Pawn::stopRecordHandler = nullptr;

	Pawn::addKeyHandler = nullptr;
	Pawn::hasKeyHandler = nullptr;
	Pawn::removeKeyHandler = nullptr;
	Pawn::removeAllKeysHandler = nullptr;

	Pawn::mutePlayerStatusHandler = nullptr;
	Pawn::mutePlayerEnableHandler = nullptr;
	Pawn::mutePlayerDisableHandler = nullptr;

	Pawn::createGStreamHandler = nullptr;
	Pawn::createSLStreamAtPointHandler = nullptr;
	Pawn::createSLStreamAtVehicleHandler = nullptr;
	Pawn::createSLStreamAtPlayerHandler = nullptr;
	Pawn::createSLStreamAtObjectHandler = nullptr;
	Pawn::createDLStreamAtPointHandler = nullptr;
	Pawn::createDLStreamAtVehicleHandler = nullptr;
	Pawn::createDLStreamAtPlayerHandler = nullptr;
	Pawn::createDLStreamAtObjectHandler = nullptr;

	Pawn::updatePositionForLPStreamHandler = nullptr;
	Pawn::updateDistanceForLStreamHandler = nullptr;

	Pawn::attachListenerToStreamHandler = nullptr;
	Pawn::hasListenerInStreamHandler = nullptr;
	Pawn::detachListenerFromStreamHandler = nullptr;
	Pawn::detachAllListenersFromStreamHandler = nullptr;

	Pawn::attachSpeakerToStreamHandler = nullptr;
	Pawn::hasSpeakerInStreamHandler = nullptr;
	Pawn::detachSpeakerFromStreamHandler = nullptr;
	Pawn::detachAllSpeakersFromStreamHandler = nullptr;

	Pawn::deleteStreamHandler = nullptr;

	Pawn::callbacksOnPlayerActivationKeyPress.clear();
	Pawn::callbacksOnPlayerActivationKeyRelease.clear();

	Logger::LogToFile("[sv:dbg:pawn:free] : module released");

	Pawn::initStatus = false;

}
