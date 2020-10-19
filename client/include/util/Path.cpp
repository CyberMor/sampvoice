/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Path.h"

#include <array>

#include <ShlObj.h>

#include "Logger.h"

Path::Path()
{
    constexpr char kSVDirRelativePath[] = "\\" "GTA San Andreas User Files"
                                          "\\" "sampvoice";

    std::array<char, MAX_PATH> myDocumentsPath {};

    if (const auto hResult = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE,
        NULL, SHGFP_TYPE_CURRENT, myDocumentsPath.data()); FAILED(hResult))
    {
        Logger::LogToFile("[err:path] : failed to get 'MyDocuments' directory (code:%ld)", hResult);
        throw std::exception();
    }

    this->pathString.reserve(MAX_PATH);

    this->pathString.append(myDocumentsPath.data());
    this->pathString.append(kSVDirRelativePath, sizeof(kSVDirRelativePath) - 1);

    if (const auto rCode = SHCreateDirectoryEx(NULL, this->pathString.c_str(), NULL);
        rCode != ERROR_ALREADY_EXISTS && rCode != ERROR_FILE_EXISTS && rCode != ERROR_SUCCESS)
    {
        Logger::LogToFile("[err:path] : failed to create plugin directory (code:%d)", rCode);
        throw std::exception();
    }
}

Path::operator const char* () const noexcept
{
    return this->pathString.c_str();
}

Path::operator const std::string& () const noexcept
{
    return this->pathString;
}
