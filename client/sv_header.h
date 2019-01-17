#pragma once

#include <stdint.h>
#include <windows.h>

#include <game/common.h>
#include <game/CPools.h>
#include <game/CMenuManager.h>

// Цвета сообщений в цифровом формате
#define SV_COLOR_ERROR				0xff9a3519		// Красный шрифт
#define SV_COLOR_INFO				0xffd6cb00		// Жёлтый шрифт
#define SV_COLOR_SUCCESS			0xff5cb300		// Зелёный шрифт
#define SV_COLOR_DEBUG				0xff1f80c1		// Синий шрифт

// Цвета сообщений в текстовом формате
#define SV_COLOR_STR_ERROR			"9a3519"
#define SV_COLOR_STR_INFO			"d6cb00"
#define SV_COLOR_STR_SUCCESS		"5cb300"
#define SV_COLOR_STR_DEBUG			"1f80c1"

// Команды плагина
#define SV_COMMAND_HELP				"svhelp"
#define SV_COMMAND_CHANGE			"svchange"
#define SV_COMMAND_REJECT			"svreject"
#define SV_COMMAND_STORE			"svstore"
#define SV_COMMAND_DEBUG			"svdebug"

// Параметры плагина
#define SV_CURRENT_VERSION			2u				// Текущая версия плагина
#define SV_NET_PACKET_SIZE			512				// Размер аудиоданных в одном сетевом пакете (рекомендуется не ставить больше чем 512)
#define SV_NET_CONNECT_SIGNATURE	0xABCDDCBA		// Сигнатура плагина добавляемая в конец RPC_25
#define SV_NET_PACKET_ID			222				// Номер пакета плагина
#define SV_AUDIO_NET_BUFFER			16384			// Размер буфера для загрузки аудиофайлов
#define SV_RENDER_PADDING_MICRO_X	25				// Смещение иконки микрофона от правой границы экрана
#define SV_RENDER_PADDING_MICRO_Y	25				// Смещение иконки микрофона от нижней границы экрана

// Названия объектов плагина
#define SV_NAME_SVDIR				"sampvoice"		// Имя основной директории
#define SV_NAME_STORE				"store"			// Имя директории хранилища
#define SV_NAME_STORE_TABLE			"store_table"	// Имя файла таблицы хранилища
#define SV_NAME_CONFIG				"sv_config"		// Имя файла настроек
#define SV_NAME_LOG					"sv_log.txt"	// Имя файла логов
#define SV_NAME_PREFIX				"sound_"		// Префикс звуковых файлов

// Полные пути к объектам плагина
#define SV_PATH_SVDIR				"./" SV_NAME_SVDIR
#define SV_PATH_STORE				SV_PATH_SVDIR "/" SV_NAME_STORE
#define SV_PATH_STORE_TABLE			SV_PATH_STORE "/" SV_NAME_STORE_TABLE
#define SV_PATH_CONFIG				SV_PATH_SVDIR "/" SV_NAME_CONFIG
#define SV_PATH_LOG					SV_PATH_SVDIR "/" SV_NAME_LOG

// Номера пакетов
struct e_packet_id {
	enum : uint8_t {

		init,						// Пакет инициализации
		set_key,					// Пакет установки клавиши активации
		voice,						// Голосовой пакет

		record_start,				// Пакет активации микрофона
		record_stop,				// Пакет деактивации микрофона
		record_volume,				// Пакет установки чувствительности микрофона

		effect_create,				// Пакет создания звукового эффекта
		effect_delete,				// Пакет удаления звукового эффекта

		stream_set_parameter,		// Пакет установки параметра потока
		stream_slide_parameter,		// Пакет скольжения параметра потока
		stream_play_sound_internal,	// Пакет воспроизведения внутреннего звука
		stream_play_sound_external,	// Пакет воспроизведения внешнего звука
		stream_stop_sound,			// Пакет остановки воспроизведения
		stream_delete,				// Пакет удаления потока

		gstream_create,				// Пакет создания глобального потока

