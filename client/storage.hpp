/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.com>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <cassert>
#include <charconv>
#include <cstring>

#include <Windows.h>
#include <Shlwapi.h>
#include <ShlObj.h>

#include <system/types.hpp>
#include <network/address.hpp>

#pragma comment(lib, "Shlwapi.lib")

struct Storage {
    
    Storage(const Storage&) = delete;
    Storage(Storage&&) = delete;
    Storage& operator=(const Storage&) = delete;
    Storage& operator=(Storage&&) = delete;

private:

    Storage() noexcept
    {
        *_resources_path = '\0';
        *_documents_path = '\0';
    }

    ~Storage() noexcept = default;

public:

    static Storage& Instance() noexcept
    {
        static Storage instance;
        return instance;
    }

public:

    bool Initialize() noexcept
    {
        static const char kResourcesDirectory[] = "\\" "sampvoice";
        static const char kDocumentsDirectory[] = "\\" "GTA San Andreas User Files"
                                                  "\\" "sampvoice";

        const DWORD resources_length = GetCurrentDirectory(sizeof(_resources_path), _resources_path);
        if (resources_length == 0 || resources_length > sizeof(_resources_path) - (sizeof(kResourcesDirectory) + 1))
        {
            *_resources_path = '\0';

            return false;
        }

        if (const HRESULT result = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE,
            NULL, SHGFP_TYPE_CURRENT, _documents_path); FAILED(result))
        {
            *_resources_path = '\0';
            *_documents_path = '\0';

            return false;
        }

        const size_t documents_length = std::strlen(_documents_path);
        if (documents_length > sizeof(_documents_path) - (sizeof(kDocumentsDirectory) + 1))
        {
            *_resources_path = '\0';
            *_documents_path = '\0';

            return false;
        }

        str_t resources_buffer = _resources_path + resources_length;
        str_t documents_buffer = _documents_path + documents_length;

        std::memcpy(resources_buffer, kResourcesDirectory, sizeof(kResourcesDirectory));
        std::memcpy(documents_buffer, kDocumentsDirectory, sizeof(kDocumentsDirectory));

        resources_buffer += sizeof(kResourcesDirectory) - 1;
        documents_buffer += sizeof(kDocumentsDirectory) - 1;

        if (PathFileExists(_resources_path) == FALSE)
        {
            *_resources_path = '\0';
            *_documents_path = '\0';

            return false;
        }

        if (const int code = SHCreateDirectoryEx(NULL, _documents_path, NULL);
            code != ERROR_ALREADY_EXISTS && code != ERROR_FILE_EXISTS && code != ERROR_SUCCESS)
        {
            *_resources_path = '\0';
            *_documents_path = '\0';

            return false;
        }

        *resources_buffer = '\\'; ++resources_buffer; *resources_buffer = '\0';
        *documents_buffer = '\\'; ++documents_buffer; *documents_buffer = '\0';

        _resources_limit = ((_resources_path + sizeof(_resources_path)) - resources_buffer) - 1;
        _documents_limit = ((_documents_path + sizeof(_documents_path)) - documents_buffer) - 1;

        _resources_buffer = resources_buffer;
        _documents_buffer = documents_buffer;

        return true;
    }

    void Deinitialize() noexcept
    {
        *_resources_path = '\0';
        *_documents_path = '\0';

        _resources_limit = 0;
        _documents_limit = 0;

        _resources_buffer = nullptr;
        _documents_buffer = nullptr;
    }

public:

    cstr_t GetResourcesDirectory(str_t* const end = nullptr) noexcept
    {
        static const char kResourcesDirectory[] = "resources";

        if (_resources_limit < sizeof(kResourcesDirectory) - 1)
            return nullptr;

        std::memcpy(_resources_buffer, kResourcesDirectory, sizeof(kResourcesDirectory));

        if (end != nullptr) *end = _resources_buffer + (sizeof(kResourcesDirectory) - 1);

        return _resources_path;
    }

    cstr_t GetLanguagesDirectory(str_t* const end = nullptr) noexcept
    {
        static const char kLanguagesDirectory[] = "languages";

        if (_resources_limit < sizeof(kLanguagesDirectory) - 1)
            return nullptr;

        std::memcpy(_resources_buffer, kLanguagesDirectory, sizeof(kLanguagesDirectory));

        if (end != nullptr) *end = _resources_buffer + (sizeof(kLanguagesDirectory) - 1);

        return _resources_path;
    }

