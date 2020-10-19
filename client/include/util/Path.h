/*
    This is a SampVoice project file
    Developer: CyberMor <cyber.mor.2020@gmail.ru>

    See more here https://github.com/CyberMor/sampvoice

    Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>
#include <string>

struct Path {

    Path(const Path&) = default;
    Path(Path&&) noexcept = default;
    Path& operator=(const Path&) = default;
    Path& operator=(Path&&) noexcept = default;

public:

    explicit Path();

    ~Path() noexcept = default;

public:

    operator const char* () const noexcept;
    operator const std::string& () const noexcept;

    template<typename StringType>
    Path& operator / (StringType&& rPath)
    {
        return this->pathString += '\\',
               this->pathString += std::forward<StringType>(rPath),
              *this;
    }

    template<typename StringType>
    Path& operator + (StringType&& cName)
    {
        return this->pathString += std::forward<StringType>(cName),
              *this;
    }

private:

    std::string pathString;

};

using PathPtr = std::unique_ptr<Path>;
#define MakePath std::make_unique<Path>
