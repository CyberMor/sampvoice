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

#include <ysf/globals.h>

class Logger {
private:

	static FILE* logFile;
	static logprintf_t logFunc;

	static std::mutex logFileMutex;
	static std::mutex logConsoleMutex;

public:

	template<class... ARGS>
	static void LogToFile(const char* message, const ARGS... args) {

		const std::lock_guard<std::mutex> lock(Logger::logFileMutex);

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
	static void LogToConsole(const char* message, const ARGS... args) {

		const std::lock_guard<std::mutex> lock(Logger::logConsoleMutex);

		if (Logger::logFunc) Logger::logFunc(message, args...);

	}

	template<class... ARGS>
	static inline void Log(const char* message, const ARGS... args) {

		Logger::LogToFile(message, args...);
		Logger::LogToConsole(message, args...);

	}

	static bool Init(const char* logFile, const logprintf_t logFunc);

	static void Free();

};
