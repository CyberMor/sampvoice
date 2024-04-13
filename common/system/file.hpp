/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <sys/stat.h>

#include <cassert>
#include <cstdio>

#include <memory/block.hpp>
#include <memory/packet.hpp>
#include <other/utils.hpp>

#include "types.hpp"

struct File {

    struct Info : private stat {

        Info() noexcept = default;
        ~Info() noexcept = default;
        Info(const Info&) noexcept = default;
        Info(Info&&) noexcept = default;
        Info& operator=(const Info&) noexcept = default;
        Info& operator=(Info&&) noexcept = default;

    public:

        auto Device()               const noexcept { return st_dev; }
        auto SerialNumber()         const noexcept { return st_ino; }
        auto Mode()                 const noexcept { return st_mode; }
        auto LinkCount()            const noexcept { return st_nlink; }
        auto UserID()               const noexcept { return st_uid; }
        auto GroupID()              const noexcept { return st_gid; }
        auto DeviceNumber()         const noexcept { return st_rdev; }
        auto Size()                 const noexcept { return st_size; }
        auto LastAccessTime()       const noexcept { return st_atime; }
        auto LastModificationTime() const noexcept { return st_mtime; }
        auto LastChangeTime()       const noexcept { return st_ctime; }

    };

private:

    static_assert(sizeof(Info) == sizeof(struct stat),
        "'Info' type must be identical to 'struct stat' type");

public:

    File() noexcept = default;
    ~File() noexcept
    {
        if (_handle != nullptr) std::fclose(_handle);
    }

    File(const File&) = delete;
    File(File&& object) noexcept
        : _handle { object._handle }
    {
        object._handle = nullptr;
    }

    File& operator=(const File&) = delete;
    File& operator=(File&& object) noexcept
    {
        if (&object != this)
        {
            if (_handle != nullptr) std::fclose(_handle);

            _handle = object._handle;

            object._handle = nullptr;
        }

        return *this;
    }

private:

    File(std::FILE* const handle) noexcept
        : _handle { handle }
    {}

public:

    File(const cstr_t file, const cstr_t modes) noexcept
        : File { std::fopen(file, modes) }
    {
        assert(file  != nullptr && *file  != '\0');
        assert(modes != nullptr && *modes != '\0');
    }

public:

    static File Temporary() noexcept
    {
        return std::tmpfile();
    }

public:

    static bool Rename(const cstr_t old_path, const cstr_t new_path) noexcept
    {
        assert(old_path != nullptr && *old_path != '\0');
        assert(new_path != nullptr && *new_path != '\0');

        return std::rename(old_path, new_path) == 0;
    }

    static bool Remove(const cstr_t path) noexcept
    {
        assert(path != nullptr && *path != '\0');

        return std::remove(path) == 0;
    }

public:

    template <class DataType = ubyte_t, class SizeType = size_t>
    static DataBlock<DataType, SizeType> Content(const cstr_t path) noexcept
    {
        assert(path != nullptr && *path != '\0');

        DataBlock<DataType, SizeType> result;

        if (std::FILE* const file = std::fopen(path, "rb"); file != nullptr)
        {
            if (std::fseek(file, 0, SEEK_END) == 0)
            {
                if (const long size = std::ftell(file); size > 0 && size % sizeof(DataType) == 0)
                {
                    if (std::fseek(file, 0, SEEK_SET) == 0)
                    {
                        if (auto content = DataBlock<DataType, SizeType>::FromHeap(size / sizeof(DataType)); content.Valid())
                        {
                            if (std::fread(content.Data(), sizeof(DataType), content.Size(), file) == content.Size())
                            {
                                result = std::move(content);
                            }
                        }
                    }
                }
            }

            std::fclose(file);
        }

        return result;
    }

    static size_t Size(const cstr_t path) noexcept
    {
        assert(path != nullptr && *path != '\0');

        size_t result = 0;

        if (std::FILE* const file = std::fopen(path, "rb"); file != nullptr)
        {
            if (std::fseek(file, 0, SEEK_END) == 0)
            {
                if (const long size = std::ftell(file); size > 0)
                {
                    result = size;
                }
            }

            std::fclose(file);
        }

        return result;
    }

public:

    static bool Explore(const cstr_t path, Info& info) noexcept
    {
        assert(path != nullptr && *path != '\0');

        return stat(path, reinterpret_cast<struct stat*>(&info)) == 0;
    }

public:

    std::FILE* Handle() const noexcept
    {
        return _handle;
    }

public:

    bool Valid() const noexcept
    {
        return _handle != nullptr;
    }

