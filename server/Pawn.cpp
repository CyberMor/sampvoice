/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Pawn.h"

#include <util/logger.h>
#include <util/memory.hpp>

bool Pawn::Init(PawnInterfacePtr pInterface) noexcept
{
    return (Pawn::pInterface = std::move(pInterface)) != nullptr;
}

void Pawn::Free() noexcept
{
    if (Pawn::pInterface == nullptr) return;

    Logger::LogToFile("[sv:dbg:pawn:free] : module releasing...");

    Pawn::debugStatus = false;

    Pawn::callbacksOnPlayerActivationKeyPress.clear();
    Pawn::callbacksOnPlayerActivationKeyRelease.clear();

    Logger::LogToFile("[sv:dbg:pawn:free] : module released");

    Pawn::pInterface = nullptr;
}

void Pawn::RegisterScript(AMX* const amx)
{
    if (Pawn::pInterface == nullptr) return;

    const AMX_NATIVE_INFO nativesList[] =
    {
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
        DefineNativeFunction(SvStreamParameterSet),
        DefineNativeFunction(SvStreamParameterReset),
        DefineNativeFunction(SvStreamParameterHas),
        DefineNativeFunction(SvStreamParameterGet),
        DefineNativeFunction(SvStreamParameterSlideFromTo),
        DefineNativeFunction(SvStreamParameterSlideTo),
        DefineNativeFunction(SvStreamParameterSlide),
        DefineNativeFunction(SvDeleteStream),

        DefineNativeFunction(SvEffectCreateChorus),
        DefineNativeFunction(SvEffectCreateCompressor),
        DefineNativeFunction(SvEffectCreateDistortion),
        DefineNativeFunction(SvEffectCreateEcho),
        DefineNativeFunction(SvEffectCreateFlanger),
        DefineNativeFunction(SvEffectCreateGargle),
        DefineNativeFunction(SvEffectCreateI3dl2reverb),
        DefineNativeFunction(SvEffectCreateParameq),
        DefineNativeFunction(SvEffectCreateReverb),
        DefineNativeFunction(SvEffectAttachStream),
        DefineNativeFunction(SvEffectDetachStream),
        DefineNativeFunction(SvEffectDelete)

#undef  DefineNativeFunction
    };

    amx_Register(amx, nativesList, SizeOfArray(nativesList));

    int tmpIndex { -1 };

    Logger::LogToFile("[sv:dbg:pawn:register] : finding 'OnPlayerActivationKeyPress' callback function...");

    if (amx_FindPublic(amx, "OnPlayerActivationKeyPress", &tmpIndex) == 0 && tmpIndex >= 0)
    {
        Logger::LogToFile("[sv:dbg:pawn:register] : finded 'OnPlayerActivationKeyPress' callback function (index:%d)", tmpIndex);
        Pawn::callbacksOnPlayerActivationKeyPress.emplace_back(amx, tmpIndex);
    }

    Logger::LogToFile("[sv:dbg:pawn:register] : finding 'OnPlayerActivationKeyRelease' callback function...");

    if (amx_FindPublic(amx, "OnPlayerActivationKeyRelease", &tmpIndex) == 0 && tmpIndex >= 0)
    {
        Logger::LogToFile("[sv:dbg:pawn:register] : finded 'OnPlayerActivationKeyRelease' callback function (index:%d)", tmpIndex);
        Pawn::callbacksOnPlayerActivationKeyRelease.emplace_back(amx, tmpIndex);
    }
}

void Pawn::OnPlayerActivationKeyPressForAll(const uint16_t playerid, const uint8_t keyid) noexcept
{
    if (Pawn::pInterface == nullptr) return;

    for (const auto& iCallback : Pawn::callbacksOnPlayerActivationKeyPress)
        iCallback.Call(keyid, playerid);
}

void Pawn::OnPlayerActivationKeyReleaseForAll(const uint16_t playerid, const uint8_t keyid) noexcept
{
    if (Pawn::pInterface == nullptr) return;

    for (const auto& iCallback : Pawn::callbacksOnPlayerActivationKeyRelease)
        iCallback.Call(keyid, playerid);
}

