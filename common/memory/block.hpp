/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <cstring>

#include <system/types.hpp>
#include <other/utils.hpp>

#include "view.hpp"

template <class DataType, class SizeType = size_t>
struct DataBlock {

    static_assert(!std::is_volatile_v<DataType>);

public:

    DataBlock() noexcept = default;
    ~DataBlock() noexcept
    {
        if (_heap)
        {
            utils::free<alignof(DataType)>(const_cast<std::remove_const_t<DataType>*>(_area.data));
        }
    }

    DataBlock(const DataBlock& object) noexcept
    {
        if (object._heap)
        {
            _area.data = static_cast<DataType*>(utils::allocate<alignof(DataType)>(object._area.size * sizeof(DataType)));
            if (_area.data != nullptr)
            {
                std::memcpy(const_cast<std::remove_const_t<DataType>*>(_area.data), object._area.data, object._area.size * sizeof(DataType));

                _area.size = object._area.size;
                _heap      = true;
            }
        }
        else
        {
            _area = object._area;
        }
    }

    DataBlock(DataBlock&& object) noexcept
        : _area { object._area }
        , _heap { object._heap }
    {
        object._area = {};
        object._heap = false;
    }

    DataBlock& operator=(const DataBlock& object) noexcept
    {
        if (&object != this)
        {
            if (_heap)
            {
                utils::free<alignof(DataType)>(const_cast<std::remove_const_t<DataType>*>(_area.data));
            }

            _area = {};
            _heap = false;

            if (object._heap)
            {
                _area.data = static_cast<DataType*>(utils::allocate<alignof(DataType)>(object._area.size * sizeof(DataType)));
                if (_area.data != nullptr)
                {
                    std::memcpy(const_cast<std::remove_const_t<DataType>*>(_area.data), object._area.data, object._area.size * sizeof(DataType));

                    _area.size = object._area.size;
                    _heap      = true;
                }
            }
            else
            {
                _area = object._area;
            }
        }

        return *this;
    }

    DataBlock& operator=(DataBlock&& object) noexcept
    {
        if (&object != this)
        {
            if (_heap)
            {
                utils::free<alignof(DataType)>(const_cast<std::remove_const_t<DataType>*>(_area.data));
            }

            _area = object._area;
            _heap = object._heap;

            object._area = {};
            object._heap = false;
        }

        return *this;
    }

public:

    template <bool Copy = false>
    static DataBlock FromData(DataType* const data, const SizeType size) noexcept
    {
        DataBlock block;

        if (data != nullptr && size != 0)
        {
            if constexpr (Copy)
            {
                block._area.data = static_cast<DataType*>(utils::allocate<alignof(DataType)>(size * sizeof(DataType)));
                if (block._area.data != nullptr)
                {
                    std::memcpy(const_cast<std::remove_const_t<DataType>*>(block._area.data), data, size * sizeof(DataType));

                    block._area.size = size;
                    block._heap      = true;
                }
            }
            else
            {
                block._area = { data, size };
            }
        }

        return block;
    }

    template <bool Zeros = false>
    static DataBlock FromArea(DataType* const buffer, const SizeType length) noexcept
    {
        DataBlock block;

        if (buffer != nullptr && length != 0)
        {
            if constexpr (Zeros)
            {
                std::memset(buffer, 0, length * sizeof(DataType));
            }

            block._area = { buffer, length };
        }

        return block;
    }

    template <bool Zeros = false>
    static DataBlock FromHeap(const SizeType size) noexcept
    {
        assert(size >= 0);

        DataBlock block;

        if (size != 0)
        {
            block._area.data = static_cast<DataType*>(utils::allocate<alignof(DataType)>(size * sizeof(DataType)));
            if (block._area.data != nullptr)
            {
                if constexpr (Zeros)
                {
                    std::memset(const_cast<std::remove_const_t<DataType>*>(block._area.data), 0, size * sizeof(DataType));
                }

                block._area.size = size;
                block._heap      = true;
            }
        }

        return block;
    }

public:

    bool operator<(const DataBlock& block) const noexcept
    {
        const int result = std::memcmp(_area.data, block._area.data, std::min(_area.size, block._area.size));
        return result < 0 || result == 0 && _area.size < block._area.size;
    }

    bool operator>(const DataBlock& block) const noexcept
    {
        const int result = std::memcmp(_area.data, block._area.data, std::min(_area.size, block._area.size));
        return result > 0 || result == 0 && _area.size > block._area.size;
    }

    bool operator<=(const DataBlock& block) const noexcept
    {
        const int result = std::memcmp(_area.data, block._area.data, std::min(_area.size, block._area.size));
        return result < 0 || result == 0 && _area.size <= block._area.size;
    }

    bool operator>=(const DataBlock& block) const noexcept
    {
        const int result = std::memcmp(_area.data, block._area.data, std::min(_area.size, block._area.size));
        return result > 0 || result == 0 && _area.size >= block._area.size;
    }

    bool operator==(const DataBlock& block) const noexcept
    {
        return _area.size == block._area.size && std::memcmp(_area.data, block._area.data, _area.size) == 0;
    }

    bool operator!=(const DataBlock& block) const noexcept
    {
        return _area.size != block._area.size || std::memcmp(_area.data, block._area.data, _area.size) != 0;
    }

public:

    DataBlock operator+(const DataBlock& block) const noexcept
    {
        DataBlock result;

        if (result = FromHeap(Size() + block.Size()); result.Valid())
        {
            std::memcpy(result.Data(), Data(), Size());
            std::memcpy(result.Data() + Size(), block.Data(), block.Size());
        }

        return result;
    }

    DataBlock& operator+=(const DataBlock& block) noexcept
    {
        if (const SizeType size = Size(); Resize(size + block.Size()))
        {
            std::memcpy(Data() + size, block.Data(), block.Size());
        }

        return *this;
    }

public:

    const DataType& operator[](const SizeType index) const noexcept
    {
        return _area[index];
    }

    DataType& operator[](const SizeType index) noexcept
    {
        return _area[index];
    }

public:

    bool Valid() const noexcept
    {
        return _area.Valid();
    }

    bool Invalid() const noexcept
    {
        return _area.Invalid();
    }

public:

    const DataView<DataType, SizeType>& Area() const noexcept
    {
        return _area;
    }

    bool Heap() const noexcept
    {
        return _heap;
    }

public:

    void FillWithZeros() noexcept
    {
        _area.FillWithZeros();
    }

    void FillWithOnes() noexcept
    {
        _area.FillWithOnes();
    }

public:

    bool ToHeap() noexcept
    {
        if (_heap == false)
        {
            if (_area.data != nullptr)
            {
                DataType* const new_data = static_cast<DataType*>(utils::allocate<alignof(DataType)>(_area.size * sizeof(DataType)));
                if (new_data != nullptr)
                {
                    std::memcpy(const_cast<std::remove_const_t<DataType>*>(new_data), _area.data, _area.size * sizeof(DataType));

                    _area.data = new_data;
                    _heap      = true;
                }
            }
        }

        return _heap;
    }

    template <bool Zeros = false>
    bool Resize(const SizeType new_size) noexcept
    {
        assert(new_size >= 0);

        if (new_size != _area.size)
        {
            if (new_size == 0)
            {
                if (_heap)
                {
                    utils::free<alignof(DataType)>(const_cast<std::remove_const_t<DataType>*>(_area.data));
                }

                _area = {};
                _heap = false;
            }
            else if (_heap)
            {
                DataType* const new_data = static_cast<DataType*>
                    (utils::reallocate<alignof(DataType)>(const_cast<std::remove_const_t<DataType>*>(_area.data),
                        _area.size * sizeof(DataType), new_size * sizeof(DataType)));
                if (new_data != nullptr)
                {
                    if constexpr (Zeros)
                    {
                        if (new_size > _area.size)
                        {
                            std::memset(const_cast<std::remove_const_t<DataType>*>(new_data + _area.size), 0, (new_size - _area.size) * sizeof(DataType));
                        }
                    }

                    _area = { new_data, new_size };
                }
            }
            else if (new_size < _area.size)
            {
                _area.size = new_size;
            }
            else
            {
                DataType* const new_data = static_cast<DataType*>(utils::allocate<alignof(DataType)>(new_size * sizeof(DataType)));
                if (new_data != nullptr)
                {
                    std::memcpy(const_cast<std::remove_const_t<DataType>*>(new_data), _area.data, _area.size * sizeof(DataType));

                    if constexpr (Zeros)
                    {
                        std::memset(const_cast<std::remove_const_t<DataType>*>(new_data + _area.size), 0, (new_size - _area.size) * sizeof(DataType));
                    }

                    _area = { new_data, new_size };
                    _heap = true;
                }
            }
        }

        return _area.size == new_size;
    }

    void Destroy() noexcept
    {
        if (_heap)
        {
            utils::free<alignof(DataType)>(const_cast<std::remove_const_t<DataType>*>(_area.data));
        }

        _area = {};
        _heap = false;
    }

public:

    const DataType* Previous(const DataType* const iterator) const noexcept
    {
        return _area.Previous(iterator);
    }

    DataType* Previous(DataType* const iterator) noexcept
    {
        return _area.Previous(iterator);
    }

    const DataType* Next(const DataType* const iterator) const noexcept
    {
        return _area.Next(iterator);
    }

    DataType* Next(DataType* const iterator) noexcept
    {
        return _area.Next(iterator);
    }

public:

    const DataType* First() const noexcept
    {
        return _area.First();
    }

    DataType* First() noexcept
    {
        return _area.First();
    }

    const DataType* Last() const noexcept
    {
        return _area.Last();
    }

    DataType* Last() noexcept
    {
        return _area.Last();
    }

public:

    const DataType* Begin() const noexcept
    {
        return _area.Begin();
    }

    DataType* Begin() noexcept
    {
        return _area.Begin();
    }

    const DataType* End() const noexcept
    {
        return _area.End();
    }

    DataType* End() noexcept
    {
        return _area.End();
    }

public:

    SizeType Bytes() const noexcept
    {
        return _area.Bytes();
    }

public:

    const DataType* Data() const noexcept
    {
        return _area.Data();
    }

    DataType* Data() noexcept
    {
        return _area.Data();
    }

    SizeType Size() const noexcept
    {
        return _area.Size();
    }

private:

    DataView<DataType, SizeType> _area;

private:

    bool _heap = false;

};