public:

    cstr_t GetLogFile() noexcept
    {
        static const char kLogFile[] = "svlog.txt";

        if (_documents_limit < sizeof(kLogFile) - 1)
            return nullptr;

        std::memcpy(_documents_buffer, kLogFile, sizeof(kLogFile));

        return _documents_path;
    }

    cstr_t GetConfigFile() noexcept
    {
        static const char kConfigFile[] = "svconfig.bin";

        if (_documents_limit < sizeof(kConfigFile) - 1)
            return nullptr;

        std::memcpy(_documents_buffer, kConfigFile, sizeof(kConfigFile));

        return _documents_path;
    }

    cstr_t GetBlacklistFile(const IPv4Address& server) noexcept
    {
        static const char kBlacklistFilePrefix[] = "svblacklist_";
        static const char kBlacklistFileDelimiter[] = "_";
        static const char kBlacklistFilePostfix[] = ".txt";

        constexpr size_t kBlacklistFileLimit = (sizeof(kBlacklistFilePrefix) - 1) + IPv4Address::HostLengthLimit +
            (sizeof(kBlacklistFileDelimiter) - 1) + IPv4Address::PortLengthLimit + (sizeof(kBlacklistFilePostfix) - 1);

        if (_documents_limit < kBlacklistFileLimit)
            return nullptr;

        str_t documents_buffer = _documents_buffer;

        std::memcpy(documents_buffer, kBlacklistFilePrefix, sizeof(kBlacklistFilePrefix) - 1);
        documents_buffer += sizeof(kBlacklistFilePrefix) - 1;

        if (server.PrintHost(documents_buffer)) while (*documents_buffer != '\0')
            ++documents_buffer;

        std::memcpy(documents_buffer, kBlacklistFileDelimiter, sizeof(kBlacklistFileDelimiter) - 1);
        documents_buffer += sizeof(kBlacklistFileDelimiter) - 1;

        if (server.PrintPort(documents_buffer)) while (*documents_buffer != '\0')
            ++documents_buffer;

        std::memcpy(documents_buffer, kBlacklistFilePostfix, sizeof(kBlacklistFilePostfix));

        return _documents_path;
    }

public:

    cstr_t GetDocumentFile(const cstr_t name) noexcept
    {
        const size_t length = std::strlen(name);
        if (_documents_limit < length) return nullptr;
        std::memcpy(_documents_buffer, name, length + 1);
        return _documents_path;
    }

public:

    cstr_t GetResourceFile(const cstr_t name) noexcept
    {
        if (str_t buffer; GetResourcesDirectory(&buffer) != nullptr)
        {
            *buffer++ = '\\';
            const size_t limit = (_resources_path + (sizeof(_resources_path) - 1)) - buffer;
            if (const size_t length = std::strlen(name); length <= limit)
            {
                std::memcpy(buffer, name, length + 1);
                return _resources_path;
            }
        }

        return nullptr;
    }

    cstr_t GetLanguageFile(const cstr_t name) noexcept
    {
        if (str_t buffer; GetLanguagesDirectory(&buffer) != nullptr)
        {
            *buffer++ = '\\';
            const size_t limit = (_resources_path + (sizeof(_resources_path) - 1)) - buffer;
            if (const size_t length = std::strlen(name); length <= limit)
            {
                std::memcpy(buffer, name, length + 1);
                return _resources_path;
            }
        }

        return nullptr;
    }

public:

    /*
        Callback = void(cstr_t path)
    */
    template <class Callback>
    bool ForEachResourceFile(Callback&& callback) noexcept
    {
        str_t buffer;

        WIN32_FIND_DATA data;

        {
            constexpr const char kSearchMask[] = "\\*";

            GetResourcesDirectory(&buffer);

            const size_t limit = (_resources_path + (sizeof(_resources_path) - 1)) - buffer;
            if (limit < sizeof(kSearchMask) - 1) return false;

            std::memcpy(buffer, kSearchMask, sizeof(kSearchMask));
        }

        const HANDLE handle = FindFirstFile(_resources_path, &data);
        if (handle == INVALID_HANDLE_VALUE) return false;

        *buffer++ = '\\';

        const size_t limit = (_resources_path + (sizeof(_resources_path) - 1)) - buffer;

        do if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            if (const size_t length = std::strlen(data.cFileName); length <= limit)
            {
                std::memcpy(buffer, data.cFileName, length + 1);
                callback(_resources_path);
            }
        }
        while (FindNextFile(handle, &data));

        const DWORD result = GetLastError();

        FindClose(handle);

        return result == ERROR_NO_MORE_FILES;
    }

    /*
        Callback = void(cstr_t path)
    */
    template <class Callback>
    bool ForEachLanguageFile(Callback&& callback) noexcept
    {
        str_t buffer;

        WIN32_FIND_DATA data;

        {
            constexpr const char kSearchMask[] = "\\*";

            GetLanguagesDirectory(&buffer);

            const size_t limit = (_resources_path + (sizeof(_resources_path) - 1)) - buffer;
            if (limit < sizeof(kSearchMask) - 1) return false;

            std::memcpy(buffer, kSearchMask, sizeof(kSearchMask));
        }

        const HANDLE handle = FindFirstFile(_resources_path, &data);
        if (handle == INVALID_HANDLE_VALUE) return false;

        *buffer++ = '\\';

        const size_t limit = (_resources_path + (sizeof(_resources_path) - 1)) - buffer;

        do if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            if (const size_t length = std::strlen(data.cFileName); length <= limit)
            {
                std::memcpy(buffer, data.cFileName, length + 1);
                callback(_resources_path);
            }
        }
        while (FindNextFile(handle, &data));

        const DWORD result = GetLastError();

        FindClose(handle);

        return result == ERROR_NO_MORE_FILES;
    }

private:

    char _resources_path[MAX_PATH];
    char _documents_path[MAX_PATH];

    size_t _resources_limit = 0;
    size_t _documents_limit = 0;

    str_t _resources_buffer = nullptr;
    str_t _documents_buffer = nullptr;

};
