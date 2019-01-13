#pragma once

#include <iostream>

#define LogInfo(message, ...) logger::log_info(message, __VA_ARGS__)
#define LogError(module_name, description, ...) logger::log_error(module_name, __func__, description, __VA_ARGS__)
#define LogDebug(message, ...) logger::log_debug(message, __VA_ARGS__)
#define WriteLog(message, ...) logger::log_wo_msg(message, __VA_ARGS__)

namespace logger {

	static FILE *log_file = nullptr;

	// Вывести информацию
	template<class... ARGS>
	static inline void log_info(
		const char *message,
		const ARGS... args
	) {

		char buffer[512];

		snprintf(buffer, sizeof(buffer), "[info] : %s\n", message);

		if (samp::status) samp::add_message_to_chat(SV_COLOR_INFO, buffer, args...);

		if (log_file) {
			fprintf(log_file, buffer, args...);
			fflush(log_file);
		}

	}

	// Вывести ошибку
	template<class... ARGS>
	static inline void log_error(
		const char *module_name,
		const char *method_name,
		const char *description,
		const ARGS... args
	) {

		char buffer[512];

		snprintf(
			buffer, sizeof(buffer),
			"[error] : [%s] : [%s] : %s\n",
			module_name,
			method_name,
			description
		);

		if (samp::status) samp::add_message_to_chat(SV_COLOR_INFO, buffer, args...);

		if (log_file) {
			fprintf(log_file, buffer, args...);
			fflush(log_file);
		}

	}

	// Вывести отладочное сообщение
	template<class... ARGS>
	static inline void log_debug(
		const char *message,
		const ARGS... args
	) {

		char buffer[512];

		snprintf(buffer, sizeof(buffer), "[debug] : %s\n", message);

		if (samp::status) samp::add_message_to_chat(SV_COLOR_DEBUG, buffer, args...);

		if (log_file) {
			fprintf(log_file, buffer, args...);
			fflush(log_file);
		}

	}

	// Записать сообщение в файл (без вывода на консоль)
	template<class... ARGS>
	static inline void log_wo_msg(
		const char *message,
		const ARGS... args
	) {
		
		if (log_file) {
			char buffer[512];
			snprintf(buffer, sizeof(buffer), "[debug] : %s\n", message);
			fprintf(log_file, buffer, args...);
			fflush(log_file);
		}

	}

	// Инициализировать логгер
	static inline bool init() {

		if (!log_file) return log_file = fopen(SV_PATH_LOG, "wt");
		else return false;

	}

	// Выгрузить логгер
	static inline void free() {

		if (log_file) {
			fclose(log_file);
			log_file = nullptr;
		}

	}

}
