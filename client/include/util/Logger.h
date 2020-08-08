/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#pragma once

#include <mutex>
#include <iostream>
#include <ctime>

#include <d3d9.h>

#include "Samp.h"

#define strerrorcolor   "e65f39"
#define strinfocolor    "ffe0a5"
#define strsuccesscolor "6bbf17"
#define strdebugcolor   "6bbaed"

constexpr DWORD errorcolor   = 0xffe65f39;
constexpr DWORD infocolor    = 0xffffe0a5;
constexpr DWORD successcolor = 0xff6bbf17;
constexpr DWORD debugcolor   = 0xff6bbaed;

class Logger {
public:

	static constexpr DWORD MaxLengthMessageToChat = 144;

private:

	static FILE* logFile;

	static std::mutex logFileMutex;
	static std::mutex logChatMutex;

public:

	static bool Init(const char* fileName);

	template<class... ARGS>
	static inline void LogToFile(
		const char* message,
		const ARGS... args
	) {

		const std::scoped_lock<std::mutex> lock(Logger::logFileMutex);

		if (!Logger::logFile) return;

		const auto cTime = time(nullptr);
		const auto timeOfDay = localtime(&cTime);

		if (!timeOfDay) return;

		fprintf(
			Logger::logFile,
			"[%.2d:%.2d:%.2d] : ",
			timeOfDay->tm_hour,
			timeOfDay->tm_min,
			timeOfDay->tm_sec
		);

		fprintf(
			Logger::logFile,
			message, args...
		);

		fputc('\n', Logger::logFile);
		fflush(Logger::logFile);

	}

	template<class... ARGS>
	static inline void LogToChat(
		const D3DCOLOR color,
		const char* message,
		const ARGS... args
	) {

		char messageBuffer[MaxLengthMessageToChat];

		if (_snprintf(messageBuffer, sizeof(messageBuffer), message, args...) != -1) {

			const std::scoped_lock lock(Logger::logChatMutex);

			Samp::AddMessageToChat(color, messageBuffer);

		} else {

			Logger::LogToFile("[inf:logger:logtochat] : message is too long to display in chat");

		}

	}

	template<class... ARGS>
	static inline void Log(
		const D3DCOLOR color,
		const char* message,
		const ARGS... args
	) {

		Logger::LogToFile(message, args...);
		Logger::LogToChat(color, message, args...);

	}

	static void Free();

};
