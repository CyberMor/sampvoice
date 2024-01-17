/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <mutex>
#include <functional>

#include <system/types.hpp>
#include <memory/view.hpp>

#include <audio/opus.h>

#include "voice.hpp"

// Constants
// --------------------------------------------

constexpr size_t kSourcePlayingBuffer = 2;
constexpr size_t kSourceLoadingBuffer = 1;

constexpr size_t kSourceMutableBuffer = kSourcePlayingBuffer + kSourceLoadingBuffer;

constexpr size_t kSourceMaximumJump   = kSourceMutableBuffer - 1;
constexpr size_t kSourceKeepingBuffer = kSourceMaximumJump   - 1;

constexpr size_t kSourceChunk         = kSourceKeepingBuffer + kSourceMutableBuffer;

// Source
// --------------------------------------------

struct Source {

    Source() noexcept = default;
    ~Source() noexcept
    {
        opus_decoder_destroy(_decoder);
    }

    Source(const Source&) = delete;
    Source(Source&&) = delete;
    Source& operator=(const Source&) = delete;
    Source& operator=(Source&&) = delete;

public:

    bool Initialize() noexcept
    {
        opus_decoder_destroy(_decoder);

        _error = -1;

        _decoder = opus_decoder_create(kFrequency, 1, &_error);

        for (size_t stream = 0; stream != kMaxStreams; ++stream)
        {
            _playing[stream] = 0;
        }

        _speakers = 0;

        for (size_t block = 0; block != kSourceChunk; ++block)
        {
            _streams[block][0] = None<uword_t>;
        }

        _initial = 0;
        _current = 0;

        return _decoder != nullptr;
    }

    void Deinitialize() noexcept
    {
        opus_decoder_destroy(_decoder);

        _decoder = nullptr;
    }

public:

    int Error() const noexcept
    {
        return _error;
    }

public:

    void PushPacket(const udword_t packet, const uword_t* const streams,
        const DataView<const ubyte_t, opus_int32>& content) noexcept
    {
        if (packet > _current + kSourceKeepingBuffer || packet + (kSourceMutableBuffer - 1) < _current)
            Reset(packet);

        if (packet < _current)
        {
            const std::lock_guard lock { _mutex };

            opus_decode_float(_decoder, content.Data(), content.Bytes(),
                ReplaceFrame(streams, packet), kFrameSamples, 0);

            return;
        }
        if (packet > _current)
        {
            uword_t data[kMaximumStreamsInPacket + 1];
             size_t size = 0;

            if (_current != _initial)
            {
                for (const uword_t* iterator = streams; *iterator != None<uword_t>; ++iterator)
                {
                    if (utils::string::has(_streams[(_current - 1) % kSourceChunk], *iterator, None<uword_t>))
                        data[size++] = *iterator;
                }
            }

            data[size] = None<uword_t>;

            const std::lock_guard lock { _mutex };

            while (_current != packet - 1) opus_decode_float(_decoder, nullptr, 0,
                EmplaceFrame(data), kFrameSamples, 0);

            opus_decode_float(_decoder, content.Data(), content.Bytes(),
                EmplaceFrame(data), kFrameSamples, 1);
        }

        const std::lock_guard lock { _mutex };

        opus_decode_float(_decoder, content.Data(), content.Bytes(),
            EmplaceFrame(streams), kFrameSamples, 0);
    }

    bool PopFrame(udword_t& packet, const uword_t stream, float* const buffer) noexcept
    {
        const std::lock_guard lock { _mutex };

        if (packet > _current || packet + kSourceChunk < _current)
            packet = _current - kSourceMutableBuffer;

        while (packet != _current && !utils::string::has(_streams[packet % kSourceChunk], stream, None<uword_t>))
            ++packet;

        if (packet == _current) return false;

        std::memcpy(buffer, _buffers[packet % kSourceChunk], kFrameBytes);

        ++packet;

        return true;
    }

public:

