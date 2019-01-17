#ifndef _SV_HEADER_H_
#define _SV_HEADER_H_

#include <map>
#include <list>
#include <queue>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <forward_list>
#include <iostream>
#include <stdint.h>
#include <time.h>

#ifndef _WIN32
#include <alloca.h>
#include <cstring>
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif

#include "audio/bass.h"
#ifdef _WIN32
#pragma comment(lib, "sdk/audio/bass")
#endif

#include "memory.hpp"

#include <pawn/amx/amx.h>
#include <pawn/plugincommon.h>

#include <raknet/bitstream.h>
#include <raknet/networktypes.h>

#include <ysf/globals.h>

#include "sv_logger.h"

#define SV_CURRENT_VERSION						2u				// Текущая версия плагина
#define SV_CURRENT_VERSION_TEXT					"v2.1"			// Текущая версия плагина в текстовом формате
#define SV_NET_CONNECT_SIGNATURE				0xABCDDCBAu		// Сигнатура заголовка RPC_25
#define SV_NET_PACKET_ID						222u			// Номер пакета плагина
#define SV_NET_PACKET_SIZE						512u			// Размер аудиоданных в одном сетевом пакете (рекомендуется не ставить больше чем 512)

#define SV_SETTINGS_DEFAULT_BITRATE				6000u			// Битрейт по умолчанию
#define SV_SETTINGS_DEFAULT_FREQUENCY			48000u			// Частота звука по умолчанию
#define SV_SETTINGS_DEFAULT_VOICERATE			100u			// Длительность кадра по умолчанию
#define SV_SETTINGS_DEFAULT_FACTORDISTANCE		1.0f			// Фактор дистанции звука по умолчанию
#define SV_SETTINGS_DEFAULT_FACTORROLLOFF		1.0f			// Фактор затухания звука по умолчанию
#define SV_SETTINGS_DEFAULT_FACTORDOPPLER		1.0f			// Фактор эффекта Допплера по умолчанию

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
		stream_stop_audio,			// Пакет остановки воспроизведения
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

// Информация об игроке
struct st_player_info {
	uint8_t sv_version;		// Версия плагина игрока
	bool has_micro;			// Статус наличия микрофона
};

// Пакеты плагина
namespace sv_packet {
#pragma pack(push, 1)

	// Пакет подключения
	struct connect {
	public:
		const uint32_t signature;		// Сигнатура плагина
		uint8_t version;				// Версия плагина
		bool has_micro;					// Наличие микрофона
	};

	// Заголовок пакета
	struct header {
	private:
		const uint8_t samp_packet_id;	// Константный номер пакета samp
	public:
		uint8_t packet_id;				// Номер пакета
		uint8_t data[];					// Содержимое пакета
		template<class st_type> inline st_type* get() {
			return reinterpret_cast<st_type*>(this->data);
		}
	};

	// Пакет инициализации
	struct init {
		uint32_t bitrate;				// Звуковой трафик
		uint16_t frequency;				// Частота звука (8000/12000/16000/24000/48000)
		uint8_t voice_rate;				// Длительность звукового кадра (40/60/80/100/120)
		float factor_distance;			// Фактор дистанции звука
		float factor_rolloff;			// Фактор затухания звука (0 - 10.0)
		float factor_doppler;			// Фактор эффекта Допплера (0 - 10.0)
	};

	// Пакет клавиши активации
	struct set_key {
		uint8_t key_id;					// Virtual Key Code
	};

	// Голосовой пакет
	struct voice {
		uint32_t stream;				// Дескриптор потока	[output]
		uint16_t volume;				// Уровень сигнала		[input]
		uint8_t data[];					// Аудиоданные			[input]
	};

	// Пакет чувствительности микрофона
	struct record_volume {
		float volume;					// Усиление микрофона [0 - 1.0]
	};

	// Пакет параметра
	struct stream_set_parameter {
		uint32_t stream;				// Дескриптор потока
		uint8_t param_id;				// Номер параметра
		float value;					// Значение параметра
	};

	// Пакет скольжения параметра
	struct stream_slide_parameter {
		uint32_t stream;				// Дескриптор потока
		uint8_t param_id;				// Номер параметра
		float start_value;				// Начальное значение
		float end_value;				// Конечное значение
		uint32_t time;					// Время скольжения
	};

	// Пакет воспроизведения внутреннего звука
	struct stream_play_audio_internal {
		uint32_t stream;				// Дескриптор потока
		uint32_t offset;				// Смещение файла
		uint32_t index;					// Индекс файла
		bool loop;						// Статус повторения
	};

