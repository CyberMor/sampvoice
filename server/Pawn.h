/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <string>

#include <pawn/amx/amx.h>
#include <pawn/plugincommon.h>

#include "Stream.h"
#include "LocalStream.h"
#include "PointStream.h"

class PawnInterface {
public:

    virtual void    SvInit                         (uint32_t bitrate) = 0;

    virtual uint8_t SvGetVersion                   (uint16_t playerid) = 0;

    // --------------------------------------------------------------------------

    virtual bool    SvHasMicro                     (uint16_t playerid) = 0;

    virtual bool    SvStartRecord                  (uint16_t playerid) = 0;

    virtual bool    SvStopRecord                   (uint16_t playerid) = 0;

    // --------------------------------------------------------------------------

    virtual bool    SvAddKey                       (uint16_t playerid,
                                                    uint8_t keyid) = 0;

    virtual bool    SvHasKey                       (uint16_t playerid,
                                                    uint8_t keyid) = 0;

    virtual bool    SvRemoveKey                    (uint16_t playerid,
                                                    uint8_t keyid) = 0;

    virtual void    SvRemoveAllKeys                (uint16_t playerid) = 0;

    // --------------------------------------------------------------------------

    virtual bool    SvMutePlayerStatus             (uint16_t playerid) = 0;

    virtual void    SvMutePlayerEnable             (uint16_t playerid) = 0;

    virtual void    SvMutePlayerDisable            (uint16_t playerid) = 0;

    // --------------------------------------------------------------------------

    virtual Stream* SvCreateGStream                (uint32_t color,
                                                    const std::string& name) = 0;

    // --------------------------------------------------------------------------

    virtual Stream* SvCreateSLStreamAtPoint        (float distance,
                                                    float posx,
                                                    float posy,
                                                    float posz,
                                                    uint32_t color,
                                                    const std::string& name) = 0;

    virtual Stream* SvCreateSLStreamAtVehicle      (float distance,
                                                    uint16_t vehicleid,
                                                    uint32_t color,
                                                    const std::string& name) = 0;

    virtual Stream* SvCreateSLStreamAtPlayer       (float distance,
                                                    uint16_t playerid,
                                                    uint32_t color,
                                                    const std::string& name) = 0;

    virtual Stream* SvCreateSLStreamAtObject       (float distance,
                                                    uint16_t objectid,
                                                    uint32_t color,
                                                    const std::string& name) = 0;

    // --------------------------------------------------------------------------

    virtual Stream* SvCreateDLStreamAtPoint        (float distance,
                                                    uint32_t maxplayers,
                                                    float posx,
                                                    float posy,
                                                    float posz,
                                                    uint32_t color,
                                                    const std::string& name) = 0;

    virtual Stream* SvCreateDLStreamAtVehicle      (float distance,
                                                    uint32_t maxplayers,
                                                    uint16_t vehicleid,
                                                    uint32_t color,
                                                    const std::string& name) = 0;

    virtual Stream* SvCreateDLStreamAtPlayer       (float distance,
                                                    uint32_t maxplayers,
                                                    uint16_t playerid,
                                                    uint32_t color,
                                                    const std::string& name) = 0;

    virtual Stream* SvCreateDLStreamAtObject       (float distance,
                                                    uint32_t maxplayers,
                                                    uint16_t objectid,
                                                    uint32_t color,
                                                    const std::string& name) = 0;

    // --------------------------------------------------------------------------

    virtual void    SvUpdatePositionForLPStream    (PointStream* lpstream,
                                                    float posx,
                                                    float posy,
                                                    float posz) = 0;

    virtual void    SvUpdateDistanceForLStream     (LocalStream* lstream,
                                                    float distance) = 0;

    // --------------------------------------------------------------------------

    virtual bool    SvAttachListenerToStream       (Stream* stream,
                                                    uint16_t playerid) = 0;

    virtual bool    SvHasListenerInStream          (Stream* stream,
                                                    uint16_t playerid) = 0;

    virtual bool    SvDetachListenerFromStream     (Stream* stream,
                                                    uint16_t playerid) = 0;

    virtual void    SvDetachAllListenersFromStream (Stream* stream) = 0;

    // --------------------------------------------------------------------------

    virtual bool    SvAttachSpeakerToStream        (Stream* stream,
                                                    uint16_t playerid) = 0;

    virtual bool    SvHasSpeakerInStream           (Stream* stream,
                                                    uint16_t playerid) = 0;

    virtual bool    SvDetachSpeakerFromStream      (Stream* stream,
                                                    uint16_t playerid) = 0;

    virtual void    SvDetachAllSpeakersFromStream  (Stream* stream) = 0;

    // --------------------------------------------------------------------------

    virtual void    SvDeleteStream                 (Stream* stream) = 0;

};

using PawnInterfacePtr = std::unique_ptr<PawnInterface>;

class Pawn {
public:

    static bool Init(PawnInterfacePtr&& pInterface) noexcept;
    static void Free() noexcept;

    static void RegisterScript(AMX* amx);

    static void OnPlayerActivationKeyPressForAll(uint16_t playerid, uint8_t keyid) noexcept;
    static void OnPlayerActivationKeyReleaseForAll(uint16_t playerid, uint8_t keyid) noexcept;

private:

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

private:

    static PawnInterfacePtr pInterface;

    static bool debugStatus;

private:

    class AmxCallback {
    public:

        AmxCallback() = delete;
        AmxCallback(const AmxCallback&) noexcept = default;
        AmxCallback(AmxCallback&&) noexcept = default;
        AmxCallback& operator=(const AmxCallback&) noexcept = default;
        AmxCallback& operator=(AmxCallback&&) noexcept = default;

    public:

        explicit AmxCallback(AMX* amx, int index) noexcept
            : amx(amx), index(index) {}

        ~AmxCallback() noexcept = default;

    public:

        template<class... ARGS>
        cell Call(ARGS... args) const noexcept
        {
            cell returnValue { NULL };

            (amx_Push(this->amx, static_cast<cell>(args)), ...);
            amx_Exec(this->amx, &returnValue, this->index);

            return returnValue;
        }

    private:

        AMX* amx { nullptr };
        int index { -1 };

    };

    static std::vector<AmxCallback> callbacksOnPlayerActivationKeyPress;
    static std::vector<AmxCallback> callbacksOnPlayerActivationKeyRelease;

};
