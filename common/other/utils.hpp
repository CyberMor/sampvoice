/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <chrono>
#include <type_traits>
#include <algorithm>
#include <thread>

#ifndef _WIN32
#include <unistd.h>
#include <byteswap.h>
#if __GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 25)
#include <sys/random.h>
#else
#define _GNU_SOURCE
#include <sys/syscall.h>
#define getrandom(buf, buflen, flags) syscall(SYS_getrandom, buf, buflen, flags)
#endif
#endif

#include <system/types.hpp>
#include <system/time.hpp>

#ifdef _WIN32
#define bswap_16 _byteswap_ushort
#define bswap_32 _byteswap_ulong
#define bswap_64 _byteswap_uint64
#endif

namespace utils
{
    namespace string
    {
        namespace ending
        {
            constexpr char crlf = -1; // Line Ending: 'crlf'
            constexpr char text = -2; // Line Ending: 'crlf' or 'cr' or 'lf'
        }

        template <class Character>
        constexpr size_t length(const Character* string, const Character rterminator = {}) noexcept
        {
            assert(string != nullptr);

            const auto begin = string;
            while (*string != rterminator) ++string;
            return string - begin;
        }

        template <class Character>
        static inline void copy(const Character* source, Character* target, size_t limit, const Character rterminator = {}, const Character wterminator = {}) noexcept
        {
            if (limit != 0)
            {
                assert(target != nullptr);

                assert(limit == 1 ||  source != nullptr);
                while (limit != 1 && *source != rterminator)
                {
                    *target++ = *source++;

                    --limit;
                }

                *target = wterminator;
            }
        }

        template <class Character>
        constexpr size_t find(const Character* string, const Character symbol, const Character rterminator = {}) noexcept
        {
            assert(string != nullptr);

            const auto begin = string;
            while (*string != symbol && *string != rterminator) ++string;
            return string - begin;
        }

        template <class Character>
        constexpr bool has(const Character* string, const Character symbol, const Character rterminator = {}) noexcept
        {
            assert(string != nullptr);

            while (*string != symbol && *string != rterminator) ++string;
            return *string == symbol;
        }
    }

    namespace crypto
    {
        static inline uint_t random() noexcept
        {
            uint_t value;
#ifdef _WIN32
            if (rand_s(&value) != 0)
                value = 0;
#else
            if (getrandom(&value, sizeof(value), 0) != sizeof(value))
                value = 0;
#endif
            return value;
        }
    }

    namespace thread
    {
        static inline void sleep(const Time time) noexcept
        {
            assert(time.Milliseconds() >= 0);

            std::this_thread::sleep_for(std::chrono::milliseconds(time.Milliseconds()));
        }
    }

    namespace bitset
    {
        template <class Type, typename = std::enable_if_t<!std::is_const_v<Type> && std::is_integral_v<Type>>>
        static inline bool set(Type& bits, const size_t bit) noexcept
        {
            assert(bit < Bits<Type>);

            const bool result = (bits & (HighBit<Type> >> bit)) == 0;
            if (result) bits |= (HighBit<Type> >> bit);
            return result;
        }

        template <class Type, typename = std::enable_if_t<std::is_integral_v<Type>>>
        constexpr bool test(const Type& bits, const size_t bit) noexcept
        {
            assert(bit < Bits<Type>);

            return (bits & (HighBit<Type> >> bit)) != 0;
        }

        template <class Type, typename = std::enable_if_t<!std::is_const_v<Type> && std::is_integral_v<Type>>>
        static inline bool reset(Type& bits, const size_t bit) noexcept
        {
            assert(bit < Bits<Type>);

            const bool result = (bits & (HighBit<Type> >> bit)) != 0;
            if (result) bits &= ~(HighBit<Type> >> bit);
            return result;
        }
    }

    namespace ct // compile-time
    {
        template <class Value, typename = std::enable_if_t<std::is_integral_v<Value>>>
        constexpr Value bswap(Value value) noexcept
        {
            Value result = 0;

            for (size_t i = 0; i != sizeof(Value); ++i)
            {
                result >>= CHAR_BIT;
                result  |= value & HighByte<Value>;
                 value <<= CHAR_BIT;
            }

            return result;
        }

        constexpr bool is_little_endian() noexcept
        {
            return 'ABCD' == 0x41424344;
        }

        constexpr bool is_big_endian() noexcept
        {
            return 'ABCD' == 0x44434241;
        }
    }

    template <size_t Size>
    static inline void bswap(const ptr_t target, const cptr_t source) noexcept
    {
        for (size_t i = 0; i != Size; ++i)
            static_cast<adr_t>(target)[i] = static_cast<cadr_t>(source)[(Size - 1) - i];
    }

    template <size_t Size>
    static inline void bswap(const ptr_t target) noexcept
    {
        for (size_t i = 0; i != Size / 2; ++i)
            std::swap(static_cast<adr_t>(target)[i], static_cast<adr_t>(target)[(Size - 1) - i]);
    }

    template <class Type>
    constexpr bool is_bswappable =
       std::is_same_v<Type,  sword_t> || std::is_same_v<Type,  uword_t>
    || std::is_same_v<Type, sdword_t> || std::is_same_v<Type, udword_t>
    || std::is_same_v<Type, sqword_t> || std::is_same_v<Type, uqword_t>
    || std::is_same_v<Type, fdword_t> || std::is_same_v<Type, fqword_t>;

