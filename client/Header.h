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

#define GetTimestamp() std::chrono::duration_cast<std::chrono::milliseconds> \
                      (std::chrono::system_clock::now().time_since_epoch()).count()
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

    constexpr BYTE  kVersion = 10;
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
            updateLPStreamDistance,
            updateLPStreamPosition,
            deleteStream,

            pressKey,
            releaseKey
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

    struct ConnectPacket
    {
        DWORD signature;
        BYTE version;
        BYTE micro;
    };

    struct ServerInfoPacket
    {
        DWORD serverKey;
        WORD serverPort;
    };

    struct PluginInitPacket
    {
        DWORD bitrate;
        BYTE mute;
    };

    struct AddKeyPacket
    {
        BYTE keyId;
    };

    struct RemoveKeyPacket
    {
        BYTE keyId;
    };

    struct CreateGStreamPacket
    {
        DWORD stream;
        DWORD color;
        char name[];
    };

    struct CreateLPStreamPacket
    {
        DWORD stream;
        float distance;
        CVector position;
        DWORD color;
        char name[];
    };

    struct CreateLStreamAtPacket
    {
        DWORD stream;
        float distance;
        WORD target;
        DWORD color;
        char name[];
    };

    struct UpdateLPStreamDistancePacket
    {
        DWORD stream;
        float distance;
    };

    struct UpdateLPStreamPositionPacket
    {
        DWORD stream;
        CVector position;
    };

    struct DeleteStreamPacket
    {
        DWORD stream;
    };

    struct PressKeyPacket
    {
        BYTE keyId;
    };

    struct ReleaseKeyPacket
    {
        BYTE keyId;
    };

#pragma pack(pop)
}
