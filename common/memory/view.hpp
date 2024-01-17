/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>

#include <system/types.hpp>
#include <other/utils.hpp>

template <class DataType, class SizeType = size_t>
struct DataView {

    constexpr DataView() noexcept = default;
    ~DataView() noexcept = default;
    constexpr DataView(const DataView&) noexcept = default;
    constexpr DataView(DataView&&) noexcept = default;
    constexpr DataView& operator=(const DataView&) noexcept = default;
    constexpr DataView& operator=(DataView&&) noexcept = default;

public:

    constexpr DataView(DataType* const data, const SizeType size) noexcept
        : data { data }
        , size { size }
    {
        assert(size >= 0);
        assert(size == 0 || data != nullptr);
    }

public:

    template <class Buffer, typename = std::enable_if_t<std::is_pod_v<std::remove_reference_t<Buffer>>>>
    static constexpr DataView FromBuffer(Buffer&& buffer) noexcept
    {
        static_assert(sizeof(Buffer) % sizeof(DataType) == 0);

        return { reinterpret_cast<DataType*>(&buffer), sizeof(Buffer) / sizeof(DataType) };
    }

    static constexpr DataView FromString(DataType* const string, const DataType terminator = {}) noexcept
    {
        assert(string != nullptr);

        return { string, utils::string::length(string, terminator) };
    }

    template <class MemoryDataType, class MemorySizeType>
    static constexpr DataView FromMemory(const DataView<MemoryDataType, MemorySizeType>& memory) noexcept
    {
        assert(memory.Bytes() % sizeof(DataType) == 0);

        return { reinterpret_cast<DataType*> (memory.Data()),
                      static_cast<SizeType>  (memory.Bytes() / sizeof(DataType)) };
    }

public:

    constexpr bool operator<(const DataView& view) const noexcept
    {
        return data < view.data || data == view.data && size < view.size;
    }

    constexpr bool operator>(const DataView& view) const noexcept
    {
        return data > view.data || data == view.data && size > view.size;
    }

    constexpr bool operator<=(const DataView& view) const noexcept
    {
        return data < view.data || data == view.data && size <= view.size;
    }

    constexpr bool operator>=(const DataView& view) const noexcept
    {
        return data > view.data || data == view.data && size >= view.size;
    }

    constexpr bool operator==(const DataView& view) const noexcept
    {
        return data == view.data && size == view.size;
    }

    constexpr bool operator!=(const DataView& view) const noexcept
    {
        return data != view.data || size != view.size;
    }

public:

    constexpr const DataType& operator[](const SizeType index) const noexcept
    {
        assert(data != nullptr);

        assert(index >= 0 && index < size);

        return data[index];
    }

    DataType& operator[](const SizeType index) noexcept
    {
        assert(data != nullptr);

        assert(index >= 0 && index < size);

        return data[index];
    }

public:

    constexpr bool Valid() const noexcept
    {
        return data != nullptr;
    }

    constexpr bool Invalid() const noexcept
    {
        return data == nullptr;
    }

public:

    constexpr bool Contains(const DataView& view) const noexcept
    {
        return view.data >= data && view.data + view.size <= data + size;
    }

    constexpr bool Intersect(const DataView& view) const noexcept
    {
        return view.data <= data + size && view.data + view.size >= data;
    }

public:

    constexpr DataView SubLeft(SizeType offset) const noexcept
    {
        assert(offset >= 0);

        if (offset > size) offset = size;

        return { data + offset, size - offset };
    }

    constexpr DataView SubLeft(SizeType offset, SizeType length) const noexcept
    {
        assert(offset >= 0 && length >= 0);

        if (offset > size) offset = size;

        if (const SizeType available = size - offset; length > available)
            length = available;

        return { data + offset, length };
    }

public:

    constexpr DataView SubRight(SizeType offset) const noexcept
    {
        assert(offset >= 0);

        if (offset > size) offset = size;

        return { data, size - offset };
    }

    constexpr DataView SubRight(SizeType offset, SizeType length) const noexcept
    {
        assert(offset >= 0 && length >= 0);

        if (offset > size) offset = size;

        if (const SizeType available = size - offset; length > available)
            length = available;

        return { (data + size) - (offset + length), length };
    }

public:

    void FillWithZeros() const noexcept
    {
        assert(size == 0 || data != nullptr);

        std::memset(Data(), { 0x00 }, Bytes());
    }

    void FillWithOnes() const noexcept
    {
        assert(size == 0 || data != nullptr);

        std::memset(Data(), { 0xFF }, Bytes());
    }

public:

    constexpr SizeType Bytes() const noexcept
    {
        return size * sizeof(DataType);
    }

public:

    constexpr DataType* First() const noexcept
    {
        assert(data != nullptr);
        assert(size != 0);

        return Data();
    }

    constexpr DataType* Last() const noexcept
    {
        assert(data != nullptr);
        assert(size != 0);

        return Data() + Size() - 1;
    }

public:

    constexpr DataType* Begin() const noexcept
    {
        return Data();
    }

    static constexpr DataType* End() noexcept
    {
        return nullptr;
    }

public:

    constexpr DataType* Previous(DataType* const iterator) const noexcept
    {
        return iterator != First() ? (iterator != End() ? iterator - 1 : Last()) : End();
    }

    constexpr DataType* Next(DataType* const iterator) const noexcept
    {
        return iterator != Last() ? (iterator != End() ? iterator + 1 : First()) : End();
    }

public:

    constexpr DataType* Data() const noexcept
    {
        return data;
    }

    constexpr SizeType Size() const noexcept
    {
        return size;
    }

public:

    DataType* data = nullptr;
    SizeType  size = 0;

};
