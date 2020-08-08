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
private:

	std::string pathString;

public:

	Path();

	operator const char* () const;
	operator const std::string& () const;

	Path& operator/(const std::string& rPath);
	Path& operator+(const std::string& cName);

};

typedef std::shared_ptr<Path> PathPtr;
#define MakePath std::make_shared<Path>