    bool Invalid() const noexcept
    {
        return _handle == nullptr;
    }

public:

    bool Initialize(const cstr_t file, const cstr_t modes) noexcept
    {
        assert(file  != nullptr && *file  != '\0');
        assert(modes != nullptr && *modes != '\0');

        _handle = (_handle != nullptr ? std::freopen(file, modes, _handle) : std::fopen(file, modes));
        return _handle != nullptr;
    }

    void Deinitialize() noexcept
    {
        if (_handle != nullptr) std::fclose(_handle);
        _handle = nullptr;
    }

    void Release() noexcept
    {
        _handle = nullptr;
    }

public:

    int Descriptor() const noexcept
    {
        assert(_handle != nullptr);

#pragma warning(push)
#pragma warning(disable : 4996)

        return fileno(_handle);

#pragma warning(pop)
    }

public:

    bool Error() const noexcept
    {
        assert(_handle != nullptr);

        return std::ferror(_handle) != 0;
    }

    bool End() const noexcept
    {
        assert(_handle != nullptr);

        return std::feof(_handle) != 0;
    }

public:

    bool Skip(const size_t bytes) noexcept
    {
        assert(_handle != nullptr);

        return std::fseek(_handle, bytes, SEEK_CUR) == 0;
    }

public:

    size_t Peek(const ptr_t buffer, const size_t length) const noexcept
    {
        assert(_handle != nullptr);

        size_t result = 0;

        if (const long offset = std::ftell(_handle); offset >= 0)
        {
            assert(length == 0 || buffer != nullptr);

            if (const size_t bytes = std::fread(buffer, 1, length, _handle);
                std::fseek(_handle, offset, SEEK_SET) == 0 && std::ferror(_handle) == 0)
            {
                std::clearerr(_handle);
                result = bytes;
            }
        }

        return result;
    }

    template <class DataType, typename = std::enable_if_t<!std::is_const_v<DataType>>>
    size_t Peek(DataType* const buffer, const size_t length) const noexcept
    {
        assert(_handle != nullptr);

        size_t result = 0;

        if (const long offset = std::ftell(_handle); offset >= 0)
        {
            assert(length == 0 || buffer != nullptr);

            if (const size_t units = std::fread(buffer, sizeof(DataType), length, _handle);
                std::fseek(_handle, offset, SEEK_SET) == 0 && std::ferror(_handle) == 0)
            {
                std::clearerr(_handle);
                result = units;
            }
        }

        return result;
    }

    template <class Buffer, typename = std::enable_if_t<!std::is_const_v<Buffer>>>
    size_t PeekValue(Buffer& buffer) const noexcept
    {
        return Peek(reinterpret_cast<std::remove_all_extents_t<Buffer>*>(&buffer), sizeof(Buffer) / sizeof(std::remove_all_extents_t<Buffer>));
    }

public:

    size_t Read(const ptr_t buffer, const size_t length) noexcept
    {
        assert(_handle != nullptr);

        assert(length == 0 || buffer != nullptr);

        return std::fread(buffer, 1, length, _handle);
    }

    template <class DataType, typename = std::enable_if_t<!std::is_const_v<DataType>>>
    size_t Read(DataType* const buffer, const size_t length) noexcept
    {
        assert(_handle != nullptr);

        assert(length == 0 || buffer != nullptr);

        return std::fread(buffer, sizeof(DataType), length, _handle);
    }

    template <class Buffer, typename = std::enable_if_t<!std::is_const_v<Buffer>>>
    size_t ReadValue(Buffer& buffer) noexcept
    {
        return Read(reinterpret_cast<std::remove_all_extents_t<Buffer>*>(&buffer), sizeof(Buffer) / sizeof(std::remove_all_extents_t<Buffer>));
    }

public:

    size_t Write(const cptr_t data, const size_t size) noexcept
    {
        assert(_handle != nullptr);

        assert(size == 0 || data != nullptr);

        return std::fwrite(data, 1, size, _handle);
    }

    template <class DataType>
    size_t Write(const DataType* const data, const size_t size) noexcept
    {
        assert(_handle != nullptr);

        assert(size == 0 || data != nullptr);

        return std::fwrite(data, sizeof(DataType), size, _handle);
    }

    template <class Value>
    size_t WriteValue(const Value& value) noexcept
    {
        return Write(reinterpret_cast<const std::remove_all_extents_t<Value>*>(&value), sizeof(Value) / sizeof(std::remove_all_extents_t<Value>));
    }

public:

    template <bool Endian = HostEndian, class... Buffers, typename = std::enable_if_t<(... && !std::is_const_v<Buffers>)>>
    int PeekPacket(Buffers&... buffers) const noexcept
    {
        DataPacket<Buffers...> packet;
        if (PeekValue(packet) != 1) return Error() ? -1 : 0;
        packet.template Get<Endian>(buffers...);
        return 1;
    }

    template <bool Endian = HostEndian, class... Buffers, typename = std::enable_if_t<(... && !std::is_const_v<Buffers>)>>
    int ReadPacket(Buffers&... buffers) noexcept
    {
        DataPacket<Buffers...> packet;
        if (ReadValue(packet) != 1) return Error() ? -1 : 0;
        packet.template Get<Endian>(buffers...);
        return 1;
    }

    template <bool Endian = HostEndian, class... Values>
    int WritePacket(const Values... values) noexcept
    {
        DataPacket<Values...> packet;
        packet.template Set<Endian>(values...);
        if (WriteValue(packet) != 1) return Error() ? -1 : 0;
        return 1;
    }

public:

    int GetCharacter(char* const buffer) noexcept
    {
        assert(_handle != nullptr);

        assert(buffer != nullptr);

        if (std::fread(buffer, sizeof(char), 1, _handle) != 1)
        {
            if (std::ferror(_handle) != 0)
                return -1; // error

            return 0; // end of file
        }

        if (*buffer == '\r')
        {
            if (std::fread(buffer, sizeof(char), 1, _handle) != 1)
            {
                if (std::ferror(_handle) != 0)
                    return -1; // error

                return 1; // success (end of file)
            }

            *buffer = (*buffer == '\n' ? utils::string::ending::crlf : '\r');
            if (*buffer != utils::string::ending::crlf)
            {
                if (std::fseek(_handle, -1, SEEK_CUR) != 0)
                    return -1; // error
            }
        }

        return 1;
    }

    template <class Character>
    int GetCharacter(Character* const buffer) noexcept
    {
        assert(_handle != nullptr);

        assert(buffer != nullptr);

        if (std::fread(buffer, sizeof(Character), 1, _handle) != 1)
        {
            if (std::ferror(_handle) != 0)
                return -1; // error

            return 0; // end of file
        }

        return 1;
    }

    bool PutCharacter(const char character) noexcept
    {
        assert(_handle != nullptr);

        assert(character >= 0 || character == utils::string::ending::crlf);

        if (character != utils::string::ending::crlf)
            return std::fwrite(&character, sizeof(char), 1, _handle) == 1;
        else return std::fwrite("\r\n", sizeof(char), 2, _handle) == 2;
    }

    template <class Character>
    bool PutCharacter(const Character character) noexcept
    {
        assert(_handle != nullptr);

        return std::fwrite(&character, sizeof(Character), 1, _handle) == 1;
    }

public:

    int GetLine(char* buffer, size_t length, const char rterminator = utils::string::ending::text, const char wterminator = '\0') noexcept
    {
        assert(_handle != nullptr);

        assert(rterminator >= 0 || rterminator == utils::string::ending::crlf || rterminator == utils::string::ending::text);
        assert(wterminator >= 0);

        assert(length == 0 || buffer != nullptr);
        while (length != 0)
        {
            const int result = GetCharacter(buffer);
            if (result == 0) *buffer = wterminator;
            if (result <= 0) return result;

            if (rterminator == utils::string::ending::text)
            {
                if (*buffer == '\n' || *buffer == utils::string::ending::crlf || *buffer == '\r')
                    *buffer = wterminator;
            }
            else // if (rterminator >= 0 || rterminator == utils::string::ending::crlf)
            {
                if (*buffer == rterminator)
                    *buffer = wterminator;
            }

            if (*buffer == wterminator)
                return 1; // success

            ++buffer;
            --length;
        }

        return -2; // too small buffer
    }

    template <class Character, typename = std::enable_if_t<!std::is_const_v<Character>>>
    int GetLine(Character* buffer, size_t length, const Character rterminator = {}, const Character wterminator = {}) noexcept
    {
        assert(_handle != nullptr);

        assert(length == 0 || buffer != nullptr);
        while (length != 0)
        {
            const int result = GetCharacter(buffer);
            if (result == 0) *buffer = wterminator;
            if (result <= 0) return result;

            if (*buffer == rterminator) *buffer = wterminator;
            if (*buffer == wterminator) return 1; // success

            ++buffer;
            --length;
        }

        return -2; // too small buffer
    }