cell AMX_NATIVE_CALL Pawn::n_SvDebug(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    Pawn::debugStatus = static_cast<bool>(params[1]);

    Logger::Log(
        "[sv:dbg:pawn:SvDebug] : mode(%hhu)",
        Pawn::debugStatus
    );

    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvInit(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto bitrate = static_cast<uint32_t>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvInit] : bitrate(%u)",
        bitrate
    );

    Pawn::pInterface->SvInit(bitrate);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvGetVersion(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvGetVersion(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvGetVersion] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvHasMicro(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 1 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvHasMicro(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvHasMicro] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvStartRecord(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 1 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvStartRecord(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvStartRecord] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvStopRecord(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 1 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvStopRecord(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvStopRecord] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvAddKey(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 2 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);
    const auto keyid = static_cast<uint8_t>(params[2]);

    const auto result = Pawn::pInterface->SvAddKey(playerid, keyid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvAddKey] : playerid(%hu), keyid(0x%hhx) : return(%hhu)",
        playerid, keyid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvHasKey(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 2 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);
    const auto keyid = static_cast<uint8_t>(params[2]);

    const auto result = Pawn::pInterface->SvHasKey(playerid, keyid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvHasKey] : playerid(%hu), keyid(0x%hhx) : return(%hhu)",
        playerid, keyid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvRemoveKey(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 2 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);
    const auto keyid = static_cast<uint8_t>(params[2]);

    const auto result = Pawn::pInterface->SvRemoveKey(playerid, keyid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvRemoveKey] : playerid(%hu), keyid(0x%hhx) : return(%hhu)",
        playerid, keyid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvRemoveAllKeys(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto playerid = static_cast<uint16_t>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvRemoveAllKeys] : playerid(%hu)",
        playerid
    );

    Pawn::pInterface->SvRemoveAllKeys(playerid);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvMutePlayerStatus(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 1 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvMutePlayerStatus(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvMutePlayerStatus] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvMutePlayerEnable(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto playerid = static_cast<uint16_t>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvMutePlayerEnable] : playerid(%hu)",
        playerid
    );

    Pawn::pInterface->SvMutePlayerEnable(playerid);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvMutePlayerDisable(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto playerid = static_cast<uint16_t>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvMutePlayerDisable] : playerid(%hu)",
        playerid
    );

    Pawn::pInterface->SvMutePlayerDisable(playerid);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateGStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 2 * sizeof(cell)) return NULL;

    const auto color = static_cast<uint32_t>(params[1]);

    cell* phys_addr { nullptr }; int tmp_len { 0 };
    if (amx_GetAddr(amx, params[2], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
    std::string name(tmp_len + 1, '\0');
    if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

    const auto result = Pawn::pInterface->SvCreateGStream(color, name);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvCreateGStream] : color(0x%x), name(%s) : return(%p)",
        color, name.c_str(), result
    );

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtPoint(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 6 * sizeof(cell)) return NULL;

    const auto distance = amx_ctof(params[1]);
    const auto posx = amx_ctof(params[2]);
    const auto posy = amx_ctof(params[3]);
    const auto posz = amx_ctof(params[4]);
    const auto color = static_cast<uint32_t>(params[5]);

    cell* phys_addr { nullptr }; int tmp_len { 0 };
    if (amx_GetAddr(amx, params[6], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
    std::string name(tmp_len + 1, '\0');
    if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

    const auto result = Pawn::pInterface->SvCreateSLStreamAtPoint(distance, posx, posy, posz, color, name);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvCreateSLStreamAtPoint] : distance(%.2f), "
        "pos(%.2f;%.2f;%.2f), color(0x%x), name(%s) : return(%p)",
        distance, posx, posy, posz, color, name.c_str(), result
    );

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtVehicle(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 4 * sizeof(cell)) return NULL;

    const auto distance = amx_ctof(params[1]);
    const auto vehicleid = static_cast<uint16_t>(params[2]);
    const auto color = static_cast<uint32_t>(params[3]);

    cell* phys_addr { nullptr }; int tmp_len { 0 };
    if (amx_GetAddr(amx, params[4], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
    std::string name(tmp_len + 1, '\0');
    if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

    const auto result = Pawn::pInterface->SvCreateSLStreamAtVehicle(distance, vehicleid, color, name);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvCreateSLStreamAtVehicle] : distance(%.2f), "
        "vehicleid(%hu), color(0x%x), name(%s) : return(%p)",
        distance, vehicleid, color, name.c_str(), result
    );

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtPlayer(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 4 * sizeof(cell)) return NULL;

    const auto distance = amx_ctof(params[1]);
    const auto playerid = static_cast<uint16_t>(params[2]);
    const auto color = static_cast<uint32_t>(params[3]);

    cell* phys_addr { nullptr }; int tmp_len { 0 };
    if (amx_GetAddr(amx, params[4], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
    std::string name(tmp_len + 1, '\0');
    if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

    const auto result = Pawn::pInterface->SvCreateSLStreamAtPlayer(distance, playerid, color, name);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvCreateSLStreamAtPlayer] : distance(%.2f), "
        "playerid(%hu), color(0x%x), name(%s) : return(%p)",
        distance, playerid, color, name.c_str(), result
    );

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtObject(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 4 * sizeof(cell)) return NULL;

    const auto distance = amx_ctof(params[1]);
    const auto objectid = static_cast<uint16_t>(params[2]);
    const auto color = static_cast<uint32_t>(params[3]);

    cell* phys_addr { nullptr }; int tmp_len { 0 };
    if (amx_GetAddr(amx, params[4], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
    std::string name(tmp_len + 1, '\0');
    if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

    const auto result = Pawn::pInterface->SvCreateSLStreamAtObject(distance, objectid, color, name);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvCreateSLStreamAtObject] : distance(%.2f), "
        "objectid(%hu), color(0x%x), name(%s) : return(%p)",
        distance, objectid, color, name.c_str(), result
    );

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtPoint(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 7 * sizeof(cell)) return NULL;

    const auto distance = amx_ctof(params[1]);
    const auto maxplayers = static_cast<uint32_t>(params[2]);
    const auto posx = amx_ctof(params[3]);
    const auto posy = amx_ctof(params[4]);
    const auto posz = amx_ctof(params[5]);
    const auto color = static_cast<uint32_t>(params[6]);

    cell* phys_addr { nullptr }; int tmp_len { 0 };
    if (amx_GetAddr(amx, params[7], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
    std::string name(tmp_len + 1, '\0');
    if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

    const auto result = Pawn::pInterface->SvCreateDLStreamAtPoint(distance, maxplayers, posx, posy, posz, color, name);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvCreateDLStreamAtPoint] : distance(%.2f), "
        "maxplayers(%u), pos(%.2f;%.2f;%.2f), color(0x%x), name(%s) : return(%p)",
        distance, maxplayers, posx, posy, posz, color, name.c_str(), result
    );

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtVehicle(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 5 * sizeof(cell)) return NULL;

    const auto distance = amx_ctof(params[1]);
    const auto maxplayers = static_cast<uint32_t>(params[2]);
    const auto vehicleid = static_cast<uint16_t>(params[3]);
    const auto color = static_cast<uint32_t>(params[4]);

    cell* phys_addr { nullptr }; int tmp_len { 0 };
    if (amx_GetAddr(amx, params[5], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
    std::string name(tmp_len + 1, '\0');
    if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

    const auto result = Pawn::pInterface->SvCreateDLStreamAtVehicle(distance, maxplayers, vehicleid, color, name);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvCreateDLStreamAtVehicle] : distance(%.2f), "
        "maxplayers(%u), vehicleid(%hu), color(0x%x), name(%s) : return(%p)",
        distance, maxplayers, vehicleid, color, name.c_str(), result
    );

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtPlayer(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 5 * sizeof(cell)) return NULL;

    const auto distance = amx_ctof(params[1]);
    const auto maxplayers = static_cast<uint32_t>(params[2]);
    const auto playerid = static_cast<uint16_t>(params[3]);
    const auto color = static_cast<uint32_t>(params[4]);

    cell* phys_addr { nullptr }; int tmp_len { 0 };
    if (amx_GetAddr(amx, params[5], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
    std::string name(tmp_len + 1, '\0');
    if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

    const auto result = Pawn::pInterface->SvCreateDLStreamAtPlayer(distance, maxplayers, playerid, color, name);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvCreateDLStreamAtPlayer] : distance(%.2f), "
        "maxplayers(%u), playerid(%hu), color(0x%x), name(%s) : return(%p)",
        distance, maxplayers, playerid, color, name.c_str(), result
    );

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtObject(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 5 * sizeof(cell)) return NULL;

    const auto distance = amx_ctof(params[1]);
    const auto maxplayers = static_cast<uint32_t>(params[2]);
    const auto objectid = static_cast<uint16_t>(params[3]);
    const auto color = static_cast<uint32_t>(params[4]);

    cell* phys_addr { nullptr }; int tmp_len { 0 };
    if (amx_GetAddr(amx, params[5], &phys_addr) || amx_StrLen(phys_addr, &tmp_len)) return NULL;
    std::string name(tmp_len + 1, '\0');
    if (amx_GetString(name.data(), phys_addr, false, tmp_len + 1)) return NULL;

    const auto result = Pawn::pInterface->SvCreateDLStreamAtObject(distance, maxplayers, objectid, color, name);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvCreateDLStreamAtObject] : distance(%.2f), "
        "maxplayers(%u), objectid(%hu), color(0x%x), name(%s) : return(%p)",
        distance, maxplayers, objectid, color, name.c_str(), result
    );

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvUpdateDistanceForLStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 2 * sizeof(cell)) return NULL;

    const auto lstream = dynamic_cast<LocalStream*>(reinterpret_cast<Stream*>(params[1]));
    if (!lstream) return NULL;

    const auto distance = amx_ctof(params[2]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvUpdateDistanceForLStream] : lstream(%p), distance(%.2f)",
        lstream, distance
    );

    Pawn::pInterface->SvUpdateDistanceForLStream(lstream, distance);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvUpdatePositionForLPStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 4 * sizeof(cell)) return NULL;

    const auto lpstream = dynamic_cast<PointStream*>(reinterpret_cast<Stream*>(params[1]));
    if (!lpstream) return NULL;

    const auto posx = amx_ctof(params[2]);
    const auto posy = amx_ctof(params[3]);
    const auto posz = amx_ctof(params[4]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvUpdatePositionForLPStream] : lpstream(%p), pos(%.2f;%.2f;%.2f)",
        lpstream, posx, posy, posz
    );

    Pawn::pInterface->SvUpdatePositionForLPStream(lpstream, posx, posy, posz);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvAttachListenerToStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 2 * sizeof(cell)) return false;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto playerid = static_cast<uint16_t>(params[2]);

    const auto result = Pawn::pInterface->SvAttachListenerToStream(stream, playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvAttachListenerToStream] : stream(%p), playerid(%hu) : return(%hhu)",
        stream, playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvHasListenerInStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 2 * sizeof(cell)) return false;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto playerid = static_cast<uint16_t>(params[2]);

    const auto result = Pawn::pInterface->SvHasListenerInStream(stream, playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvHasListenerInStream] : stream(%p), playerid(%hu) : return(%hhu)",
        stream, playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvDetachListenerFromStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 2 * sizeof(cell)) return false;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto playerid = static_cast<uint16_t>(params[2]);

    const auto result = Pawn::pInterface->SvDetachListenerFromStream(stream, playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvDetachListenerFromStream] : stream(%p), playerid(%hu) : return(%hhu)",
        stream, playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvDetachAllListenersFromStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvDetachAllListenersFromStream] : stream(%p)",
        stream
    );

    Pawn::pInterface->SvDetachAllListenersFromStream(stream);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvAttachSpeakerToStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 2 * sizeof(cell)) return false;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto playerid = static_cast<uint16_t>(params[2]);

    const auto result = Pawn::pInterface->SvAttachSpeakerToStream(stream, playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvAttachSpeakerToStream] : stream(%p), playerid(%hu) : return(%hhu)",
        stream, playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvHasSpeakerInStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 2 * sizeof(cell)) return false;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto playerid = static_cast<uint16_t>(params[2]);

    const auto result = Pawn::pInterface->SvHasSpeakerInStream(stream, playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvHasSpeakerInStream] : stream(%p), playerid(%hu) : return(%hhu)",
        stream, playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvDetachSpeakerFromStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return false;
    if (params[0] != 2 * sizeof(cell)) return false;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto playerid = static_cast<uint16_t>(params[2]);

    const auto result = Pawn::pInterface->SvDetachSpeakerFromStream(stream, playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvDetachSpeakerFromStream] : stream(%p), playerid(%hu) : return(%hhu)",
        stream, playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvDetachAllSpeakersFromStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvDetachAllSpeakersFromStream] : stream(%p)",
        stream
    );

    Pawn::pInterface->SvDetachAllSpeakersFromStream(stream);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvStreamParameterSet(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 3 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto parameter = static_cast<uint8_t>(params[2]);
    const auto value = amx_ctof(params[3]);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvStreamParameterSet] : "
        "stream(%p), parameter(%hhu), value(%.2f)", stream, parameter, value);

    Pawn::pInterface->SvStreamParameterSet(stream, parameter, value);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvStreamParameterReset(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 2 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto parameter = static_cast<uint8_t>(params[2]);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvStreamParameterReset] : "
        "stream(%p), parameter(%hhu)", stream, parameter);

    Pawn::pInterface->SvStreamParameterReset(stream, parameter);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvStreamParameterHas(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 2 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto parameter = static_cast<uint8_t>(params[2]);

    const auto result = Pawn::pInterface->SvStreamParameterHas(stream, parameter);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvStreamParameterHas] : "
        "stream(%p), parameter(%hhu) : return(%hhu)", stream, parameter, result);

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvStreamParameterGet(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 2 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto parameter = static_cast<uint8_t>(params[2]);

    const auto result = Pawn::pInterface->SvStreamParameterGet(stream, parameter);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvStreamParameterGet] : "
        "stream(%p), parameter(%hhu) : return(%.2f)", stream, parameter, result);

    return amx_ftoc(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvStreamParameterSlideFromTo(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 5 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto parameter = static_cast<uint8_t>(params[2]);
    const auto startvalue = amx_ctof(params[3]);
    const auto endvalue = amx_ctof(params[4]);
    const auto time = static_cast<uint32_t>(params[5]);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvStreamParameterSlideFromTo] : "
        "stream(%p), parameter(%hhu), startvalue(%.2f), endvalue(%.2f), time(%u)",
        stream, parameter, startvalue, endvalue, time);

    Pawn::pInterface->SvStreamParameterSlideFromTo(stream, parameter, startvalue, endvalue, time);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvStreamParameterSlideTo(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 4 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto parameter = static_cast<uint8_t>(params[2]);
    const auto endvalue = amx_ctof(params[4]);
    const auto time = static_cast<uint32_t>(params[5]);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvStreamParameterSlideTo] : "
        "stream(%p), parameter(%hhu), endvalue(%.2f), time(%u)",
        stream, parameter, endvalue, time);

    Pawn::pInterface->SvStreamParameterSlideTo(stream, parameter, endvalue, time);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvStreamParameterSlide(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 4 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);
    const auto parameter = static_cast<uint8_t>(params[2]);
    const auto deltavalue = amx_ctof(params[4]);
    const auto time = static_cast<uint32_t>(params[5]);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvStreamParameterSlide] : "
        "stream(%p), parameter(%hhu), deltavalue(%.2f), time(%u)",
        stream, parameter, deltavalue, time);

    Pawn::pInterface->SvStreamParameterSlide(stream, parameter, deltavalue, time);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvDeleteStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvDeleteStream] : stream(%p)",
        stream
    );

    Pawn::pInterface->SvDeleteStream(stream);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectCreateChorus(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 8 * sizeof(cell)) return NULL;

    const auto priority = static_cast<int>(params[1]);
    const auto wetdrymix = amx_ctof(params[2]);
    const auto depth = amx_ctof(params[3]);
    const auto feedback = amx_ctof(params[4]);
    const auto frequency = amx_ctof(params[5]);
    const auto waveform = static_cast<uint32_t>(params[6]);
    const auto delay = amx_ctof(params[7]);
    const auto phase = static_cast<uint32_t>(params[8]);

    const auto result = Pawn::pInterface->SvEffectCreateChorus(priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectCreateChorus] : "
        "priority(%d), wetdrymix(%.2f), depth(%.2f), feedback(%.2f), frequency(%.2f), waveform(%u), delay(%.2f), phase(%u) : return(%p)",
        priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase, result);

    return reinterpret_cast<cell>(result);

}

