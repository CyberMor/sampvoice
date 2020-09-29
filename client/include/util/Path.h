/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <string>

class Path {
public:

    Path(const Path&) = default;
    Path(Path&&) noexcept = default;
    Path& operator=(const Path&) = default;
    Path& operator=(Path&&) noexcept = default;

public:

    explicit Path();

public:

    operator const char* () const noexcept;
    operator const std::string& () const noexcept;

    Path& operator/(const std::string& rPath);
    Path& operator+(const std::string& cName);

private:

    std::string pathString;

};

using PathPtr = std::shared_ptr<Path>;
#define MakePath std::make_shared<Path>
