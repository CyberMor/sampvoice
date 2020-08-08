/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Resource.h"

#include <assert.h>

#include "Logger.h"

Resource::Resource(
	const HMODULE hModule,
	const DWORD rId,
	const char* rType
) {

	assert(hModule);

	const HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(rId), rType);

	if (!hRsrc) {

		Logger::LogToFile("[err:resource_%p] : failed to find resource (code:%u)", this, GetLastError());

		throw std::bad_alloc();

	}

	if (!(this->resource = LoadResource(hModule, hRsrc))) {

		Logger::LogToFile("[err:resource_%p] : failed to load resource handle (code:%u)", this, GetLastError());

		throw std::bad_alloc();

	}

	if (!(this->dataPtr = LockResource(this->resource))) {

		Logger::LogToFile("[err:resource_%p] : failed to get data pointer (code:%u)", this, GetLastError());

		FreeResource(this->resource);

		throw std::bad_alloc();

	}

	if (!(this->dataSize = SizeofResource(hModule, hRsrc))) {

		Logger::LogToFile("[err:resource_%p] : failed to get data size (code:%u)", this, GetLastError());

		FreeResource(this->resource);

		throw std::bad_alloc();

	}

}

LPVOID Resource::GetDataPtr() const {

	return this->dataPtr;

}

DWORD Resource::GetDataSize() const {

	return this->dataSize;

}

Resource::~Resource() {

	FreeResource(this->resource);

}