    static inline sword_t bswap(const sword_t value) noexcept { return bswap_16(value); }
    static inline uword_t bswap(const uword_t value) noexcept { return bswap_16(value); }

    static inline void bswap(sword_t* const value) noexcept { *value = bswap_16(*value); }
    static inline void bswap(uword_t* const value) noexcept { *value = bswap_16(*value); }

    static inline sdword_t bswap(const sdword_t value) noexcept { return bswap_32(value); }
    static inline udword_t bswap(const udword_t value) noexcept { return bswap_32(value); }

    static inline void bswap(sdword_t* const value) noexcept { *value = bswap_32(*value); }
    static inline void bswap(udword_t* const value) noexcept { *value = bswap_32(*value); }

    static inline sqword_t bswap(const sqword_t value) noexcept { return bswap_64(value); }
    static inline uqword_t bswap(const uqword_t value) noexcept { return bswap_64(value); }

    static inline void bswap(sqword_t* const value) noexcept { *value = bswap_64(*value); }
    static inline void bswap(uqword_t* const value) noexcept { *value = bswap_64(*value); }

    static inline fdword_t bswap(const fdword_t value) noexcept
    {
        fdword_t result;
        bswap<sizeof(fdword_t)>(&result, &value);
        return result;
    }
    static inline fqword_t bswap(const fqword_t value) noexcept
    {
        fqword_t result;
        bswap<sizeof(fqword_t)>(&result, &value);
        return result;
    }

    static inline void bswap(fdword_t* const value) noexcept { bswap<sizeof(fdword_t)>(value); }
    static inline void bswap(fqword_t* const value) noexcept { bswap<sizeof(fqword_t)>(value); }

    static inline bool is_little_endian() noexcept
    {
        const volatile uword_t word = 0xFF;
        return *reinterpret_cast<const volatile ubyte_t*>(&word) != 0;
    }

    static inline bool is_big_endian() noexcept
    {
        const volatile uword_t word = 0xFF;
        return *reinterpret_cast<const volatile ubyte_t*>(&word) == 0;
    }

    template <class Number, typename = std::enable_if_t<std::is_unsigned_v<Number>>>
    constexpr bool is_two_power(const Number number) noexcept
    {
        return number != 0 && (number & (number - 1)) == 0;
    }

    template <class Number>
    constexpr Number align_down(const Number number, const size_t align = alignof(std::max_align_t)) noexcept
    {
        return (Number)((size_t)(number) & ~(align - 1));
    }

    template <class Number>
    constexpr Number align_up(const Number number, const size_t align = alignof(std::max_align_t)) noexcept
    {
        return (Number)(((size_t)(number) + (align - 1)) & ~(align - 1));
    }

    template <size_t Align = alignof(std::max_align_t)>
    static inline ptr_t allocate(const size_t size) noexcept
    {
        static_assert(is_two_power(Align));

        if constexpr (Align > alignof(std::max_align_t))
#ifdef _WIN32
            return _aligned_malloc(size, Align);
#else
            return std::aligned_alloc(Align, size);
#endif
        else return std::malloc(size);
    }

    template <size_t Align = alignof(std::max_align_t)>
    static inline ptr_t reallocate(const ptr_t old_pointer, [[maybe_unused]] const size_t old_size, const size_t new_size) noexcept
    {
        static_assert(is_two_power(Align));

        if constexpr (Align > alignof(std::max_align_t))
#ifdef _WIN32
            return _aligned_realloc(old_pointer, new_size, Align);
#else
        {
            const ptr_t new_pointer = std::aligned_alloc(Align, new_size);
            if (new_pointer != nullptr)
            {
                std::memcpy(new_pointer, old_pointer, std::min(new_size, old_size));
                std::free(old_pointer);
            }

            return new_pointer;
        }
#endif
        else return std::realloc(old_pointer, new_size);
    }

    template <size_t Align = alignof(std::max_align_t)>
    static inline void free(const ptr_t pointer) noexcept
    {
        static_assert(is_two_power(Align));

#ifdef _WIN32
        if constexpr (Align > alignof(std::max_align_t))
            _aligned_free(pointer);
        else
#endif
            std::free(pointer);
    }

    template <class Number, typename = std::enable_if_t<std::is_integral_v<Number>>>
    constexpr char to_hex(const Number nibble) noexcept
    {
        const char kNibbleToSymbolTable[16]
        {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            'A', 'B', 'C', 'D', 'E', 'F'
        };

        return kNibbleToSymbolTable[nibble & static_cast<Number>(0b1111)];
    }
}

constexpr bool LittleEndian = false;
constexpr bool    BigEndian = true;

constexpr bool   HostEndian = utils::ct::is_big_endian();
constexpr bool    NetEndian = BigEndian;

#define ForwardForEach(iterator, container)    \
    for (auto iterator  = (container).Begin(); \
              iterator != (container).End();   \
              iterator  = (container).Next(iterator))

#define BackwardForEach(iterator, container)                         \
    for (auto iterator  = (container).Previous((container).End());   \
              iterator != (container).Previous((container).Begin()); \
              iterator  = (container).Previous(iterator))

#define BitsetForEach(bit, bits)                                                                      \
    for (std::remove_cv_t<decltype(bits)> bit = 0, _bits_ = (bits); _bits_ != 0; _bits_ <<= 1, ++bit) \
        if (_bits_ & HighBit<decltype(_bits_)>)