    template <size_t Length>
    int GetLine(char(&buffer)[Length], const char rterminator = utils::string::ending::text, const char wterminator = '\0') noexcept
    {
        return GetLine(buffer, Length, rterminator, wterminator);
    }

    template <class Character, size_t Length, typename = std::enable_if_t<!std::is_const_v<Character>>>
    int GetLine(Character(&buffer)[Length], const Character rterminator = {}, const Character wterminator = {}) noexcept
    {
        return GetLine(buffer, Length, rterminator, wterminator);
    }

    bool PutLine(const char* const string, const size_t length, const char wterminator = '\n') noexcept
    {
        assert(_handle != nullptr);

        return Write(string, length) == length && PutCharacter(wterminator);
    }

    template <class Character>
    bool PutLine(const Character* const string, const size_t length, const Character wterminator = {}) noexcept
    {
        assert(_handle != nullptr);

        return Write(string, length) == length && PutCharacter(wterminator);
    }

    bool PutLine(const char* const string, const char wterminator = '\n', const char rterminator = '\0') noexcept
    {
        return PutLine(string, utils::string::length(string, rterminator), wterminator);
    }

    template <class Character>
    bool PutLine(const Character* const string, const Character wterminator = {}, const Character rterminator = {}) noexcept
    {
        return PutLine(string, utils::string::length(string, rterminator), wterminator);
    }

public:

    template <class... Buffers, typename = std::enable_if_t<(... && !std::is_const_v<Buffers>)>>
    bool Scan(const cstr_t format, Buffers&... buffers) noexcept
    {
        static_assert(sizeof...(buffers) != 0);

        assert(_handle != nullptr);

        assert(format != nullptr && *format != '\0');

        return std::fscanf(_handle, format, (&buffers)...) == sizeof...(buffers);
    }

    template <class... Values>
    bool Print(const cstr_t format, const Values... values) noexcept
    {
        static_assert(sizeof...(values) != 0);

        assert(_handle != nullptr);

        assert(format != nullptr && *format != '\0');

        return std::fprintf(_handle, format, (values)...) == sizeof...(values);
    }

public:

    size_t Available() const noexcept
    {
        assert(_handle != nullptr);

        size_t result = 0;

        if (const long offset = std::ftell(_handle); offset >= 0)
        {
            if (std::fseek(_handle, 0, SEEK_END) == 0)
            {
                if (const long size = std::ftell(_handle); std::fseek(_handle, offset, SEEK_SET) == 0 && size > offset)
                {
                    result = size - offset;
                }
            }
        }

        return result;
    }

    size_t Size() const noexcept
    {
        assert(_handle != nullptr);

        size_t result = 0;

        if (const long offset = std::ftell(_handle); offset >= 0)
        {
            if (std::fseek(_handle, 0, SEEK_END) == 0)
            {
                if (const long size = std::ftell(_handle); std::fseek(_handle, offset, SEEK_SET) == 0 && size > 0)
                {
                    result = size;
                }
            }
        }

        return result;
    }

public:

    bool Explore(Info& info) const noexcept
    {
        return fstat(Descriptor(), reinterpret_cast<struct stat*>(&info)) == 0;
    }

public:

    bool Flush() noexcept
    {
        assert(_handle != nullptr);

        return std::fflush(_handle) == 0;
    }

    void Clear() noexcept
    {
        assert(_handle != nullptr);

        std::clearerr(_handle);
    }

    void Rewind() noexcept
    {
        assert(_handle != nullptr);

        std::rewind(_handle);
    }

public:

    bool GetOffset(long& offset) const noexcept
    {
        assert(_handle != nullptr);

        return (offset = std::ftell(_handle)) >= 0;
    }

    bool SetOffset(const long offset, const int origin = SEEK_SET) noexcept
    {
        assert(_handle != nullptr);

        return std::fseek(_handle, offset, origin) == 0;
    }

public:

    bool SetBuffer(const ptr_t buffer, const size_t length) noexcept
    {
        assert(_handle != nullptr);

        assert(buffer != nullptr && length != 0);

        return std::setvbuf(_handle, static_cast<char*>(buffer), _IOFBF, length) == 0;
    }

    bool ResizeBuffer(const size_t new_size) noexcept
    {
        assert(_handle != nullptr);

        assert(new_size != 0);

        return std::setvbuf(_handle, nullptr, _IOFBF, new_size) == 0;
    }

    bool DisableBuffering() noexcept
    {
        assert(_handle != nullptr);

        return std::setvbuf(_handle, nullptr, _IONBF, 0) == 0;
    }

private:

    std::FILE* _handle = nullptr;

};
