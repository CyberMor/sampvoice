/*
	This is a SampVoice project file
	Developer: CyberMor <cyber.mor.2020@gmail.ru>

	See more here https://github.com/CyberMor/sampvoice

	Copyright (c) Daniel (CyberMor) 2020 All rights reserved
*/

#include "Logger.h"

FILE* Logger::logFile(nullptr);

std::mutex Logger::logFileMutex;
std::mutex Logger::logChatMutex;

bool Logger::Init(const char* fileName) {

	const std::scoped_lock lock(Logger::logFileMutex);

	return !Logger::logFile && (Logger::logFile = fopen(fileName, "wt"));

}

void Logger::Free() {

	const std::scoped_lock lock(Logger::logFileMutex);

	if (Logger::logFile) {

		fclose(Logger::logFile);
		Logger::logFile = nullptr;

	}

}
