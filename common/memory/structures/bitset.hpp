/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <atomic>

#include <system/types.hpp>

template <size_t Capacity>
struct Bitset {

    static_assert(Capacity != 0);

public:

    Bitset() noexcept = default;
    ~Bitset() noexcept = default;

public:

    static constexpr size_t Size() noexcept
    {
        return Capacity;
    }

private:

    static constexpr size_t Chunks() noexcept
    {
        return (Capacity - 1) / Bits<uint_t> + 1;
    }

public:

    Bitset& operator&=(const Bitset& other) noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
            _chunks[chunk].fetch_and(other._chunks[chunk].load(std::memory_order_relaxed),
                std::memory_order_relaxed);

        return *this;
    }

    Bitset& operator|=(const Bitset& other) noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
            _chunks[chunk].fetch_or(other._chunks[chunk].load(std::memory_order_relaxed),
                std::memory_order_relaxed);

        return *this;
    }

    Bitset& operator^=(const Bitset& other) noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
            _chunks[chunk].fetch_xor(other._chunks[chunk].load(std::memory_order_relaxed),
                std::memory_order_relaxed);

        return *this;
    }

public:

    bool operator==(const Bitset& other) const noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
            if (_chunks[chunk].load(std::memory_order_relaxed) !=
          other._chunks[chunk].load(std::memory_order_relaxed))
                return false;

        return true;
    }

    bool operator!=(const Bitset& other) const noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
            if (_chunks[chunk].load(std::memory_order_relaxed) !=
          other._chunks[chunk].load(std::memory_order_relaxed))
                return true;

        return false;
    }

public:

    bool Zeros() const noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
            if (_chunks[chunk].load(std::memory_order_relaxed) != Zero<uint_t>)
                return false;

        return true;
    }

    bool Ones() const noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
            if (_chunks[chunk].load(std::memory_order_relaxed) != ~Zero<uint_t>)
                return false;

        return true;
    }

public:

    size_t FindFirstUnset() const noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
        {
            if (uint_t bits = _chunks[chunk].load(std::memory_order_relaxed); bits != ~Zero<uint_t>)
            {
                size_t index = chunk * Bits<uint_t>;
                while (bits & HighBit<uint_t>) bits <<= 1, ++index;
                return index;
            }
        }

        return None<size_t>;
    }

public:

    bool Set(const size_t index) noexcept
    {
        assert(index < Capacity);

        const size_t chunk = index / Bits<uint_t>;
        const uint_t mask = HighBit<uint_t> >> (index % Bits<uint_t>);
        const uint_t bits = _chunks[chunk].fetch_or(mask, std::memory_order_relaxed);

        return (bits & mask) == Zero<uint_t>;
    }

    bool Test(const size_t index) const noexcept
    {
        assert(index < Capacity);

        const size_t chunk = index / Bits<uint_t>;
        const uint_t mask = HighBit<uint_t> >> (index % Bits<uint_t>);
        const uint_t bits = _chunks[chunk].load(std::memory_order_relaxed);

        return (bits & mask) != Zero<uint_t>;
    }

    bool Reset(const size_t index) noexcept
    {
        assert(index < Capacity);

        const size_t chunk = index / Bits<uint_t>;
        const uint_t mask = HighBit<uint_t> >> (index % Bits<uint_t>);
        const uint_t bits = _chunks[chunk].fetch_and(~mask, std::memory_order_relaxed);

        return (bits & mask) != Zero<uint_t>;
    }

public:

    void Clear() noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
            _chunks[chunk].store(Zero<uint_t>, std::memory_order_relaxed);
    }

public:

    /*
        Callback = void(size_t index)
    */
    template <class Callback>
    void ForEach(Callback&& callback) const noexcept
    {
        for (size_t chunk = 0; chunk != Chunks(); ++chunk)
        {
            if (uint_t bits = _chunks[chunk].load(std::memory_order_relaxed); bits != Zero<uint_t>)
            {
                size_t index = chunk * Bits<uint_t>;
                do { if (bits & HighBit<uint_t>) callback(index); bits <<= 1, ++index; }
                while (bits != Zero<uint_t>);
            }
        }
    }

private:

    std::atomic<uint_t> _chunks[Chunks()] = {};

};