		lstream_update,				// Пакет обновления локального статического потока
		lstream_create_at_point,	// Пакет создания локального статического потока на точку
		lstream_create_at_vehicle,	// Пакет создания локального динамического потока с привязкой к автомобилю
		lstream_create_at_player,	// Пакет создания локального динамического потока с привязкой к игроку
		lstream_create_at_object,	// Пакет создания локального динамического потока с привязкой к объекту

		sound_register,				// Пакет регистрации звука	<---| server
		sound_request,				// Пакет запроса звука		--->| server

		enum_end

	};
};

// Номера эффектов
struct e_effect_id {
	enum : uint8_t {

		chorus,						// Звуковой эффект "chorus"
		compressor,					// Звуковой эффект "compressor"
		distortion,					// Звуковой эффект "distortion"
		echo,						// Звуковой эффект "echo"
		flanger,					// Звуковой эффект "flanger"
		gargle,						// Звуковой эффект "gargle"
		i3dl2reverb,				// Звуковой эффект "i3dl2reverb"
		parameq,					// Звуковой эффект "parameq"
		reverb,						// Звуковой эффект "reverb"

		enum_end

	};
};

// Номера параметров
struct e_sparam_id {
	enum : uint8_t {

		frequency	= 1,	// Частота звука
		volume		= 2,	// Громкость звука
		panning		= 3,	// Панорамирование звука
		eaxmix		= 4,	// Объёмность звука
		src			= 8,	// Качество звука

		enum_end

	};
};

// Пакеты плагина
namespace sv_packet {
#pragma pack(push, 1)

	// Пакет подключения
	struct connect {
	private:
		const uint32_t signature = SV_NET_CONNECT_SIGNATURE;
		uint8_t version = SV_CURRENT_VERSION;
	public:
		bool has_micro;	// Наличие микрофона
	};

	// Заголовок пакета
	struct header {
	public:
		uint8_t samp_packet_id = SV_NET_PACKET_ID;
		uint8_t packet_id;	// Номер пакета плагина
		uint8_t data[];		// Содержимое пакета
		template<class st_type> inline st_type* get() {
			return reinterpret_cast<st_type*>(this->data);
		}
	};

	// Пакет инициализации
	struct init {
		uint32_t bitrate;		// Звуковой трафик
		uint16_t frequency;		// Частота звука (8000/12000/16000/24000/48000)
		uint8_t voice_rate;		// Длительность звукового кадра (40/60/80/100/120)
		float factor_distance;	// Фактор дистанции звука
		float factor_rolloff;	// Фактор затухания звука (0 - 10.0)
		float factor_doppler;	// Фактор эффекта Допплера (0 - 10.0)
	};

	// Пакет клавиши активации
	struct set_key {
		uint8_t key_id;			// Virtual Key Code
	};

	// Голосовой пакет
	struct voice {
		uint32_t stream;		// Дескриптор потока	[output]
		uint16_t volume;		// Уровень сигнала		[input]
		uint8_t data[];			// Аудиоданные			[input]
	};

	// Пакет чувствительности микрофона
	struct record_volume {
		float volume;			// Усиление микрофона [0 - 1.0]
	};

	// Пакет параметра
	struct stream_set_parameter {
		uint32_t stream;		// Дескриптор потока
		uint8_t param_id;		// Номер параметра
		float value;			// Значение параметра
	};

	// Пакет скольжения параметра
	struct stream_slide_parameter {
		uint32_t stream;		// Дескриптор потока
		uint8_t param_id;		// Номер параметра
		float start_value;		// Начальное значение
		float end_value;		// Конечное значение
		uint32_t time;			// Время скольжения
	};

	// Пакет воспроизведения внутреннего звука
	struct stream_play_audio_internal {
		uint32_t stream;		// Дескриптор потока
		uint32_t offset;		// Смещение файла
		uint32_t index;			// Индекс файла
		bool loop;				// Статус повторения
	};

	// Пакет воспроизведения внешнего звука
	struct stream_play_audio_external {
		uint32_t stream;		// Дескриптор потока
		uint32_t offset;		// Смещение файла
		bool loop;				// Статус повторения
		char url[];				// URL звукового файла
	};

