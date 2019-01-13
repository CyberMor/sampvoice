#ifndef _SV_LOGGER_H_
#define _SV_LOGGER_H_

#include "sv_header.h"

#define LogInfo(message, ...) logger::log_info(message, ## __VA_ARGS__)
#define LogError(name, description, ...) logger::log_error(name, __func__, description, ## __VA_ARGS__)
#define LogDebug(message, ...) logger::log_debug(message, ## __VA_ARGS__)

namespace logger {

	static FILE *log_file = nullptr;
	static logprintf_t log_func = nullptr;

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
			"[sampvoice] : [error] : [%s] : [%s] : %s",
			module_name,
			method_name,
			description
		);

		if (log_func) log_func(buffer, args...);

		if (log_file) {
			fprintf(log_file, buffer, args...);
			fputc('\n', log_file);
			fflush(log_file);
		}

	}

	// Вывести информацию
	template<class... ARGS>
	static inline void log_info(
		const char *message,
		const ARGS... args
	) {
		
		char buffer[512];

		snprintf(buffer, sizeof(buffer), "[sampvoice] : [info] : %s", message);

		if (log_func) log_func(buffer, args...);

		if (log_file) {
			fprintf(log_file, buffer, args...);
			fputc('\n', log_file);
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

		snprintf(buffer, sizeof(buffer), "[sampvoice] : [debug] : %s", message);

		if (log_func) log_func(buffer, args...);

		if (log_file) {
			fprintf(log_file, buffer, args...);
			fputc('\n', log_file);
			fflush(log_file);
		}

	}

	// Инициализировать логгер
	static inline bool init(
		logprintf_t log_func = nullptr
	) {

		if (!log_file) {
			logger::log_func = log_func;
			return log_file = fopen("sampvoice_log.txt", "wt");
		}
		
		else return false;

	}

	// Выгрузить логгер
	static inline void free() {

		if (log_file) {
			fclose(log_file);
			log_file = nullptr;
			log_func = nullptr;
		}

	}

}

#endif