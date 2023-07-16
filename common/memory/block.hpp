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
struct Block {

    Block() noexcept = default;
    ~Block() noexcept
    {
        if (_heap)
        {
            utils::free<alignof(DataType)>(const_cast<std::remove_const_t<DataType>*>(_area.data));
        }
    }

    Block(const Block& object) noexcept
    {
        if (object._heap)
        {
            _area.data = static_cast<DataType*>
                (utils::allocate<alignof(DataType)>(object._area.size * sizeof(DataType)));
            if (_area.data != nullptr)
            {
                std::memcpy(const_cast<std::remove_const_t<DataType>*>(_area.data),
                    object._area.data, object._area.size * sizeof(DataType));

                _area.size = object._area.size;
                _heap      = true;
            }
        }
        else
        {
            _area = object._area;
        }
    }

    Block(Block&& object) noexcept
        : _area { object._area }
        , _heap { object._heap }
    {
        object._area = {};
        object._heap = false;
    }

    Block& operator=(const Block& object) noexcept
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
                _area.data = static_cast<DataType*>
                    (utils::allocate<alignof(DataType)>(object._area.size * sizeof(DataType)));
                if (_area.data != nullptr)
                {
                    std::memcpy(const_cast<std::remove_const_t<DataType>*>(_area.data),
                        object._area.data, object._area.size * sizeof(DataType));

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

    Block& operator=(Block&& object) noexcept
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
    static Block FromData(const View<DataType, SizeType>& data) noexcept
    {
        Block block;

        if (data.data != nullptr && data.size != 0)
        {
            if constexpr (Copy)
            {
                block._area.data = static_cast<DataType*>
                    (utils::allocate<alignof(DataType)>(data.size * sizeof(DataType)));
                if (block._area.data != nullptr)
                {
                    std::memcpy(const_cast<std::remove_const_t<DataType>*>(block._area.data),
                        data.data, data.size * sizeof(DataType));

                    block._area.size = data.size;
                    block._heap      = true;
                }
            }
            else
            {
                block._area = data;
            }
        }

        return block;
    }

    template <bool Zeros = false>
    static Block FromArea(const View<DataType, SizeType>& area) noexcept
    {
        Block block;

        if (area.data != nullptr && area.size != 0)
        {
            if constexpr (Zeros)
            {
                std::memset(area.data, 0, area.size * sizeof(DataType));
            }

            block._area = area;
        }

        return block;
    }

    template <bool Zeros = false>
    static Block FromHeap(const SizeType size) noexcept
    {
        assert(size >= 0);

        Block block;

        if (size != 0)
        {
            block._area.data = static_cast<DataType*>
                (utils::allocate<alignof(DataType)>(size * sizeof(DataType)));
            if (block._area.data != nullptr)
            {
                if constexpr (Zeros)
                {
                    std::memset(const_cast<std::remove_const_t<DataType>*>(block._area.data),
                        0, size * sizeof(DataType));
                }

                block._area.size = size;
                block._heap      = true;
            }
        }

        return block;
    }

public:

    bool operator<(const Block& block) const noexcept
    {
        const int result = std::memcmp(_area.data, block._area.data,
            std::min(_area.size, block._area.size));
        return result < 0 || result == 0 && _area.size < block._area.size;
    }

    bool operator>(const Block& block) const noexcept
    {
        const int result = std::memcmp(_area.data, block._area.data,
            std::min(_area.size, block._area.size));
        return result > 0 || result == 0 && _area.size > block._area.size;
    }

    bool operator<=(const Block& block) const noexcept
    {
        const int result = std::memcmp(_area.data, block._area.data,
            std::min(_area.size, block._area.size));
        return result < 0 || result == 0 && _area.size <= block._area.size;
    }

    bool operator>=(const Block& block) const noexcept
    {
        const int result = std::memcmp(_area.data, block._area.data,
            std::min(_area.size, block._area.size));
        return result > 0 || result == 0 && _area.size >= block._area.size;
    }

    bool operator==(const Block& block) const noexcept
    {
        return _area.size == block._area.size &&
            std::memcmp(_area.data, block._area.data, block._area.size) == 0;
    }

    bool operator!=(const Block& block) const noexcept
    {
        return _area.size != block._area.size ||
            std::memcmp(_area.data, block._area.data, block._area.size) != 0;
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

    const View<DataType, SizeType>& Area() const noexcept
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
        std::memset(Data(), 0x00, Bytes());
    }

    void FillWithOnes() noexcept
    {
        std::memset(Data(), 0xFF, Bytes());
    }

public:

    bool ToHeap() noexcept
    {
        if (_heap == false)
        {
            if (_area.data != nullptr)
            {
                DataType* const new_data = static_cast<DataType*>
                    (utils::allocate<alignof(DataType)>(_area.size * sizeof(DataType)));
                if (new_data != nullptr)
                {
                    std::memcpy(const_cast<std::remove_const_t<DataType>*>(new_data),
                        _area.data, _area.size * sizeof(DataType));

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
                            std::memset(const_cast<std::remove_const_t<DataType>*>(new_data + _area.size),
                                0, (new_size - _area.size) * sizeof(DataType));
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
                DataType* const new_data = static_cast<DataType*>
                    (utils::allocate<alignof(DataType)>(new_size * sizeof(DataType)));
                if (new_data != nullptr)
                {
                    std::memcpy(const_cast<std::remove_const_t<DataType>*>(new_data),
                        _area.data, _area.size * sizeof(DataType));

                    if constexpr (Zeros)
                    {
                        std::memset(const_cast<std::remove_const_t<DataType>*>(new_data + _area.size),
                            0, (new_size - _area.size) * sizeof(DataType));
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

    SizeType Bytes() const noexcept
    {
        return _area.Bytes();
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

    const DataType* Previous(DataType* const iterator) const noexcept
    {
        return _area.Previous(iterator);
    }

    DataType* Previous(DataType* const iterator) noexcept
    {
        return _area.Previous(iterator);
    }

    const DataType* Next(DataType* const iterator) const noexcept
    {
        return _area.Next(iterator);
    }

    DataType* Next(DataType* const iterator) noexcept
    {
        return _area.Next(iterator);
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

    View<DataType, SizeType> _area;

private:

    bool _heap = false;

};