	// Пакет остановки воспроизведения
	struct stream_stop_audio {
		uint32_t stream;		// Дескриптор потока
	};

	// Пакет удаления потока
	struct stream_delete {
		uint32_t stream;		// Дескриптор потока
	};

	// Пакет глобального потока
	struct gstream_create {
		uint32_t stream;		// Дескриптор потока
	};

	// Пакет параметров локального статического потока
	struct slstream_params {
		uint32_t stream;		// Дескриптор потока
		float position[3];		// Позиция
		float orientation[3];	// Направление
		float velocity[3];		// Скорость
	};

	// Пакет создания локального динамического потока
	struct dlstream_create {
		uint32_t stream;		// Дескриптор потока
		uint16_t target;		// ID цели привязки
	};

	// Пакет создания эффекта
	struct effect_create {
		uint32_t stream;		// Дескриптор потока
		uint32_t effect;		// Дескриптор эффекта
		uint8_t effect_id;		// Номер эффекта
		int priority;			// Приоритет эффекта
		uint8_t parameters[];	// Параметры эффекта
	};

	// Пакет удаления эффекта
	struct effect_delete {
		uint32_t stream;		// Дескриптор потока
		uint32_t effect;		// Дескриптор эффекта
	};

	// Пакет регистрации звука
	struct sound_register {
		uint32_t index;			// Индекс звука в таблице
		char url[];				// URL файла + '\0'
	};

	// Пакет запроса звука
	struct sound_request {
		uint32_t index;			// Индекс звука в таблице
	};

#pragma pack(pop)
}

// Структуры плагина
namespace sv_structs {
#pragma pack(push, 1)

	// Структура настроек
	struct settings {
		int icon_x, icon_y;		// Позиция иконки микрофона
		uint32_t store_limit;	// Максимальный размер хранилища
		bool reject_status;		// Статус запрета активации микрофона
		bool debug_status;		// Статус отладки
	};

#pragma pack(pop)
}

// Настройки плагина
class settings {
private:

	static sv_structs::settings c_settings;

public:

	static inline sv_structs::settings* get_settings() {
		return &settings::c_settings;
	}

	static inline bool load(
		const char *filename
	) {
		if (FILE *file = fopen(filename, "rb"))
			return fread(&settings::c_settings, sizeof(settings::c_settings), 1, file);
		return false;
	}

	static inline bool save(
		const char *filename
	) {
		if (FILE *file = fopen(filename, "wb"))
			return fwrite(&settings::c_settings, sizeof(settings::c_settings), 1, file);
		return false;
	}

};
sv_structs::settings settings::c_settings;

// Общие функции
namespace util {

	// Дескриптор плагина
	static HMODULE h_module;

	// Загрузить ресурс
	static bool load_resource(
		const uint32_t rId,		// [in]
		const char *rType,		// [in]
		HGLOBAL *pResource,		// [out]
		void **ppAddress,		// [out]
		uint32_t *pSize			// [out]
	) {
		HRSRC res;
		if ((res = FindResource(h_module, MAKEINTRESOURCE(rId), rType)) &&
			(*pResource = LoadResource(h_module, res)) &&
			(*ppAddress = LockResource(*pResource)) &&
			(*pSize = SizeofResource(h_module, res))
		) return true;
		return false;
	}

	// Выгрузить ресурс
	static inline void free_resource(
		HGLOBAL resource
	) {
		FreeResource(resource);
	}

	// Нажата ли клавиша
	static inline bool is_key_down(
		uint8_t key_id
	) {
		return GetKeyState(key_id) & 0x80;
	}

	// Получить статус игры
	static inline bool is_active() {
		return *(uint8_t*)(0x8D621C) && !FrontEndMenuManager.m_bMenuActive;
	}

	// Получить ширину экрана
	static inline int get_width_screen() {
		return GetSystemMetrics(SM_CXSCREEN);
	}

	// Получить высоту экрана
	static inline int get_height_screen() {
		return GetSystemMetrics(SM_CYSCREEN);
	}

}
