/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Pawn.h"

#include <util/logger.h>
#include <util/memory.hpp>

bool Pawn::Init(PawnInterfacePtr&& pInterface) noexcept
{
    return !!(Pawn::pInterface = std::move(pInterface));
}

void Pawn::Free() noexcept
{
    if (!Pawn::pInterface) return;

    Logger::LogToFile("[sv:dbg:pawn:free] : module releasing...");

    Pawn::debugStatus = false;

    Pawn::callbacksOnPlayerActivationKeyPress.clear();
    Pawn::callbacksOnPlayerActivationKeyRelease.clear();

    Logger::LogToFile("[sv:dbg:pawn:free] : module released");

    Pawn::pInterface = nullptr;
}

void Pawn::RegisterScript(AMX* const amx)
{
    if (!Pawn::pInterface) return;

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

        DefineNativeFunction(SvDeleteStream)

#undef  DefineNativeFunction
    };

    amx_Register(amx, nativesList, SizeOfArray(nativesList));

    int tmpIndex { -1 };

    Logger::LogToFile("[sv:dbg:pawn:register] : finding 'OnPlayerActivationKeyPress' callback function...");

    if (!amx_FindPublic(amx, "OnPlayerActivationKeyPress", &tmpIndex) && tmpIndex >= 0)
    {
        Logger::LogToFile("[sv:dbg:pawn:register] : finded 'OnPlayerActivationKeyPress' callback function (index:%d)", tmpIndex);
        Pawn::callbacksOnPlayerActivationKeyPress.emplace_back(amx, tmpIndex);
    }

    Logger::LogToFile("[sv:dbg:pawn:register] : finding 'OnPlayerActivationKeyRelease' callback function...");

    if (!amx_FindPublic(amx, "OnPlayerActivationKeyRelease", &tmpIndex) && tmpIndex >= 0)
    {
        Logger::LogToFile("[sv:dbg:pawn:register] : finded 'OnPlayerActivationKeyRelease' callback function (index:%d)", tmpIndex);
        Pawn::callbacksOnPlayerActivationKeyRelease.emplace_back(amx, tmpIndex);
    }
}

void Pawn::OnPlayerActivationKeyPressForAll(const uint16_t playerid, const uint8_t keyid) noexcept
{
    if (!Pawn::pInterface) return;

    for (const auto& iCallback : Pawn::callbacksOnPlayerActivationKeyPress)
        iCallback.Call(keyid, playerid);
}

void Pawn::OnPlayerActivationKeyReleaseForAll(const uint16_t playerid, const uint8_t keyid) noexcept
{
    if (!Pawn::pInterface) return;

    for (const auto& iCallback : Pawn::callbacksOnPlayerActivationKeyRelease)
        iCallback.Call(keyid, playerid);
}

cell AMX_NATIVE_CALL Pawn::n_SvDebug(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    Pawn::debugStatus = static_cast<bool>(params[1]);

    Logger::Log(
        "[sv:dbg:pawn:SvDebug] : mode(%hhu)",
        Pawn::debugStatus
    );

    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvInit(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto bitrate = static_cast<uint32_t>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvInit] : bitrate(%u)",
        bitrate
    );

    Pawn::pInterface->SvInit(bitrate);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvGetVersion(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvGetVersion(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvGetVersion] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvHasMicro(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
    if (params[0] != 1 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvHasMicro(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvHasMicro] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvStartRecord(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
    if (params[0] != 1 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvStartRecord(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvStartRecord] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvStopRecord(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
    if (params[0] != 1 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvStopRecord(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvStopRecord] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvAddKey(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
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

cell AMX_NATIVE_CALL Pawn::n_SvHasKey(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
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

cell AMX_NATIVE_CALL Pawn::n_SvRemoveKey(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
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

cell AMX_NATIVE_CALL Pawn::n_SvRemoveAllKeys(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto playerid = static_cast<uint16_t>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvRemoveAllKeys] : playerid(%hu)",
        playerid
    );

    Pawn::pInterface->SvRemoveAllKeys(playerid);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvMutePlayerStatus(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
    if (params[0] != 1 * sizeof(cell)) return false;

    const auto playerid = static_cast<uint16_t>(params[1]);

    const auto result = Pawn::pInterface->SvMutePlayerStatus(playerid);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvMutePlayerStatus] : playerid(%hu) : return(%hhu)",
        playerid, result
    );

    return result;
}

cell AMX_NATIVE_CALL Pawn::n_SvMutePlayerEnable(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto playerid = static_cast<uint16_t>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvMutePlayerEnable] : playerid(%hu)",
        playerid
    );

    Pawn::pInterface->SvMutePlayerEnable(playerid);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvMutePlayerDisable(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto playerid = static_cast<uint16_t>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvMutePlayerDisable] : playerid(%hu)",
        playerid
    );

    Pawn::pInterface->SvMutePlayerDisable(playerid);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvCreateGStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtPoint(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtVehicle(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtPlayer(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvCreateSLStreamAtObject(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtPoint(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtVehicle(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtPlayer(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvCreateDLStreamAtObject(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvUpdateDistanceForLStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvUpdatePositionForLPStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
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

cell AMX_NATIVE_CALL Pawn::n_SvAttachListenerToStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
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

cell AMX_NATIVE_CALL Pawn::n_SvHasListenerInStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
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

cell AMX_NATIVE_CALL Pawn::n_SvDetachListenerFromStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
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

cell AMX_NATIVE_CALL Pawn::n_SvDetachAllListenersFromStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvDetachAllListenersFromStream] : stream(%p)",
        stream
    );

    Pawn::pInterface->SvDetachAllListenersFromStream(stream);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvAttachSpeakerToStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
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

cell AMX_NATIVE_CALL Pawn::n_SvHasSpeakerInStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
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

cell AMX_NATIVE_CALL Pawn::n_SvDetachSpeakerFromStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return false;
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

cell AMX_NATIVE_CALL Pawn::n_SvDetachAllSpeakersFromStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvDetachAllSpeakersFromStream] : stream(%p)",
        stream
    );

    Pawn::pInterface->SvDetachAllSpeakersFromStream(stream);
    return NULL;
}

cell AMX_NATIVE_CALL Pawn::n_SvDeleteStream(AMX* amx, cell* params)
{
    if (!Pawn::pInterface) return NULL;
    if (params[0] != 1 * sizeof(cell)) return NULL;
    if (!Pawn::pInterface) return NULL;

    const auto stream = reinterpret_cast<Stream*>(params[1]);

    if (Pawn::debugStatus) Logger::Log(
        "[sv:dbg:pawn:SvDeleteStream] : stream(%p)",
        stream
    );

    Pawn::pInterface->SvDeleteStream(stream);
    return NULL;
}

PawnInterfacePtr Pawn::pInterface { nullptr };

bool Pawn::debugStatus { false };

std::vector<Pawn::AmxCallback> Pawn::callbacksOnPlayerActivationKeyPress;
std::vector<Pawn::AmxCallback> Pawn::callbacksOnPlayerActivationKeyRelease;
