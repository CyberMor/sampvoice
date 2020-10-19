/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <chrono>
#include <thread>

#include <game/CVector.h>

#define SleepForMilliseconds(mscount) std::this_thread::sleep_for(std::chrono::milliseconds(mscount))

namespace SV
{
    // Strings
    // --------------------------------------------

    constexpr auto kLogFileName = "svlog.txt";
    constexpr auto kConfigFileName = "svconfig.bin";

    // Constants
    // --------------------------------------------

    constexpr WORD  kNonePlayer = 0xffff;

    constexpr BYTE  kVersion = 11;
    constexpr DWORD kSignature = 0xDeadBeef;

    constexpr DWORD kAudioUpdateThreads = 4;
    constexpr DWORD kAudioUpdatePeriod = 10;

    constexpr DWORD kVoiceRate = 100;
    constexpr DWORD kFrequency = 48000;
    constexpr DWORD kFrameSizeInSamples = (kFrequency / 1000) * kVoiceRate;
    constexpr DWORD kFrameSizeInBytes = kFrameSizeInSamples * sizeof(WORD);

    constexpr DWORD kChannelPreBufferFramesCount = 3;
    constexpr DWORD kChannelPreBufferSizeInMs = kChannelPreBufferFramesCount * kVoiceRate;
    constexpr DWORD kChannelBufferSizeInMs = 3 * kChannelPreBufferSizeInMs;

    struct ControlPacketType
    {
        enum : BYTE
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
        enum : BYTE
        {
            keepAlive,
            voicePacket
        };
    };

    // Packets
    // --------------------------------------------

#pragma pack(push, 1)

    // v3.0
    // -----------------------------------

    struct ConnectPacket
    {
        UINT32 signature;
        UINT8 version;
        UINT8 micro;
    };

    struct ServerInfoPacket
    {
        UINT32 serverKey;
        UINT16 serverPort;
    };

    struct PluginInitPacket
    {
        UINT32 bitrate;
        UINT8 mute;
    };

    struct AddKeyPacket
    {
        UINT8 keyId;
    };

    struct RemoveKeyPacket
    {
        UINT8 keyId;
    };

    struct CreateGStreamPacket
    {
        UINT32 stream;
        UINT32 color;
        CHAR name[];
    };

    struct CreateLPStreamPacket
    {
        UINT32 stream;
        FLOAT distance;
        CVector position;
        UINT32 color;
        CHAR name[];
    };

    struct CreateLStreamAtPacket
    {
        UINT32 stream;
        FLOAT distance;
        UINT32 target;
        UINT32 color;
        CHAR name[];
    };

    struct UpdateLStreamDistancePacket
    {
        UINT32 stream;
        FLOAT distance;
    };

    struct UpdateLPStreamPositionPacket
    {
        UINT32 stream;
        CVector position;
    };

    struct DeleteStreamPacket
    {
        UINT32 stream;
    };

    struct PressKeyPacket
    {
        UINT8 keyId;
    };

    struct ReleaseKeyPacket
    {
        UINT8 keyId;
    };

    // v3.1 added
    // -----------------------------------

    struct SetStreamParameterPacket
    {
        UINT32 stream;
        UINT32 parameter;
        FLOAT value;
    };

    struct SlideStreamParameterPacket
    {
        UINT32 stream;
        UINT32 parameter;
        FLOAT startvalue;
        FLOAT endvalue;
        UINT32 time;
    };

    struct CreateEffectPacket
    {
        UINT32 stream;
        UINT32 effect;
        UINT32 number;
        INT32 priority;
        UINT8 params[];
    };

    struct DeleteEffectPacket
    {
        UINT32 stream;
        UINT32 effect;
    };

#pragma pack(pop)
}
