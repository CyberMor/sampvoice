/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#include <system/types.hpp>
#include <execution/spinlock.hpp>
#include <memory/types.hpp>
#include <other/utils.hpp>

#include "array.hpp"

template <class Type, size_t Capacity>
struct Pool {

    static_assert(Capacity != 0);

public:

    Pool() noexcept = default;

private:

    struct alignas(CACHE_LINE) Slot
    {
        mutable Spinlock lock;
        size_t references = 0;
        Type* value = nullptr;
    };

public:

    ~Pool() noexcept
    {
        ForwardForEach(slot, _slots)
        {
            if (slot->references != 0)
            {
                delete slot->value;
            }
        }
    }

    Pool(const Pool&) = delete;
    Pool(Pool&&) = delete;
    Pool& operator=(const Pool&) = delete;
    Pool& operator=(Pool&&) = delete;

public:

    const Type& operator[](const size_t index) const noexcept
    {
        assert(_slots[index].references != 0);

        return *_slots[index].value;
    }

    Type& operator[](const size_t index) noexcept
    {
        assert(_slots[index].references != 0);

        return *_slots[index].value;
    }

public:

    void Deinitialize() noexcept
    {
        ForwardForEach(slot, _slots)
        {
            if (slot->references != 0)
            {
                delete slot->value;

                slot->references = 0;
            }
        }
    }

public:

    template <size_t References = 1, class... Arguments>
    size_t Emplace(Arguments&&... arguments) noexcept
    {
        bool selected;
        bool emplaced;

        ForwardForEach(slot, _slots)
        {
            slot->lock.Lock();

            if (selected = (slot->references == 0))
            {
                slot->value = new (std::nothrow) Type { std::forward<Arguments>(arguments)... };
                if (emplaced = (slot->value != nullptr)) slot->references = References;
            }

            slot->lock.Unlock();

            if (selected) return emplaced ? slot - _slots.Begin() : None<size_t>;
        }

        return None<size_t>;
    }

    template <size_t References = 1, class... Arguments>
    bool EmplaceAt(const size_t index, const bool block, Arguments&&... arguments) noexcept
    {
        bool emplaced = false;

        _slots[index].lock.Lock();

        if (block) while (_slots[index].references & HighBit<size_t>)
        {
            _slots[index].lock.Unlock();
            std::this_thread::yield();
            _slots[index].lock.Lock();
        }

        if (_slots[index].references == 0)
        {
            _slots[index].value = new (std::nothrow) Type { std::forward<Arguments>(arguments)... };
            if (emplaced = (_slots[index].value != nullptr)) _slots[index].references = References;
        }

        _slots[index].lock.Unlock();

        return emplaced;
    }

    template <size_t References = 1>
    void Remove(const size_t index, const bool block) noexcept
    {
        _slots[index].lock.Lock();

        if ((_slots[index].references & HighBit<size_t>) == 0)
        {
            assert(_slots[index].references >= References);

            _slots[index].references -= References;
            if (_slots[index].references == 0) delete _slots[index].value;
            else _slots[index].references |= HighBit<size_t>;

            if (block) while (_slots[index].references != 0)
            {
                _slots[index].lock.Unlock();
                std::this_thread::yield();
                _slots[index].lock.Lock();
            }
        }

        _slots[index].lock.Unlock();
    }

public:

    template <size_t References = 1>
    bool Acquire(const size_t index) noexcept
    {
        _slots[index].lock.Lock();

        const bool acquired = _slots[index].references != 0 && (_slots[index].references & HighBit<size_t>) == 0;
        if (acquired) _slots[index].references += References;

        _slots[index].lock.Unlock();

        return acquired;
    }

    template <size_t References = 1>
    void Release(const size_t index) noexcept
    {
        _slots[index].lock.Lock();

        assert((_slots[index].references & ~HighBit<size_t>) >= References);

        _slots[index].references -= References;
        if ((_slots[index].references & ~HighBit<size_t>) == 0)
        {
            delete _slots[index].value;
            _slots[index].references = 0;
        }

        _slots[index].lock.Unlock();
    }

public:

    void Clear() noexcept
    {
        ForwardForEach(slot, _slots)
        {
            slot->lock.Lock();

            if (slot->references != 0 && (slot->references & HighBit<size_t>) == 0)
            {
                slot->references -= 1;
                if (slot->references == 0) delete slot->value;
                else slot->references |= HighBit<size_t>;
            }

            slot->lock.Unlock();
        }
    }

public:

    static constexpr size_t Size() noexcept
    {
        return Capacity;
    }

private:

    Array<Slot, Capacity> _slots;

};