	// Пакет воспроизведения внешнего звука
	struct stream_play_audio_external {
		uint32_t stream;				// Дескриптор потока
		uint32_t offset;				// Смещение файла
		bool loop;						// Статус повторения
		char url[];						// URL звукового файла
	};

	// Пакет остановки воспроизведения
	struct stream_stop_audio {
		uint32_t stream;				// Дескриптор потока
	};

	// Пакет удаления потока
	struct stream_delete {
		uint32_t stream;				// Дескриптор потока
	};

	// Пакет глобального потока
	struct gstream_create {
		uint32_t stream;				// Дескриптор потока
	};

	// Пакет параметров локального статического потока
	struct slstream_params {
		uint32_t stream;				// Дескриптор потока
		float position[3];				// Позиция
		float orientation[3];			// Направление
		float velocity[3];				// Скорость
	};

	// Пакет создания локального динамического потока
	struct dlstream_create {
		uint32_t stream;				// Дескриптор потока
		uint16_t target;				// ID цели привязки
	};

	// Пакет создания эффекта
	struct effect_create {
		uint32_t stream;				// Дескриптор потока
		uint32_t effect;				// Дескриптор эффекта
		uint8_t effect_id;				// Номер эффекта
		int priority;					// Приоритет эффекта
		uint8_t parameters[];			// Параметры эффекта
	};

	// Пакет удаления эффекта
	struct effect_delete {
		uint32_t stream;				// Дескриптор потока
		uint32_t effect;				// Дескриптор эффекта
	};

	// Пакет регистрации звука
	struct sound_register {
		uint32_t index;					// Индекс звука
		char url[];						// URL файла + '\0'
	};

	// Пакет запроса звука
	struct sound_request {
		uint32_t index;					// Индекс звука
	};

#pragma pack(pop)
}

// Настройки плагина
sv_packet::init settings = {
	SV_SETTINGS_DEFAULT_BITRATE,
	SV_SETTINGS_DEFAULT_FREQUENCY,
	SV_SETTINGS_DEFAULT_VOICERATE,
	SV_SETTINGS_DEFAULT_FACTORDISTANCE,
	SV_SETTINGS_DEFAULT_FACTORROLLOFF,
	SV_SETTINGS_DEFAULT_FACTORDOPPLER
};

// Дистанция зоны стрима
#ifdef _WIN32
float &stream_distance = *(float*)(0x4E588C);	// Win32 0.3.7
#else
float &stream_distance = *(float*)(0x8197DD8);	// Linux x86 0.3.7
#endif

// Адрес сервера
struct st_server_info {
	uint32_t ip;
	uint16_t port;
};

// Получить адрес сервера
static inline st_server_info* get_server_info() {
#ifdef _WIN32
	return (st_server_info*)((uint8_t*)(pNetGame->pRak) + 0x240);	// Win32 0.3.7
#else
	return (st_server_info*)((uint8_t*)(pNetGame->pRak) + 0x231);	// Linux x86 0.3.7
#endif
}

// Обёртка над BitStream
class bitstream : public BitStream {
public:

	bitstream(
		uint8_t packet_id
	) {
		this->Write((uint8_t)(SV_NET_PACKET_ID));
		this->Write(packet_id);
	}

	bitstream(
		uint8_t packet_id,
		const void *data_ptr,
		uint32_t data_size
	) : bitstream(packet_id) {
		this->Write((const char*)(data_ptr), data_size);
	}

};

// Объявление audio
namespace audio {
	class receiver;
	class group;
	namespace effects {
		class effectable;
		class effect;
		class effect_group;
		class chorus;
		class compressor;
		class distortion;
		class echo;
		class flanger;
		class gargle;
		class i3dl2reverb;
		class parameq;
		class reverb;
	}
	namespace sounds {
		class store;
		class sound;
	}
	namespace streams {
		class streamable;
		class stream;
		class stream_group;
		class stream_static;
		class stream_dynamic;
		class stream_static_global;
		class stream_static_local_at_point;
		class stream_static_local_at_vehicle;
		class stream_static_local_at_player;
		class stream_static_local_at_object;
		class stream_dynamic_local_at_point;
		class stream_dynamic_local_at_vehicle;
		class stream_dynamic_local_at_player;
		class stream_dynamic_local_at_object;
	}
	class buffer;
}

#endif