cell AMX_NATIVE_CALL Pawn::n_SvEffectCreateCompressor(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 7 * sizeof(cell)) return NULL;

    const auto priority = static_cast<int>(params[1]);
    const auto gain = amx_ctof(params[2]);
    const auto attack = amx_ctof(params[3]);
    const auto release = amx_ctof(params[4]);
    const auto threshold = amx_ctof(params[5]);
    const auto ratio = amx_ctof(params[6]);
    const auto predelay = amx_ctof(params[7]);

    const auto result = Pawn::pInterface->SvEffectCreateCompressor(priority, gain, attack, release, threshold, ratio, predelay);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectCreateCompressor] : "
        "priority(%d), gain(%.2f), attack(%.2f), release(%.2f), threshold(%.2f), ratio(%.2f), predelay(%.2f) : return(%p)",
        priority, gain, attack, release, threshold, ratio, predelay, result);

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectCreateDistortion(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 6 * sizeof(cell)) return NULL;

    const auto priority = static_cast<int>(params[1]);
    const auto gain = amx_ctof(params[2]);
    const auto edge = amx_ctof(params[3]);
    const auto posteqcenterfrequency = amx_ctof(params[4]);
    const auto posteqbandwidth = amx_ctof(params[5]);
    const auto prelowpasscutoff = amx_ctof(params[6]);

    const auto result = Pawn::pInterface->SvEffectCreateDistortion(priority, gain, edge, posteqcenterfrequency, posteqbandwidth, prelowpasscutoff);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectCreateDistortion] : "
        "priority(%d), gain(%.2f), edge(%.2f), posteqcenterfrequency(%.2f), posteqbandwidth(%.2f), prelowpasscutoff(%.2f) : return(%p)",
        priority, gain, edge, posteqcenterfrequency, posteqbandwidth, prelowpasscutoff, result);

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectCreateEcho(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 6 * sizeof(cell)) return NULL;

    const auto priority = static_cast<int>(params[1]);
    const auto wetdrymix = amx_ctof(params[2]);
    const auto feedback = amx_ctof(params[3]);
    const auto leftdelay = amx_ctof(params[4]);
    const auto rightdelay = amx_ctof(params[5]);
    const auto pandelay = static_cast<bool>(params[6]);

    const auto result = Pawn::pInterface->SvEffectCreateEcho(priority, wetdrymix, feedback, leftdelay, rightdelay, pandelay);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectCreateEcho] : "
        "priority(%d), wetdrymix(%.2f), feedback(%.2f), leftdelay(%.2f), rightdelay(%.2f), pandelay(%hhu) : return(%p)",
        priority, wetdrymix, feedback, leftdelay, rightdelay, pandelay, result);

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectCreateFlanger(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 8 * sizeof(cell)) return NULL;

    const auto priority = static_cast<int>(params[1]);
    const auto wetdrymix = amx_ctof(params[2]);
    const auto depth = amx_ctof(params[3]);
    const auto feedback = amx_ctof(params[4]);
    const auto frequency = amx_ctof(params[5]);
    const auto waveform = static_cast<uint32_t>(params[6]);
    const auto delay = amx_ctof(params[7]);
    const auto phase = static_cast<uint32_t>(params[8]);

    const auto result = Pawn::pInterface->SvEffectCreateFlanger(priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectCreateFlanger] : "
        "priority(%d), wetdrymix(%.2f), depth(%.2f), feedback(%.2f), frequency(%.2f), waveform(%u), delay(%.2f), phase(%u) : return(%p)",
        priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase, result);

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectCreateGargle(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 3 * sizeof(cell)) return NULL;

    const auto priority = static_cast<int>(params[1]);
    const auto ratehz = static_cast<uint32_t>(params[2]);
    const auto waveshape = static_cast<uint32_t>(params[3]);

    const auto result = Pawn::pInterface->SvEffectCreateGargle(priority, ratehz, waveshape);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectCreateGargle] : "
        "priority(%d), ratehz(%u), waveshape(%u) : return(%p)",
        priority, ratehz, waveshape, result);

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectCreateI3dl2reverb(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 13 * sizeof(cell)) return NULL;

    const auto priority = static_cast<int>(params[1]);
    const auto room = static_cast<int>(params[2]);
    const auto roomhf = static_cast<int>(params[3]);
    const auto roomrollofffactor = amx_ctof(params[4]);
    const auto decaytime = amx_ctof(params[5]);
    const auto decayhfratio = amx_ctof(params[6]);
    const auto reflections = static_cast<int>(params[7]);
    const auto reflectionsdelay = amx_ctof(params[8]);
    const auto reverb = static_cast<int>(params[9]);
    const auto reverbdelay = amx_ctof(params[10]);
    const auto diffusion = amx_ctof(params[11]);
    const auto density = amx_ctof(params[12]);
    const auto hfreference = amx_ctof(params[13]);

    const auto result = Pawn::pInterface->SvEffectCreateI3dl2reverb(
        priority, room, roomhf, roomrollofffactor, decaytime, decayhfratio,
        reflections, reflectionsdelay, reverb, reverbdelay, diffusion, density, hfreference
    );

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectCreateI3dl2reverb] : "
        "priority(%d), room(%d), roomhf(%d), roomrollofffactor(%.2f), decaytime(%.2f), "
        "decayhfratio(%.2f), reflections(%d), reflectionsdelay(%.2f), reverb(%d), "
        "reverbdelay(%.2f), diffusion(%.2f), density(%.2f), hfreference(%.2f) : return(%p)",
        priority, room, roomhf, roomrollofffactor, decaytime, decayhfratio, reflections,
        reflectionsdelay, reverb, reverbdelay, diffusion, density, hfreference, result);

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectCreateParameq(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 4 * sizeof(cell)) return NULL;

    const auto priority = static_cast<int>(params[1]);
    const auto center = amx_ctof(params[2]);
    const auto bandwidth = amx_ctof(params[3]);
    const auto gain = amx_ctof(params[4]);

    const auto result = Pawn::pInterface->SvEffectCreateParameq(priority, center, bandwidth, gain);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectCreateParameq] : "
        "priority(%d), center(%.2f), bandwidth(%.2f), gain(%.2f) : return(%p)",
        priority, center, bandwidth, gain, result);

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectCreateReverb(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 5 * sizeof(cell)) return NULL;

    const auto priority = static_cast<int>(params[1]);
    const auto ingain = amx_ctof(params[2]);
    const auto reverbmix = amx_ctof(params[3]);
    const auto reverbtime = amx_ctof(params[4]);
    const auto highfreqrtratio = amx_ctof(params[5]);

    const auto result = Pawn::pInterface->SvEffectCreateReverb(priority, ingain, reverbmix, reverbtime, highfreqrtratio);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectCreateReverb] : "
        "priority(%d), ingain(%.2f), reverbmix(%.2f), reverbtime(%.2f), highfreqrtratio(%.2f) : return(%p)",
        priority, ingain, reverbmix, reverbtime, highfreqrtratio, result);

    return reinterpret_cast<cell>(result);
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectAttachStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 2 * sizeof(cell)) return NULL;

    const auto effect = reinterpret_cast<Effect*>(params[1]);
    const auto stream = reinterpret_cast<Stream*>(params[2]);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectAttachStream] : "
        "effect(%p), stream(%p)", effect, stream);

    Pawn::pInterface->SvEffectAttachStream(effect, stream);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectDetachStream(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 2 * sizeof(cell)) return NULL;

    const auto effect = reinterpret_cast<Effect*>(params[1]);
    const auto stream = reinterpret_cast<Stream*>(params[2]);

    if (Pawn::debugStatus) Logger::Log("[sv:dbg:pawn:SvEffectDetachStream] : "
        "effect(%p), stream(%p)", effect, stream);

    Pawn::pInterface->SvEffectDetachStream(effect, stream);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvEffectDelete(AMX* const amx, cell* const params)
{
    if (Pawn::pInterface == nullptr) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto effect = reinterpret_cast<Effect*>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvEffectDelete] : effect(%p)",
        effect
    );

    Pawn::pInterface->SvEffectDelete(effect);
    return NULL;
}

bool Pawn::debugStatus { false };

PawnInterfacePtr Pawn::pInterface { nullptr };

std::vector<Pawn::AmxCallback> Pawn::callbacksOnPlayerActivationKeyPress;
std::vector<Pawn::AmxCallback> Pawn::callbacksOnPlayerActivationKeyRelease;
