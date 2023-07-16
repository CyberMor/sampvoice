/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstddef>

#include <system/types.hpp>
#include <other/utils.hpp>

template <size_t Chunk, size_t Block>
struct FixedPoolAllocator {

    static_assert(Chunk != 0 && Block != 0);

public:

    FixedPoolAllocator() noexcept { Clear(); }
    ~FixedPoolAllocator() noexcept = default;
    FixedPoolAllocator(const FixedPoolAllocator&) = delete;
    FixedPoolAllocator(FixedPoolAllocator&&) = delete;
    FixedPoolAllocator& operator=(const FixedPoolAllocator&) = delete;
    FixedPoolAllocator& operator=(FixedPoolAllocator&&) = delete;

public:

    bool Valid() const noexcept
    {
        return true;
    }

    bool Invalid() const noexcept
    {
        return false;
    }

public:

    bool Initialize() noexcept
    {
        return Clear(), true;
    }

    void Deinitialize() noexcept
    {}

public:

    ptr_t Allocate(const size_t bytes) noexcept
    {
        if (bytes > utils::align_up(Block))
            return nullptr;

        const ptr_t pointer = _chunk_iterator;
        if (pointer != nullptr)
        {
            _chunk_iterator = *static_cast<const adr_t*>(pointer);
        }

        return pointer;
    }

    void Deallocate(const ptr_t pointer) noexcept
    {
        if (pointer != nullptr)
        {
            *static_cast<adr_t*>(pointer) = _chunk_iterator;
            _chunk_iterator = static_cast<adr_t>(pointer);
        }
    }

public:

    void Clear() noexcept
    {
        const adr_t last_block = _chunk_begin + (Chunk - 1) * utils::align_up(Block);

        for (adr_t iterator = _chunk_begin; iterator != last_block; iterator += utils::align_up(Block))
            *reinterpret_cast<adr_t*>(iterator) = iterator + utils::align_up(Block);

        *reinterpret_cast<adr_t*>(last_block) = nullptr;

        _chunk_iterator = _chunk_begin;
    }

private:

    alignas (alignof(std::max_align_t))
    ubyte_t _chunk_begin[Chunk * utils::align_up(Block)];
      adr_t _chunk_iterator;

};
