/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <cstdlib>

#include <system/types.hpp>
#include <other/utils.hpp>

struct GrowingPoolAllocator {

    GrowingPoolAllocator() noexcept = default;
    ~GrowingPoolAllocator() noexcept
    {
        while (_chunk_begin != nullptr)
        {
            const auto previous_chunk = *reinterpret_cast<const adr_t*>
                (_chunk_begin + _chunk * _block);
            std::free(_chunk_begin);
            _chunk_begin = previous_chunk;
        }
    }

    GrowingPoolAllocator(const GrowingPoolAllocator&) = delete;
    GrowingPoolAllocator(GrowingPoolAllocator&& object) noexcept
        : _chunk_begin    { object._chunk_begin }
        , _chunk_iterator { object._chunk_iterator }
        , _chunk          { object._chunk }
        , _block          { object._block }
    {
        object._chunk_begin = nullptr;
    }

    GrowingPoolAllocator& operator=(const GrowingPoolAllocator&) = delete;
    GrowingPoolAllocator& operator=(GrowingPoolAllocator&& object) noexcept
    {
        if (&object != this)
        {
            while (_chunk_begin != nullptr)
            {
                const auto previous_chunk = *reinterpret_cast<const adr_t*>
                    (_chunk_begin + _chunk * _block);
                std::free(_chunk_begin);
                _chunk_begin = previous_chunk;
            }

            _chunk_begin    = object._chunk_begin;
            _chunk_iterator = object._chunk_iterator;
            _chunk          = object._chunk;
            _block          = object._block;

            object._chunk_begin = nullptr;
        }

        return *this;
    }

public:

    GrowingPoolAllocator(const size_t chunk, size_t block) noexcept
    {
        if (chunk != 0 && block != 0)
        {
            block = utils::align_up(block);
            _chunk_begin = static_cast<adr_t>(std::malloc(chunk * block + sizeof(adr_t)));
            if (_chunk_begin != nullptr)
            {
                *reinterpret_cast<adr_t*>(_chunk_begin + chunk * block) = nullptr;

                const adr_t last_block = _chunk_begin + (chunk - 1) * block;

                for (adr_t iterator = _chunk_begin; iterator != last_block; iterator += block)
                    *reinterpret_cast<adr_t*>(iterator) = iterator + block;

                *reinterpret_cast<adr_t*>(last_block) = nullptr;

                _chunk_iterator = _chunk_begin;

                _chunk = chunk;
                _block = block;
            }
        }
    }

public:

    bool Valid() const noexcept
    {
        return _chunk_begin != nullptr;
    }

    bool Invalid() const noexcept
    {
        return _chunk_begin == nullptr;
    }

public:

    bool Initialize(const size_t chunk, size_t block) noexcept
    {
        while (_chunk_begin != nullptr)
        {
            const auto previous_chunk = *reinterpret_cast<const adr_t*>
                (_chunk_begin + _chunk * _block);
            std::free(_chunk_begin);
            _chunk_begin = previous_chunk;
        }

        if (chunk != 0 && block != 0)
        {
            block = utils::align_up(block);
            _chunk_begin = static_cast<adr_t>(std::malloc(chunk * block + sizeof(adr_t)));
            if (_chunk_begin != nullptr)
            {
                *reinterpret_cast<adr_t*>(_chunk_begin + chunk * block) = nullptr;

                const adr_t last_block = _chunk_begin + (chunk - 1) * block;

                for (adr_t iterator = _chunk_begin; iterator != last_block; iterator += block)
                    *reinterpret_cast<adr_t*>(iterator) = iterator + block;

                *reinterpret_cast<adr_t*>(last_block) = nullptr;

                _chunk_iterator = _chunk_begin;

                _chunk = chunk;
                _block = block;
            }
        }

        return _chunk_begin != nullptr;
    }

    void Deinitialize() noexcept
    {
        while (_chunk_begin != nullptr)
        {
            const auto previous_chunk = *reinterpret_cast<const adr_t*>
                (_chunk_begin + _chunk * _block);
            std::free(_chunk_begin);
            _chunk_begin = previous_chunk;
        }
    }

public:

    ptr_t Allocate(const size_t bytes) noexcept
    {
        assert(_chunk_begin != nullptr);

        if (bytes > _block) return nullptr;

        if (_chunk_iterator == nullptr)
        {
            if (const auto new_chunk = static_cast<adr_t>(std::malloc(_chunk * _block + sizeof(adr_t)));
                           new_chunk != nullptr)
            {
                *reinterpret_cast<adr_t*>(new_chunk + _chunk * _block) = _chunk_begin;

                const adr_t last_block = new_chunk + (_chunk - 1) * _block;

                for (adr_t iterator = new_chunk; iterator != last_block; iterator += _block)
                    *reinterpret_cast<adr_t*>(iterator) = iterator + _block;

                *reinterpret_cast<adr_t*>(last_block) = nullptr;

                _chunk_begin    = new_chunk;
                _chunk_iterator = new_chunk;
            }
        }

        const ptr_t pointer = _chunk_iterator;
        if (pointer != nullptr)
        {
            _chunk_iterator = *static_cast<const adr_t*>(pointer);
        }

        return pointer;
    }

    void Deallocate(const ptr_t pointer) noexcept
    {
        assert(_chunk_begin != nullptr);

        if (pointer != nullptr)
        {
            *static_cast<adr_t*>(pointer) = _chunk_iterator;
            _chunk_iterator = static_cast<adr_t>(pointer);
        }
    }

public:

    void Clear() noexcept
    {
        assert(_chunk_begin != nullptr);

        adr_t next_chunk = nullptr; while (true)
        {
            const adr_t last_block = _chunk_begin + (_chunk - 1) * _block;

            for (adr_t iterator = _chunk_begin; iterator != last_block; iterator += _block)
                *reinterpret_cast<adr_t*>(iterator) = iterator + _block;

            *reinterpret_cast<adr_t*>(last_block) = next_chunk;

            const auto previous_chunk = *reinterpret_cast<const adr_t*>
                (_chunk_begin + _chunk * _block);
            if (previous_chunk == nullptr) break;

            next_chunk = _chunk_begin;

            _chunk_begin = previous_chunk;
        }

        _chunk_iterator = _chunk_begin;
    }

private:

    adr_t _chunk_begin    = nullptr;
    adr_t _chunk_iterator = nullptr;

    size_t _chunk = 0;
    size_t _block = 0;

};
