/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <memory>

#include <system/types.hpp>

template <class Type, size_t Capacity>
struct Array {

    static_assert(Capacity != 0);

public:

    Array() noexcept
    {
        std::uninitialized_default_construct(Data(), Data() + Size());
    }

    ~Array() noexcept
    {
        std::destroy(Data(), Data() + Size());
    }

    Array(const Array& object) noexcept
    {
        std::uninitialized_copy(object.Data(), object.Data() + object.Size(), Data());
    }

    Array(Array&& object) noexcept
    {
        std::uninitialized_move(object.Data(), object.Data() + object.Size(), Data());
    }

    Array& operator=(const Array& object) noexcept
    {
        if (&object != this)
        {
            std::copy(object.Data(), object.Data() + object.Size(), Data());
        }

        return *this;
    }

    Array& operator=(Array&& object) noexcept
    {
        if (&object != this)
        {
            std::move(object.Data(), object.Data() + object.Size(), Data());
        }

        return *this;
    }

public:

    template <class Value>
    Array(const Value& value) noexcept
    {
        std::uninitialized_fill(Data(), Data() + Size(), value);
    }

public:

    const Type& operator[](const size_t index) const noexcept
    {
        assert(index < Size());

        return Data()[index];
    }

    Type& operator[](const size_t index) noexcept
    {
        assert(index < Size());

        return Data()[index];
    }

public:

    template <class Value>
    void Fill(const Value& value) noexcept
    {
        std::fill(Data(), Data() + Size(), value);
    }

public:

    static constexpr size_t Bytes() noexcept
    {
        return Capacity * sizeof(Type);
    }

public:

    const Type* First() const noexcept
    {
        return Data();
    }

    Type* First() noexcept
    {
        return Data();
    }

    const Type* Last() const noexcept
    {
        return Data() + Size() - 1;
    }

    Type* Last() noexcept
    {
        return Data() + Size() - 1;
    }

public:

    const Type* Begin() const noexcept
    {
        return Data();
    }

    Type* Begin() noexcept
    {
        return Data();
    }

    const Type* End() const noexcept
    {
        return nullptr;
    }

    Type* End() noexcept
    {
        return nullptr;
    }

public:

    const Type* Previous(Type* const iterator) const noexcept
    {
        return iterator != First() ? (iterator != End() ? iterator - 1 : Last()) : End();
    }

    Type* Previous(Type* const iterator) noexcept
    {
        return iterator != First() ? (iterator != End() ? iterator - 1 : Last()) : End();
    }

    const Type* Next(Type* const iterator) const noexcept
    {
        return iterator != Last() ? (iterator != End() ? iterator + 1 : First()) : End();
    }

    Type* Next(Type* const iterator) noexcept
    {
        return iterator != Last() ? (iterator != End() ? iterator + 1 : First()) : End();
    }

public:

    const Type* Data() const noexcept
    {
        return reinterpret_cast<const Type*>(_elements);
    }

    Type* Data() noexcept
    {
        return reinterpret_cast<Type*>(_elements);
    }

    static constexpr size_t Size() noexcept
    {
        return Capacity;
    }

private:

    alignas (alignof(Type))
    ubyte_t _elements[Capacity * sizeof(Type)];

};
