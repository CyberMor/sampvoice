/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <memory>

#include <Windows.h>

class Resource {
private:

	HGLOBAL resource = NULL;
	LPVOID dataPtr = NULL;
	DWORD dataSize = NULL;

public:

	Resource() = delete;
	Resource(const Resource& object) = delete;
	Resource(Resource&& object) = delete;

	Resource& operator=(const Resource& object) = delete;
	Resource& operator=(Resource&& object) = delete;

	Resource(
		const HMODULE hModule,
		const DWORD rId,
		const char* rType
	);

	LPVOID GetDataPtr() const;
	DWORD GetDataSize() const;

	~Resource();

};

typedef std::shared_ptr<Resource> ResourcePtr;
#define MakeResource std::make_shared<Resource>
