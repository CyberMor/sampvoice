/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cstddef>
#include <algorithm>

#include <system/types.hpp>
#include <other/utils.hpp>

#include "system_allocator.hpp"

template <class Allocator>
struct ObjectAllocator {

    ObjectAllocator() noexcept = default;
    ~ObjectAllocator() noexcept = default;

public:

    template <class... Arguments>
    ObjectAllocator(Arguments&&... arguments) noexcept
        : _allocator { std::forward<Arguments>(arguments)... }
    {}

public:

    bool Valid() const noexcept
    {
        return _allocator.Valid();
    }

    bool Invalid() const noexcept
    {
        return _allocator.Invalid();
    }

public:

    template <class... Arguments>
    bool Initialize(Arguments&&... arguments) noexcept
    {
        return _allocator.Initialize(std::forward<Arguments>(arguments)...);
    }

    void Deinitialize() noexcept
    {
        _allocator.Deinitialize();
    }

public:

    template <class Object, class... Arguments>
    Object* Construct(Arguments&&... arguments) noexcept
    {
        if constexpr (std::is_same_v<Allocator, SystemAllocator>)
        {
            return new (std::nothrow) Object { std::forward<Arguments>(arguments)... };
        }
        else
        {
            const auto pointer = static_cast<Object*>(_allocator.Allocate(sizeof(Object)));
            if (pointer != nullptr) new (pointer) Object { std::forward<Arguments>(arguments)... };
            return pointer;
        }
    }

    template <class Object>
    void Destroy(Object* const pointer) noexcept
    {
        if constexpr (std::is_same_v<Allocator, SystemAllocator>)
        {
            delete pointer;
        }
        else
        {
            if (pointer != nullptr) pointer->~Object();
            _allocator.Deallocate(pointer);
        }
    }

private:

    static constexpr size_t kArrayInfoSize = utils::align_up(sizeof(size_t));

public:

    template <class Object>
    Object* ConstructArray(const size_t count) noexcept
    {
        if (count == 0) return nullptr;

        if constexpr (std::is_same_v<Allocator, SystemAllocator>)
        {
            return new (std::nothrow) Object [count] ();
        }
        else
        {
            auto pointer = static_cast<adr_t>(_allocator.Allocate(kArrayInfoSize + count * sizeof(Object)));
            if (pointer != nullptr)
            {
                *reinterpret_cast<size_t*>(pointer) = count;
                pointer += kArrayInfoSize;
                for (size_t i = 0; i != count; ++i)
                    new (pointer + i * sizeof(Object)) Object {};
            }

            return reinterpret_cast<Object*>(pointer);
        }
    }

    template <class Object>
    void DestroyArray(Object* const pointer) noexcept
    {
        if constexpr (std::is_same_v<Allocator, SystemAllocator>)
        {
            delete[] pointer;
        }
        else
        {
            auto iterator = reinterpret_cast<adr_t>(pointer);
            if (iterator != nullptr)
            {
                iterator -= kArrayInfoSize;
                const size_t count = *reinterpret_cast<const size_t*>(iterator);
                for (size_t i = 0; i != count; ++i) pointer[i].~Object();
            }

            _allocator.Deallocate(iterator);
        }
    }

public:

    void Clear() noexcept
    {
        _allocator.Clear();
    }

private:

    Allocator _allocator;

};
