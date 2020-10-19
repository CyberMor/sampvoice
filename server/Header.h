/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <chrono>
#include <cstdint>
#include <thread>

#include <ysf/utils/cvector.h>

#define SleepForMilliseconds(mscount) (std::this_thread::sleep_for(std::chrono::milliseconds(mscount)))

namespace SV
{
    // Constants
    // --------------------------------------------

    constexpr const char* kLogFileName       = "svlog.txt";
    constexpr uint32_t    kFrequency         = 48000;
    constexpr uint16_t    kNonePlayer        = 0xffff;
    constexpr uint32_t    kVoiceThreadsCount = 8;
    constexpr uint32_t    kDefaultBitrate    = 24000;
    constexpr uint8_t     kVersion           = 11;
    constexpr uint32_t    kSignature         = 0xDeadBeef;
    constexpr const char* kSignaturePattern  = "\xef\xbe\xad\xde";
    constexpr const char* kSignatureMask     = "xxxx";

    // Types
    // --------------------------------------------

    struct ControlPacketType
    {
        enum : uint8_t
        {
            // v3.0
            // ---------------------

            serverInfo,
            pluginInit,

            muteEnable,
            muteDisable,
            startRecord,
            stopRecord,
            addKey,
            removeKey,
            removeAllKeys,
            createGStream,
            createLPStream,
            createLStreamAtVehicle,
            createLStreamAtPlayer,
            createLStreamAtObject,
            updateLStreamDistance,
            updateLPStreamPosition,
            deleteStream,

            pressKey,
            releaseKey,

            // v3.1 added
            // ---------------------

            setStreamParameter,
            slideStreamParameter,
            createEffect,
            deleteEffect
        };
    };

    struct VoicePacketType
    {
        enum : uint8_t
        {
            keepAlive,
            voicePacket
        };
    };

    struct ParameterType
    {
        enum : uint8_t
        {
            frequency = 1,
            volume    = 2,
            panning   = 3,
            eaxmix    = 4,
            src       = 8
        };
    };

    struct EffectType
    {
        enum : uint8_t
        {
            chorus,
            compressor,
            distortion,
            echo,
            flanger,
            gargle,
            i3dl2reverb,
            parameq,
            reverb
        };
    };

    // Packets
    // --------------------------------------------

#pragma pack(push, 1)

    // v3.0
    // -----------------------------------

    struct ConnectPacket
    {
        uint32_t signature;
        uint8_t version;
        uint8_t micro;
    };

    struct ServerInfoPacket
    {
        uint32_t serverKey;
        uint16_t serverPort;
    };

    struct PluginInitPacket
    {
        uint32_t bitrate;
        uint8_t mute;
    };

    struct AddKeyPacket
    {
        uint8_t keyId;
    };

    struct RemoveKeyPacket
    {
        uint8_t keyId;
    };

    struct CreateGStreamPacket
    {
        uint32_t stream;
        uint32_t color;
        char name[];
    };

    struct CreateLPStreamPacket
    {
        uint32_t stream;
        float distance;
        CVector position;
        uint32_t color;
        char name[];
    };

    struct CreateLStreamAtPacket
    {
        uint32_t stream;
        float distance;
        uint32_t target;
        uint32_t color;
        char name[];
    };

    struct UpdateLStreamDistancePacket
    {
        uint32_t stream;
        float distance;
    };

    struct UpdateLPStreamPositionPacket
    {
        uint32_t stream;
        CVector position;
    };

    struct DeleteStreamPacket
    {
        uint32_t stream;
    };

    struct PressKeyPacket
    {
        uint8_t keyId;
    };

    struct ReleaseKeyPacket
    {
        uint8_t keyId;
    };

    // v3.1 added
    // -----------------------------------

    struct SetStreamParameterPacket
    {
        uint32_t stream;
        uint32_t parameter;
        float value;
    };

    struct SlideStreamParameterPacket
    {
        uint32_t stream;
        uint32_t parameter;
        float startvalue;
        float endvalue;
        uint32_t time;
    };

    struct CreateEffectPacket
    {
        uint32_t stream;
        uint32_t effect;
        uint32_t number;
        int32_t priority;
        uint8_t params[];
    };

    struct DeleteEffectPacket
    {
        uint32_t stream;
        uint32_t effect;
    };

#pragma pack(pop)
}
