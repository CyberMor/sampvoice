/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Path.h"

#include <ShlObj.h>

#include "Logger.h"

Path::Path() {

	char myDocumentsPath[MAX_PATH] = {};

	if (const HRESULT hResult = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, myDocumentsPath); FAILED(hResult)) {

		Logger::LogToFile("[err:path_%p] : failed to get 'MyDocuments' directory (code:%d)", this, hResult);

		throw std::bad_alloc();

	}

	this->pathString = std::string(myDocumentsPath) + "\\" + "GTA San Andreas User Files" + "\\" + "sampvoice";

	if (const int rCode = SHCreateDirectoryEx(NULL, this->pathString.c_str(), NULL);
		rCode != ERROR_SUCCESS && rCode != ERROR_FILE_EXISTS && rCode != ERROR_ALREADY_EXISTS
	) {

		Logger::LogToFile("[err:path_%p] : failed to create plugin directory (code:%d)", this, rCode);

		throw std::bad_alloc();

	}

}

Path::operator const char* () const {

	return this->pathString.c_str();

}

Path::operator const std::string& () const {

	return this->pathString;

}

Path& Path::operator/(const std::string& rPath) {

	return this->pathString += '\\', this->pathString += rPath, *this;

}

Path& Path::operator+(const std::string& cName) {

	return this->pathString += cName, *this;

}