    void Reset(const udword_t packet = 0) noexcept
    {
        if (_decoder != nullptr)
        {
            opus_decoder_ctl(_decoder, OPUS_RESET_STATE);
        }

        for (size_t stream = 0; stream != kMaxStreams; ++stream)
        {
            if (_playing[stream] != 0)
            {
                if (OnStopStream != nullptr)
                    OnStopStream(stream);

                _playing[stream] = 0;
            }
        }

        _speakers = 0;

        const std::lock_guard lock { _mutex };

        for (size_t block = 0; block != kSourceChunk; ++block)
        {
            _streams[block][0] = None<uword_t>;
        }

        _initial = packet;
        _current = packet;
    }

public:

    void Tick(const Time delta) noexcept
    {
        if (_speakers != 0)
        {
            for (size_t stream = 0; stream != kMaxStreams; ++stream)
            {
                if (_playing[stream] != 0)
                {
                    if ((_playing[stream] -= delta) <= 0)
                    {
                        assert(_speakers != 0);

                        if (OnStopStream != nullptr)
                            OnStopStream(stream);

                        _playing[stream] = 0;

                        --_speakers;
                    }
                }
            }
        }
    }

private:

    float* EmplaceFrame(const uword_t* const streams) noexcept
    {
        const udword_t packet = _current++;

        utils::string::copy(streams, _streams[packet % kSourceChunk],
            std::size(_streams[packet % kSourceChunk]), None<uword_t>, None<uword_t>);

        if ((packet + 1) - _initial >= kSourceMutableBuffer)
        {
            const uword_t* iterators[kSourcePlayingBuffer];

            for (udword_t i = packet - kSourceLoadingBuffer, j = 0; i != packet - kSourceMutableBuffer; --i, ++j)
                iterators[j] = _streams[i % kSourceChunk];

            for (iterators[0]; *iterators[0] != None<uword_t>; ++iterators[0])
            {
                if (_playing[*iterators[0]] != 0)
                    _playing[*iterators[0]] += kInterval;
                else
                {
                    bool require_playing = true;

                    for (size_t i = 1; i != kSourcePlayingBuffer; ++i)
                    {
                        while (*iterators[i] < *iterators[0]) ++iterators[i];
                        require_playing &= (*iterators[i] == *iterators[0]);
                    }

                    if (require_playing)
                    {
                        if (OnPlayStream != nullptr)
                            OnPlayStream(*iterators[0], (packet + 1) - kSourceMutableBuffer);

                        _playing[*iterators[0]] = kSourcePlayingBuffer * kInterval;

                        ++_speakers;
                    }
                }
            }
        }

        return _buffers[packet % kSourceChunk];
    }

    float* ReplaceFrame(const uword_t* const streams, const udword_t packet) noexcept
    {
        if (_current - packet > kSourceLoadingBuffer)
        {
            constexpr uword_t kSkipValue = ~static_cast<uword_t>(1);

            for (const uword_t* iterator = streams; *iterator != None<uword_t>; ++iterator)
            {
                if (const size_t position = utils::string::find(_streams[packet % kSourceChunk],
                    *iterator, None<uword_t>); position == None<size_t>)
                {
                    if (_playing[*iterator] != 0)
                        _playing[*iterator] += kInterval;
                }
                else
                {
                    _streams[packet % kSourceChunk][position] = kSkipValue;
                }
            }

            for (const uword_t* iterator = _streams[packet % kSourceChunk];
                *iterator != None<uword_t>; ++iterator)
            {
                if (*iterator != kSkipValue)
                {
                    if (_playing[*iterator] != 0)
                        _playing[*iterator] -= kInterval;
                }
            }
        }

        utils::string::copy(streams, _streams[packet % kSourceChunk],
            std::size(_streams[packet % kSourceChunk]), None<uword_t>, None<uword_t>);

        return _buffers[packet % kSourceChunk];
    }

private:

    OpusDecoder* _decoder = nullptr;
    int          _error;

    Time         _playing[kMaxStreams];

    uword_t      _streams[kSourceChunk][kMaximumStreamsInPacket + 1];
    float        _buffers[kSourceChunk][kFrameSamples];

    udword_t     _initial;
    udword_t     _current;

private:

    mutable std::mutex _mutex;

public:

    std::function<void(uword_t, udword_t)> OnPlayStream;
    std::function<void(uword_t)>           OnStopStream;

private:

    size_t _speakers = 0;

};
