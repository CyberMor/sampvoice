#ifndef _SERVER_H_
#define _SERVER_H_

#include "sv_header.h"

namespace pawn {

	static constexpr bool debug_mode = false;

	class handler_interface {
	public:

		// Основное
		virtual bool init(uint32_t bitrate, uint16_t frequency, uint8_t voice_rate, float factor_distance, float factor_rolloff, float factor_doppler) = 0;
		virtual uint8_t get_version(uint16_t player_id) = 0;
		virtual void set_key(uint16_t player_id, uint8_t key_id) = 0;
		virtual void send_packet(Packet *packet, audio::receiver *target) = 0;

		// Микрофон
		virtual bool has_micro(uint16_t player_id) = 0;
		virtual void record_start(uint16_t player_id) = 0;
		virtual void record_stop(uint16_t player_id) = 0;
		virtual void record_volume(uint16_t player_id, float volume) = 0;

		// Эффект
		virtual audio::effects::chorus* effect_create_chorus(int priority, float wetdrymix, float depth, float feedback, float frequency, uint32_t waveform, float delay, uint32_t phase) = 0;
		virtual audio::effects::compressor* effect_create_compressor(int priority, float gain, float attack, float release, float threshold, float ratio, float predelay) = 0;
		virtual audio::effects::distortion* effect_create_distortion(int priority, float gain, float edge, float posteqcenterfrequency, float posteqbandwidth, float prelowpasscutoff) = 0;
		virtual audio::effects::echo* effect_create_echo(int priority, float wetdrymix, float feedback, float leftdelay, float rightdelay, bool pandelay) = 0;
		virtual audio::effects::flanger* effect_create_flanger(int priority, float wetdrymix, float depth, float feedback, float frequency, uint32_t waveform, float delay, uint32_t phase) = 0;
		virtual audio::effects::gargle* effect_create_gargle(int priority, uint32_t ratehz, uint32_t waveshape) = 0;
		virtual audio::effects::i3dl2reverb* effect_create_i3dl2reverb(int priority, int room, int roomhf, float roomrollofffactor, float decaytime, float decayhfratio, int reflections, float reflectionsdelay, int reverb, float reverbdelay, float diffusion, float density, float hfreference) = 0;
		virtual audio::effects::parameq* effect_create_parameq(int priority, float center, float bandwidth, float gain) = 0;
		virtual audio::effects::reverb* effect_create_reverb(int priority, float ingain, float reverbmix, float reverbtime, float highfreqrtratio) = 0;
		virtual void effect_delete(audio::effects::effect* effect) = 0;

		// Интерфейс потока
		virtual void stream_set_parameter(audio::streams::streamable* target, uint8_t param_id, float value) = 0;
		virtual void stream_slide_parameter(audio::streams::streamable* target, uint8_t param_id, float start_value, float end_value, uint32_t time) = 0;
		virtual void stream_effect_attach(audio::streams::streamable *target, audio::effects::effectable* effect) = 0;
		virtual void stream_effect_detach(audio::streams::streamable *target, audio::effects::effectable* effect) = 0;
		virtual bool stream_play_sound_internal(audio::streams::streamable *target, uint32_t index, bool loop) = 0;
		virtual bool stream_play_sound_external(audio::streams::streamable *target, const char *url, bool loop) = 0;
		virtual void stream_stop_sound(audio::streams::streamable *target) = 0;
		virtual void stream_player_attach(audio::streams::streamable* target, uint16_t player_id) = 0;
		virtual void stream_player_detach(audio::streams::streamable* target, uint16_t player_id) = 0;
		virtual void stream_delete(audio::streams::stream* stream) = 0;

		// Статический глобальный поток
		virtual audio::streams::stream_static_global* sgstream_create() = 0;

		// Статический локальный поток
		virtual void slstream_update(audio::streams::stream_static_local_at_point* slstream, float pos_x, float pos_y, float pos_z, float ornt_x, float ornt_y, float ornt_z, float vel_x, float vel_y, float vel_z) = 0;
		virtual audio::streams::stream_static_local_at_point* slstream_create_at_point(float pos_x, float pos_y, float pos_z, float ornt_x, float ornt_y, float ornt_z, float vel_x, float vel_y, float vel_z) = 0;
		virtual audio::streams::stream_static_local_at_vehicle* slstream_create_at_vehicle(uint16_t vehicle_id) = 0;
		virtual audio::streams::stream_static_local_at_player* slstream_create_at_player(uint16_t player_id) = 0;
		virtual audio::streams::stream_static_local_at_object* slstream_create_at_object(uint16_t object_id) = 0;

		// Динамический локальный поток
		virtual audio::streams::stream_dynamic_local_at_point* dlstream_create_at_point(float pos_x, float pos_y, float pos_z, float ornt_x, float ornt_y, float ornt_z, float vel_x, float vel_y, float vel_z) = 0;
		virtual audio::streams::stream_dynamic_local_at_vehicle* dlstream_create_at_vehicle(uint16_t vehicle_id) = 0;
		virtual audio::streams::stream_dynamic_local_at_player* dlstream_create_at_player(uint16_t player_id) = 0;
		virtual audio::streams::stream_dynamic_local_at_object* dlstream_create_at_object(uint16_t object_id) = 0;

		// Интерфейс группы
		virtual void group_add(audio::group *group, void *element) = 0;
		virtual void group_remove(audio::group *group, void *element) = 0;
		virtual void group_delete(audio::group *group) = 0;

		// Группа потоков
		virtual audio::streams::stream_group* sgroup_create() = 0;

		// Группа эффектов
		virtual audio::effects::effect_group* egroup_create() = 0;

		// Звуковой буфер
		virtual audio::buffer* buffer_create() = 0;
		virtual void buffer_clear(audio::buffer *buffer) = 0;
		virtual bool buffer_save(audio::buffer *buffer, const char *filename) = 0;
		virtual bool buffer_load(audio::buffer *buffer, const char *filename) = 0;
		virtual void buffer_attach(audio::buffer *buffer, audio::streams::streamable *target) = 0;
		virtual void buffer_detach(audio::buffer *buffer, audio::streams::streamable *target) = 0;
		virtual void buffer_detach_all(audio::buffer *buffer) = 0;
		virtual void buffer_play(audio::buffer *buffer, bool loop) = 0;
		virtual void buffer_stop(audio::buffer *buffer) = 0;
		virtual void buffer_delete(audio::buffer *buffer) = 0;

		// Хранилище звуков
		virtual uint32_t sound_register(const char *url) = 0;

	};

	static handler_interface *handler = nullptr;

	namespace natives {



		// Инициализировать настройки плагина
		static cell AMX_NATIVE_CALL n_sv_init(
			AMX *amx, cell *params
		) {

			if (params[0] != (6 * sizeof(cell))) return false;

			const uint32_t bitrate = static_cast<uint32_t>(params[1]);
			const uint16_t frequency = static_cast<uint16_t>(params[2]);
			const uint8_t voice_rate = static_cast<uint8_t>(params[3]);
			const float factor_distance = amx_ctof(params[4]);
			const float factor_rolloff = amx_ctof(params[5]);
			const float factor_doppler = amx_ctof(params[6]);

			if (debug_mode) {
				const bool result = handler->init(bitrate, frequency, voice_rate, factor_distance, factor_rolloff, factor_doppler);
				LogDebug("[%s] : bitrate(%u), frequency(%hu), voice_rate(%hhu), factor_distance(%f), factor_rolloff(%f), factor_doppler(%f) : return(%s)", __FUNCTION__, bitrate, frequency, voice_rate, factor_distance, factor_rolloff, factor_doppler, (result ? "true" : "false"));
				return result;
			}

			return handler->init(bitrate, frequency, voice_rate, factor_distance, factor_rolloff, factor_doppler);

		}

		// Получить версию клиентского плагина
		static cell AMX_NATIVE_CALL n_sv_get_version(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			const uint16_t player_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) {
				const uint8_t result = handler->get_version(player_id);
				LogDebug("[%s] : player_id(%hu) : return(%hhu)", __FUNCTION__, player_id, result);
				return result;
			}

			return handler->get_version(player_id);

		}

		// Установить клавишу активации микрофона для игрока
		static cell AMX_NATIVE_CALL n_sv_set_key(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			const uint16_t player_id = static_cast<uint16_t>(params[1]);
			const uint8_t key_id = static_cast<uint8_t>(params[2]);

			if (debug_mode) LogDebug("[%s] : player_id(%hu), key_id(%hhX)", __FUNCTION__, player_id, key_id);

			handler->set_key(player_id, key_id);
			return NULL;

		}

		// Передать пакет ресиверу
		static cell AMX_NATIVE_CALL n_sv_send_packet(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			Packet *packet = reinterpret_cast<Packet*>(params[1]);
			audio::receiver *target = reinterpret_cast<audio::receiver*>(params[2]);

			if (debug_mode) LogDebug("[%s] : packet(%p), target(%p)", __FUNCTION__, packet, target);

			handler->send_packet(packet, target);
			return NULL;

		}



		// Проверить наличие микрофона у игрока
		static cell AMX_NATIVE_CALL n_sv_has_micro(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return false;

			const uint16_t player_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) {
				const bool result = handler->has_micro(player_id);
				LogDebug("[%s] : player_id(%hu) : return(%s)", __FUNCTION__, player_id, (result ? "true" : "false"));
				return result;
			}

			return handler->has_micro(player_id);

		}

		// Включить микрофон игрока для записи
		static cell AMX_NATIVE_CALL n_sv_record_start(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			const uint16_t player_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) LogDebug("[%s] : player_id(%hu)", __FUNCTION__, player_id);

			handler->record_start(player_id);
			return NULL;

		}

		// Отключить микрофон игрока
		static cell AMX_NATIVE_CALL n_sv_record_stop(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			const uint16_t player_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) LogDebug("[%s] : player_id(%hu)", __FUNCTION__, player_id);

			handler->record_stop(player_id);
			return NULL;

		}

		// Установить чувствительность микрофона игрока
		static cell AMX_NATIVE_CALL n_sv_record_volume(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			const uint16_t player_id = static_cast<uint16_t>(params[1]);
			const float volume = amx_ctof(params[2]);

			if (debug_mode) LogDebug("[%s] : player_id(%hu), volume(%f)", __FUNCTION__, player_id, volume);

			handler->record_volume(player_id, volume);
			return NULL;

		}



		// Создать звуковой эффект "chorus"
		static cell AMX_NATIVE_CALL n_sv_effect_create_chorus(
			AMX *amx, cell *params
		) {

			if (params[0] != (8 * sizeof(cell))) return NULL;

			const int priority = static_cast<int>(params[1]);
			const float wetdrymix = amx_ctof(params[2]);
			const float depth = amx_ctof(params[3]);
			const float feedback = amx_ctof(params[4]);
			const float frequency = amx_ctof(params[5]);
			const uint32_t waveform = static_cast<uint32_t>(params[6]);
			const float delay = amx_ctof(params[7]);
			const uint32_t phase = static_cast<uint32_t>(params[8]);

			if (debug_mode) {
				const auto result = handler->effect_create_chorus(priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase);
				LogDebug("[%s] : priority(%d), wetdrymix(%f), depth(%f), feedback(%f), frequency(%f), waveform(%u), delay(%f), phase(%u) : return(%p)", __FUNCTION__, priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->effect_create_chorus(priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase));

		}

		// Создать звуковой эффект "compressor"
		static cell AMX_NATIVE_CALL n_sv_effect_create_compressor(
			AMX *amx, cell *params
		) {

			if (params[0] != (7 * sizeof(cell))) return NULL;

			const int priority = static_cast<int>(params[1]);
			const float gain = amx_ctof(params[2]);
			const float attack = amx_ctof(params[3]);
			const float release = amx_ctof(params[4]);
			const float threshold = amx_ctof(params[5]);
			const float ratio = amx_ctof(params[6]);
			const float predelay = amx_ctof(params[7]);

			if (debug_mode) {
				const auto result = handler->effect_create_compressor(priority, gain, attack, release, threshold, ratio, predelay);
				LogDebug("[%s] : priority(%d), gain(%f), attack(%f), release(%f), threshold(%f), ratio(%f), predelay(%f) : return(%p)", __FUNCTION__, priority, gain, attack, release, threshold, ratio, predelay, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->effect_create_compressor(priority, gain, attack, release, threshold, ratio, predelay));

		}

		// Создать звуковой эффект "distortion"
		static cell AMX_NATIVE_CALL n_sv_effect_create_distortion(
			AMX *amx, cell *params
		) {

			if (params[0] != (6 * sizeof(cell))) return NULL;

			const int priority = static_cast<int>(params[1]);
			const float gain = amx_ctof(params[2]);
			const float edge = amx_ctof(params[3]);
			const float posteqcenterfrequency = amx_ctof(params[4]);
			const float posteqbandwidth = amx_ctof(params[5]);
			const float prelowpasscutoff = amx_ctof(params[6]);

			if (debug_mode) {
				const auto result = handler->effect_create_distortion(priority, gain, edge, posteqcenterfrequency, posteqbandwidth, prelowpasscutoff);
				LogDebug("[%s] : priority(%d), gain(%f), edge(%f), posteqcenterfrequency(%f), posteqbandwidth(%f), prelowpasscutoff(%f) : return(%p)", __FUNCTION__, priority, gain, edge, posteqcenterfrequency, posteqbandwidth, prelowpasscutoff, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->effect_create_distortion(priority, gain, edge, posteqcenterfrequency, posteqbandwidth, prelowpasscutoff));

		}

		// Создать звуковой эффект "echo"
		static cell AMX_NATIVE_CALL n_sv_effect_create_echo(
			AMX *amx, cell *params
		) {

			if (params[0] != (6 * sizeof(cell))) return NULL;

			const int priority = static_cast<int>(params[1]);
			const float wetdrymix = amx_ctof(params[2]);
			const float feedback = amx_ctof(params[3]);
			const float leftdelay = amx_ctof(params[4]);
			const float rightdelay = amx_ctof(params[5]);
			const bool pandelay = static_cast<bool>(params[6]);

			if (debug_mode) {
				const auto result = handler->effect_create_echo(priority, wetdrymix, feedback, leftdelay, rightdelay, pandelay);
				LogDebug("[%s] : priority(%d), wetdrymix(%f), feedback(%f), leftdelay(%f), rightdelay(%f), pandelay(%s) : return(%p)", __FUNCTION__, priority, wetdrymix, feedback, leftdelay, rightdelay, (pandelay ? "true" : "false"), result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->effect_create_echo(priority, wetdrymix, feedback, leftdelay, rightdelay, pandelay));

		}

		// Создать звуковой эффект "flanger"
		static cell AMX_NATIVE_CALL n_sv_effect_create_flanger(
			AMX *amx, cell *params
		) {

			if (params[0] != (8 * sizeof(cell))) return NULL;

			const int priority = static_cast<int>(params[1]);
			const float wetdrymix = amx_ctof(params[2]);
			const float depth = amx_ctof(params[3]);
			const float feedback = amx_ctof(params[4]);
			const float frequency = amx_ctof(params[5]);
			const uint32_t waveform = static_cast<uint32_t>(params[6]);
			const float delay = amx_ctof(params[7]);
			const uint32_t phase = static_cast<uint32_t>(params[8]);

			if (debug_mode) {
				const auto result = handler->effect_create_flanger(priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase);
				LogDebug("[%s] : priority(%d), wetdrymix(%f), depth(%f), feedback(%f), frequency(%f), waveform(%u), delay(%f), phase(%u) : return(%p)", __FUNCTION__, priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->effect_create_flanger(priority, wetdrymix, depth, feedback, frequency, waveform, delay, phase));

		}

		// Создать звуковой эффект "gargle"
		static cell AMX_NATIVE_CALL n_sv_effect_create_gargle(
			AMX *amx, cell *params
		) {

			if (params[0] != (3 * sizeof(cell))) return NULL;

			const int priority = static_cast<int>(params[1]);
			const uint32_t ratehz = static_cast<uint32_t>(params[2]);
			const uint32_t waveshape = static_cast<uint32_t>(params[3]);

			if (debug_mode) {
				const auto result = handler->effect_create_gargle(priority, ratehz, waveshape);
				LogDebug("[%s] : priority(%d), ratehz(%u), waveshape(%u) : return(%p)", __FUNCTION__, priority, ratehz, waveshape, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->effect_create_gargle(priority, ratehz, waveshape));

		}

		// Создать звуковой эффект "i3dl2reverb"
		static cell AMX_NATIVE_CALL n_sv_effect_create_i3dl2reverb(
			AMX *amx, cell *params
		) {

			if (params[0] != (13 * sizeof(cell))) return NULL;

			const int priority = static_cast<int>(params[1]);
			const int room = static_cast<int>(params[2]);
			const int roomhf = static_cast<int>(params[3]);
			const float roomrollofffactor = amx_ctof(params[4]);
			const float decaytime = amx_ctof(params[5]);
			const float decayhfratio = amx_ctof(params[6]);
			const int reflections = static_cast<int>(params[7]);
			const float reflectionsdelay = amx_ctof(params[8]);
			const int reverb = static_cast<int>(params[9]);
			const float reverbdelay = amx_ctof(params[10]);
			const float diffusion = amx_ctof(params[11]);
			const float density = amx_ctof(params[12]);
			const float hfreference = amx_ctof(params[13]);

			if (debug_mode) {
				const auto result = handler->effect_create_i3dl2reverb(priority, room, roomhf, roomrollofffactor, decaytime, decayhfratio, reflections, reflectionsdelay, reverb, reverbdelay, diffusion, density, hfreference);
				LogDebug("[%s] : priority(%d), room(%d), roomhf(%d), roomrollofffactor(%f), decaytime(%f), decayhfratio(%f), reflections(%d), reflectionsdelay(%f), reverb(%d), reverbdelay(%f), diffusion(%f), density(%f), hfreference(%f) : return(%p)", __FUNCTION__, priority, room, roomhf, roomrollofffactor, decaytime, decayhfratio, reflections, reflectionsdelay, reverb, reverbdelay, diffusion, density, hfreference, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->effect_create_i3dl2reverb(priority, room, roomhf, roomrollofffactor, decaytime, decayhfratio, reflections, reflectionsdelay, reverb, reverbdelay, diffusion, density, hfreference));

		}

		// Создать звуковой эффект "parameq"
		static cell AMX_NATIVE_CALL n_sv_effect_create_parameq(
			AMX *amx, cell *params
		) {

			if (params[0] != (4 * sizeof(cell))) return NULL;

			const int priority = static_cast<int>(params[1]);
			const float center = amx_ctof(params[2]);
			const float bandwidth = amx_ctof(params[3]);
			const float gain = amx_ctof(params[4]);

			if (debug_mode) {
				const auto result = handler->effect_create_parameq(priority, center, bandwidth, gain);
				LogDebug("[%s] : priority(%d), center(%f), bandwidth(%f), gain(%f) : return(%p)", __FUNCTION__, priority, center, bandwidth, gain, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->effect_create_parameq(priority, center, bandwidth, gain));

		}

		// Создать звуковой эффект "reverb"
		static cell AMX_NATIVE_CALL n_sv_effect_create_reverb(
			AMX *amx, cell *params
		) {

			if (params[0] != (5 * sizeof(cell))) return NULL;

			const int priority = static_cast<int>(params[1]);
			const float ingain = amx_ctof(params[2]);
			const float reverbmix = amx_ctof(params[3]);
			const float reverbtime = amx_ctof(params[4]);
			const float highfreqrtratio = amx_ctof(params[5]);

			if (debug_mode) {
				const auto result = handler->effect_create_reverb(priority, ingain, reverbmix, reverbtime, highfreqrtratio);
				LogDebug("[%s] : priority(%d), ingain(%f), reverbmix(%f), reverbtime(%f), highfreqrtratio(%f) : return(%p)", __FUNCTION__, priority, ingain, reverbmix, reverbtime, highfreqrtratio, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->effect_create_reverb(priority, ingain, reverbmix, reverbtime, highfreqrtratio));

		}

		// Удалить эффект
		static cell AMX_NATIVE_CALL n_sv_effect_delete(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			audio::effects::effect* effect = reinterpret_cast<audio::effects::effect*>(params[1]);

			if (debug_mode) LogDebug("[%s] : effect(%p)", __FUNCTION__, effect);

			handler->effect_delete(effect);
			return NULL;

		}



		// Установить параметр класса потока
		static cell AMX_NATIVE_CALL n_sv_stream_set_parameter(
			AMX *amx, cell *params
		) {

			if (params[0] != (3 * sizeof(cell))) return NULL;

			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[1]);
			const uint8_t param_id = static_cast<uint8_t>(params[2]);
			const float value = amx_ctof(params[3]);

			if (debug_mode) LogDebug("[%s] : target(%p), param_id(%hhu), value(%f)", __FUNCTION__, target, param_id, value);

			handler->stream_set_parameter(target, param_id, value);
			return NULL;

		}

		// Задать скольжение параметра класса потока
		static cell AMX_NATIVE_CALL n_sv_stream_slide_parameter(
			AMX *amx, cell *params
		) {

			if (params[0] != (5 * sizeof(cell))) return NULL;

			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[1]);
			const uint8_t param_id = static_cast<uint8_t>(params[2]);
			const float start_value = amx_ctof(params[3]);
			const float end_value = amx_ctof(params[4]);
			const uint32_t time = static_cast<uint32_t>(params[5]);

			if (debug_mode) LogDebug("[%s] : target(%p), param_id(%hhu), start_value(%f), end_value(%f), time(%u)", __FUNCTION__, target, param_id, start_value, end_value, time);

			handler->stream_slide_parameter(target, param_id, start_value, end_value, time);
			return NULL;

		}

		// Привязать класс эффекта к классу потока
		static cell AMX_NATIVE_CALL n_sv_stream_effect_attach(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[1]);
			audio::effects::effectable* effect = reinterpret_cast<audio::effects::effectable*>(params[2]);

			if (debug_mode) LogDebug("[%s] : target(%p), effect(%p)", __FUNCTION__, target, effect);

			handler->stream_effect_attach(target, effect);
			return NULL;

		}

		// Отвязать класс эффекта от класса потока
		static cell AMX_NATIVE_CALL n_sv_stream_effect_detach(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[1]);
			audio::effects::effectable* effect = reinterpret_cast<audio::effects::effectable*>(params[2]);

			if (debug_mode) LogDebug("[%s] : target(%p), effect(%p)", __FUNCTION__, target, effect);

			handler->stream_effect_detach(target, effect);
			return NULL;

		}

		// Запустить воспроизведение звукового файла из хранилища в класс потока
		static cell AMX_NATIVE_CALL n_sv_stream_play_sound_internal(
			AMX *amx, cell *params
		) {

			if (params[0] != (3 * sizeof(cell))) return false;

			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[1]);
			const uint32_t index = static_cast<uint32_t>(params[2]);
			const bool loop = static_cast<bool>(params[3]);

			if (debug_mode) {
				const auto result = handler->stream_play_sound_internal(target, index, loop);
				LogDebug("[%s] : target(%p), index(%u), loop(%s) : return(%s)", __FUNCTION__, target, index, (loop ? "true" : "false"), (result ? "true" : "false"));
				return result;
			}

			return handler->stream_play_sound_internal(target, index, loop);

		}

		// Запустить воспроизведение звукового файла по url в класс потока
		static cell AMX_NATIVE_CALL n_sv_stream_play_sound_external(
			AMX *amx, cell *params
		) {

			int length;
			char *string;
			cell *phys_addr;

			if (params[0] != (3 * sizeof(cell))) return false;

			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[1]);
			const bool loop = static_cast<bool>(params[3]);
			if (!amx_GetAddr(amx, params[2], &phys_addr) &&
				!amx_StrLen(phys_addr, &length) &&
				(string = (char*)(alloca(length + 1))) &&
				!amx_GetString(string, phys_addr, false, length + 1)
			) {

				string[length] = '\0';

				if (debug_mode) {
					const auto result = handler->stream_play_sound_external(target, string, loop);
					LogDebug("[%s] : target(%p), url(%s), loop(%s) : return(%s) ", __FUNCTION__, target, string, (loop ? "true" : "false"), (result ? "true" : "false"));
					return result;
				}

				return handler->stream_play_sound_external(target, string, loop);

			} else return false;

		}

		// Остановить воспроизведение звукового файла в класс потока
		static cell AMX_NATIVE_CALL n_sv_stream_stop_sound(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[1]);

			if (debug_mode) LogDebug("[%s] : target(%p)", __FUNCTION__, target);

			handler->stream_stop_sound(target);
			return NULL;

		}

		// Привязать игрока к классу потока
		static cell AMX_NATIVE_CALL n_sv_stream_player_attach(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[1]);
			const uint16_t player_id = static_cast<uint16_t>(params[2]);

			if (debug_mode) LogDebug("[%s] : target(%p), player_id(%hu)", __FUNCTION__, target, player_id);

			handler->stream_player_attach(target, player_id);
			return NULL;

		}

		// Отвязать игрока от класса потока
		static cell AMX_NATIVE_CALL n_sv_stream_player_detach(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[1]);
			const uint16_t player_id = static_cast<uint16_t>(params[2]);

			if (debug_mode) LogDebug("[%s] : target(%p), player_id(%hu)", __FUNCTION__, target, player_id);

			handler->stream_player_detach(target, player_id);
			return NULL;

		}

		// Удалить поток
		static cell AMX_NATIVE_CALL n_sv_stream_delete(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			audio::streams::stream* stream = reinterpret_cast<audio::streams::stream*>(params[1]);

			if (debug_mode) LogDebug("[%s] : stream(%p)", __FUNCTION__, stream);

			handler->stream_delete(stream);
			return NULL;

		}



		// Создать статический глобальный поток
		static cell AMX_NATIVE_CALL n_sv_sgstream_create(
			AMX *amx, cell *params
		) {

			if (params[0] != (0 * sizeof(cell))) return NULL;

			if (debug_mode) {
				const auto result = handler->sgstream_create();
				LogDebug("[%s] : return(result)", __FUNCTION__, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->sgstream_create());

		}

		// Обновить координаты статического локального потока
		static cell AMX_NATIVE_CALL n_sv_slstream_update(
			AMX *amx, cell *params
		) {

			if (params[0] != (10 * sizeof(cell))) return NULL;

			audio::streams::stream_static_local_at_point* slstream = reinterpret_cast<audio::streams::stream_static_local_at_point*>(params[1]);
			const float pos_x = amx_ctof(params[2]);
			const float pos_y = amx_ctof(params[3]);
			const float pos_z = amx_ctof(params[4]);
			const float ornt_x = amx_ctof(params[5]);
			const float ornt_y = amx_ctof(params[6]);
			const float ornt_z = amx_ctof(params[7]);
			const float vel_x = amx_ctof(params[8]);
			const float vel_y = amx_ctof(params[9]);
			const float vel_z = amx_ctof(params[10]);

			if (debug_mode) LogDebug("[%s] : slstream(%p), pos_x(%f), pos_y(%f), pos_z(%f), ornt_x(%f), ornt_y(%f), ornt_z(%f), vel_x(%f), vel_y(%f), vel_z(%f)", __FUNCTION__, slstream, pos_x, pos_y, pos_z, ornt_x, ornt_y, ornt_z, vel_x, vel_y, vel_z);

			handler->slstream_update(slstream, pos_x, pos_y, pos_z, ornt_x, ornt_y, ornt_z, vel_x, vel_y, vel_z);
			return NULL;

		}

		// Создать статический локальный поток на точку
		static cell AMX_NATIVE_CALL n_sv_slstream_create_at_point(
			AMX *amx, cell *params
		) {

			if (params[0] != (9 * sizeof(cell))) return NULL;

			const float pos_x = amx_ctof(params[1]);
			const float pos_y = amx_ctof(params[2]);
			const float pos_z = amx_ctof(params[3]);
			const float ornt_x = amx_ctof(params[4]);
			const float ornt_y = amx_ctof(params[5]);
			const float ornt_z = amx_ctof(params[6]);
			const float vel_x = amx_ctof(params[7]);
			const float vel_y = amx_ctof(params[8]);
			const float vel_z = amx_ctof(params[9]);

			if (debug_mode) {
				const auto result = handler->slstream_create_at_point(pos_x, pos_y, pos_z, ornt_x, ornt_y, ornt_z, vel_x, vel_y, vel_z);
				LogDebug("[%s] : pos_x(%f), pos_y(%f), pos_z(%f), ornt_x(%f), ornt_y(%f), ornt_z(%f), vel_x(%f), vel_y(%f), vel_z(%f) : return(%p)", __FUNCTION__, pos_x, pos_y, pos_z, ornt_x, ornt_y, ornt_z, vel_x, vel_y, vel_z, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->slstream_create_at_point(pos_x, pos_y, pos_z, ornt_x, ornt_y, ornt_z, vel_x, vel_y, vel_z));

		}

		// Создать статический локальный поток с привязкой к автомобилю
		static cell AMX_NATIVE_CALL n_sv_slstream_create_at_vehicle(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			const uint16_t vehicle_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) {
				const auto result = handler->slstream_create_at_vehicle(vehicle_id);
				LogDebug("[%s] : vehicle_id(%hu) : return(%p)", __FUNCTION__, vehicle_id, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->slstream_create_at_vehicle(vehicle_id));

		}

		// Создать статический локальный поток с привязкой к игроку
		static cell AMX_NATIVE_CALL n_sv_slstream_create_at_player(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			const uint16_t player_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) {
				const auto result = handler->slstream_create_at_player(player_id);
				LogDebug("[%s] : player_id(%hu) : return(%p)", __FUNCTION__, player_id, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->slstream_create_at_player(player_id));

		}

		// Создать статический локальный поток с привязкой к объекту
		static cell AMX_NATIVE_CALL n_sv_slstream_create_at_object(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			const uint16_t object_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) {
				const auto result = handler->slstream_create_at_object(object_id);
				LogDebug("[%s] : object_id(%hu) : return(%p)", __FUNCTION__, object_id, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->slstream_create_at_object(object_id));

		}



		// Создать динамический локальный поток на точку
		static cell AMX_NATIVE_CALL n_sv_dlstream_create_at_point(
			AMX *amx, cell *params
		) {

			if (params[0] != (9 * sizeof(cell))) return NULL;

			const float pos_x = amx_ctof(params[1]);
			const float pos_y = amx_ctof(params[2]);
			const float pos_z = amx_ctof(params[3]);
			const float ornt_x = amx_ctof(params[4]);
			const float ornt_y = amx_ctof(params[5]);
			const float ornt_z = amx_ctof(params[6]);
			const float vel_x = amx_ctof(params[7]);
			const float vel_y = amx_ctof(params[8]);
			const float vel_z = amx_ctof(params[9]);

			if (debug_mode) {
				const auto result = handler->dlstream_create_at_point(pos_x, pos_y, pos_z, ornt_x, ornt_y, ornt_z, vel_x, vel_y, vel_z);
				LogDebug("[%s] : pos_x(%f), pos_y(%f), pos_z(%f), ornt_x(%f), ornt_y(%f), ornt_z(%f), vel_x(%f), vel_y(%f), vel_z(%f) : return(%p)", __FUNCTION__, pos_x, pos_y, pos_z, ornt_x, ornt_y, ornt_z, vel_x, vel_y, vel_z, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->dlstream_create_at_point(pos_x, pos_y, pos_z, ornt_x, ornt_y, ornt_z, vel_x, vel_y, vel_z));

		}

		// Создать динамический локальный поток с привязкой к автомобилю
		static cell AMX_NATIVE_CALL n_sv_dlstream_create_at_vehicle(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			const uint16_t vehicle_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) {
				const auto result = handler->dlstream_create_at_vehicle(vehicle_id);
				LogDebug("[%s] : vehicle_id(%hu) : return(%p)", __FUNCTION__, vehicle_id, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->dlstream_create_at_vehicle(vehicle_id));

		}

		// Создать динамический локальный поток с привязкой к игроку
		static cell AMX_NATIVE_CALL n_sv_dlstream_create_at_player(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			const uint16_t player_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) {
				const auto result = handler->dlstream_create_at_player(player_id);
				LogDebug("[%s] : player_id(%hu) : return(%p)", __FUNCTION__, player_id, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->dlstream_create_at_player(player_id));

		}

		// Создать динамический локальный поток с привязкой к объекту
		static cell AMX_NATIVE_CALL n_sv_dlstream_create_at_object(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			const uint16_t object_id = static_cast<uint16_t>(params[1]);

			if (debug_mode) {
				const auto result = handler->dlstream_create_at_object(object_id);
				LogDebug("[%s] : object_id(%hu) : return(%p)", __FUNCTION__, object_id, result);
				return reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->dlstream_create_at_object(object_id));

		}



		// Добавить элемент в группу
		static cell AMX_NATIVE_CALL n_sv_group_add(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			audio::group* group = reinterpret_cast<audio::group*>(params[1]);
			void * const element = reinterpret_cast<void*>(params[2]);

			if (debug_mode) LogDebug("[%s] : group(%p), element(%p)", __FUNCTION__, group, element);

			handler->group_add(group, element);
			return NULL;

		}

		// Удалить элемент из группы
		static cell AMX_NATIVE_CALL n_sv_group_remove(
			AMX *amx, cell *params
		) {
			
			if (params[0] != (2 * sizeof(cell))) return NULL;

			audio::group* group = reinterpret_cast<audio::group*>(params[1]);
			void * const element = reinterpret_cast<void*>(params[2]);

			if (debug_mode) LogDebug("[%s] : group(%p), element(%p)", __FUNCTION__, group, element);

			handler->group_remove(group, element);
			return NULL;

		}

		// Удалить группу
		static cell AMX_NATIVE_CALL n_sv_group_delete(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			audio::group* group = reinterpret_cast<audio::group*>(params[1]);

			if (debug_mode) LogDebug("[%s] : group(%p)", __FUNCTION__, group);

			handler->group_delete(group);
			return NULL;

		}



		// Создать группу потоков
		static cell AMX_NATIVE_CALL n_sv_sgroup_create(
			AMX *amx, cell *params
		) {

			if (params[0] != (0 * sizeof(cell))) return NULL;

			if (debug_mode) {
				const auto result = handler->sgroup_create();
				LogDebug("[%s] : return(%p)", __FUNCTION__, result);
				reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->sgroup_create());

		}



		// Создать группу эффектов
		static cell AMX_NATIVE_CALL n_sv_egroup_create(
			AMX *amx, cell *params
		) {

			if (params[0] != (0 * sizeof(cell))) return NULL;

			if (debug_mode) {
				const auto result = handler->egroup_create();
				LogDebug("[%s] : return(%p)", __FUNCTION__, result);
				reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->egroup_create());

		}



		// Создать звуковой буфер
		static cell AMX_NATIVE_CALL n_sv_buffer_create(
			AMX *amx, cell *params
		) {

			if (params[0] != (0 * sizeof(cell))) return NULL;
			
			if (debug_mode) {
				const auto result = handler->buffer_create();
				LogDebug("[%s] : return(%p)", __FUNCTION__, result);
				reinterpret_cast<cell>(result);
			}

			return reinterpret_cast<cell>(handler->buffer_create());

		}

		// Очистить звуковой буфер
		static cell AMX_NATIVE_CALL n_sv_buffer_clear(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			audio::buffer* buffer = reinterpret_cast<audio::buffer*>(params[1]);

			if (debug_mode) LogDebug("[%s] : buffer(%p)", __FUNCTION__, buffer);

			handler->buffer_clear(buffer);
			return NULL;

		}

		// Сохранить звуковой буфер в файл
		static cell AMX_NATIVE_CALL n_sv_buffer_save(
			AMX *amx, cell *params
		) {

			int length;
			char *string;
			cell *phys_addr;

			if (params[0] != (2 * sizeof(cell))) return false;

			audio::buffer* buffer = reinterpret_cast<audio::buffer*>(params[1]);
			if (!amx_GetAddr(amx, params[2], &phys_addr) &&
				!amx_StrLen(phys_addr, &length) &&
				(string = (char*)alloca(length + 1)) &&
				!amx_GetString(string, phys_addr, false, length + 1)
			) {

				string[length] = '\0';
				
				if (debug_mode) {
					const auto result = handler->buffer_save(buffer, string);
					LogDebug("[%s] : buffer(%p), filename(%s) : return(%s) ", __FUNCTION__, buffer, string, (result ? "true" : "false"));
					return result;
				}

				return handler->buffer_save(buffer, string);

			} else return false;

		}

		// Загрузить звуковой буфер из файла
		static cell AMX_NATIVE_CALL n_sv_buffer_load(
			AMX *amx, cell *params
		) {

			int length;
			char *string;
			cell *phys_addr;

			if (params[0] != (2 * sizeof(cell))) return false;

			audio::buffer* buffer = reinterpret_cast<audio::buffer*>(params[1]);
			if (!amx_GetAddr(amx, params[2], &phys_addr) &&
				!amx_StrLen(phys_addr, &length) &&
				(string = (char*)alloca(length + 1)) &&
				!amx_GetString(string, phys_addr, false, length + 1)
			) {

				string[length] = '\0';

				if (debug_mode) {
					const auto result = handler->buffer_load(buffer, string);
					LogDebug("[%s] : buffer(%p), filename(%s) : return(%s) ", __FUNCTION__, buffer, string, (result ? "true" : "false"));
					return result;
				}

				return handler->buffer_load(buffer, string);

			} else return false;

		}

		// Привязать буфер к потоковому классу
		static cell AMX_NATIVE_CALL n_sv_buffer_attach(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			audio::buffer* buffer = reinterpret_cast<audio::buffer*>(params[1]);
			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[2]);

			if (debug_mode) LogDebug("[%s] : buffer(%p), target(%p)", __FUNCTION__, buffer, target);

			handler->buffer_attach(buffer, target);
			return NULL;

		}

		// Отвязать буфер от потокового класса
		static cell AMX_NATIVE_CALL n_sv_buffer_detach(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			audio::buffer* buffer = reinterpret_cast<audio::buffer*>(params[1]);
			audio::streams::streamable* target = reinterpret_cast<audio::streams::streamable*>(params[2]);

			if (debug_mode) LogDebug("[%s] : buffer(%p), target(%p)", __FUNCTION__, buffer, target);

			handler->buffer_detach(buffer, target);
			return NULL;

		}

		// Отвязать буфер от всех потоковых классов
		static cell AMX_NATIVE_CALL n_sv_buffer_detach_all(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			audio::buffer* buffer = reinterpret_cast<audio::buffer*>(params[1]);

			if (debug_mode) LogDebug("[%s] : buffer(%p)", __FUNCTION__, buffer);

			handler->buffer_detach_all(buffer);
			return NULL;

		}

		// Начать трансляцию пакетов
		static cell AMX_NATIVE_CALL n_sv_buffer_play(
			AMX *amx, cell *params
		) {

			if (params[0] != (2 * sizeof(cell))) return NULL;

			audio::buffer* buffer = reinterpret_cast<audio::buffer*>(params[1]);
			const bool loop = static_cast<bool>(params[2]);

			if (debug_mode) LogDebug("[%s] : buffer(%p), loop(%s)", __FUNCTION__, buffer, (loop ? "true" : "false"));

			handler->buffer_play(buffer, loop);
			return NULL;

		}

		// Остановить трансляцию пакетов
		static cell AMX_NATIVE_CALL n_sv_buffer_stop(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			audio::buffer* buffer = reinterpret_cast<audio::buffer*>(params[1]);

			if (debug_mode) LogDebug("[%s] : buffer(%p)", __FUNCTION__, buffer);

			handler->buffer_stop(buffer);
			return NULL;

		}

		// Удалить буфер
		static cell AMX_NATIVE_CALL n_sv_buffer_delete(
			AMX *amx, cell *params
		) {

			if (params[0] != (1 * sizeof(cell))) return NULL;

			audio::buffer* buffer = reinterpret_cast<audio::buffer*>(params[1]);

			if (debug_mode) LogDebug("[%s] : buffer(%p)", __FUNCTION__, buffer);

			handler->buffer_delete(buffer);
			return NULL;

		}



		// Зарегистрировать звук
		static cell AMX_NATIVE_CALL n_sv_sound_register(
			AMX *amx, cell *params
		) {

			int length;
			char *string;
			cell *phys_addr;

			if (params[0] != sizeof(cell)) return NULL;

			if (!amx_GetAddr(amx, params[1], &phys_addr) &&
				!amx_StrLen(phys_addr, &length) &&
				(string = (char*)alloca(length + 1)) &&
				!amx_GetString(string, phys_addr, false, length + 1)
			) {

				string[length] = '\0';

				if (debug_mode) {
					const auto result = handler->sound_register(string);
					LogDebug("[%s] : url(%s) : return(%u) ", __FUNCTION__, string, result);
					return result;
				}

				return handler->sound_register(string);

			} else return NULL;

		}



	}

	class script {
	private:

		static std::vector<script> scripts_onplayervoice;
		static std::vector<script> scripts_onstreamend;
		static std::vector<script> scripts_onbufferend;

		AMX * const amx;
		int index_onplayervoice;
		int index_onstreamend;
		int index_onbufferend;

		script(AMX* _amx) : amx(_amx) {}

	public:

		// Зарегистрировать скрипт
		static void reg(
			AMX *amx
		) {
			
			int tmp_index;

			if (!pawn::handler) return;

			const AMX_NATIVE_INFO natives_list[] = {

				// Основное
				{ "sv_init",						natives::n_sv_init },
				{ "sv_get_version",					natives::n_sv_get_version },
				{ "sv_set_key",						natives::n_sv_set_key },
				{ "sv_send_packet",					natives::n_sv_send_packet },

				// Микрофон
				{ "sv_has_micro",					natives::n_sv_has_micro },
				{ "sv_record_start",				natives::n_sv_record_start },
				{ "sv_record_stop",					natives::n_sv_record_stop },
				{ "sv_record_volume",				natives::n_sv_record_volume },

				// Эффект
				{ "sv_effect_create_chorus",		natives::n_sv_effect_create_chorus },
				{ "sv_effect_create_compressor",	natives::n_sv_effect_create_compressor },
				{ "sv_effect_create_distortion",	natives::n_sv_effect_create_distortion },
				{ "sv_effect_create_echo",			natives::n_sv_effect_create_echo },
				{ "sv_effect_create_flanger",		natives::n_sv_effect_create_flanger },
				{ "sv_effect_create_gargle",		natives::n_sv_effect_create_gargle },
				{ "sv_effect_create_i3dl2reverb",	natives::n_sv_effect_create_i3dl2reverb },
				{ "sv_effect_create_parameq",		natives::n_sv_effect_create_parameq },
				{ "sv_effect_create_reverb",		natives::n_sv_effect_create_reverb },
				{ "sv_effect_delete",				natives::n_sv_effect_delete },

				// Интерфейс потока
				{ "sv_stream_set_parameter",		natives::n_sv_stream_set_parameter },
				{ "sv_stream_slide_parameter",		natives::n_sv_stream_slide_parameter },
				{ "sv_stream_effect_attach",		natives::n_sv_stream_effect_attach },
				{ "sv_stream_effect_detach",		natives::n_sv_stream_effect_detach },
				{ "sv_stream_play_sound_internal",	natives::n_sv_stream_play_sound_internal },
				{ "sv_stream_play_sound_external",	natives::n_sv_stream_play_sound_external },
				{ "sv_stream_stop_sound",			natives::n_sv_stream_stop_sound },
				{ "sv_stream_player_attach",		natives::n_sv_stream_player_attach },
				{ "sv_stream_player_detach",		natives::n_sv_stream_player_detach },
				{ "sv_stream_delete",				natives::n_sv_stream_delete },

				// Статический глобальный поток
				{ "sv_sgstream_create",				natives::n_sv_sgstream_create },

				// Статический локальный поток
				{ "sv_slstream_update",				natives::n_sv_slstream_update },
				{ "sv_slstream_create_at_point",	natives::n_sv_slstream_create_at_point },
				{ "sv_slstream_create_at_vehicle",	natives::n_sv_slstream_create_at_vehicle },
				{ "sv_slstream_create_at_player",	natives::n_sv_slstream_create_at_player },
				{ "sv_slstream_create_at_object",	natives::n_sv_slstream_create_at_object },

				// Динамический локальный поток
				{ "sv_dlstream_create_at_point",	natives::n_sv_dlstream_create_at_point },
				{ "sv_dlstream_create_at_vehicle",	natives::n_sv_dlstream_create_at_vehicle },
				{ "sv_dlstream_create_at_player",	natives::n_sv_dlstream_create_at_player },
				{ "sv_dlstream_create_at_object",	natives::n_sv_dlstream_create_at_object },

				// Интерфейс группы
				{ "sv_group_add",					natives::n_sv_group_add },
				{ "sv_group_remove",				natives::n_sv_group_remove },
				{ "sv_group_delete",				natives::n_sv_group_delete },

				// Группа потоков
				{ "sv_sgroup_create",				natives::n_sv_sgroup_create },

				// Группа эффектов
				{ "sv_egroup_create",				natives::n_sv_egroup_create },

				// Звуковой буфер
				{ "sv_buffer_create",				natives::n_sv_buffer_create },
				{ "sv_buffer_clear",				natives::n_sv_buffer_clear },
				{ "sv_buffer_save",					natives::n_sv_buffer_save },
				{ "sv_buffer_load",					natives::n_sv_buffer_load },
				{ "sv_buffer_attach",				natives::n_sv_buffer_attach },
				{ "sv_buffer_detach",				natives::n_sv_buffer_detach },
				{ "sv_buffer_detach_all",			natives::n_sv_buffer_detach_all },
				{ "sv_buffer_play",					natives::n_sv_buffer_play },
				{ "sv_buffer_stop",					natives::n_sv_buffer_stop },
				{ "sv_buffer_delete",				natives::n_sv_buffer_delete },

				// Хранилище звуков
				{ "sv_sound_register",				natives::n_sv_sound_register }

			}; amx_Register(amx, natives_list, sizeof(natives_list) / sizeof(AMX_NATIVE_INFO));

			script new_obj = script(amx);

			// Поиск паблика OnPlayerVoice
			if (!amx_FindPublic(amx, "OnPlayerVoice", &tmp_index) &&
				tmp_index >= 0
			) {
				if (debug_mode) LogDebug("[%s] : find OnPlayerVoice : index(%d)", __FUNCTION__, tmp_index);
				new_obj.index_onplayervoice = tmp_index;
				scripts_onplayervoice.push_back(new_obj);
			}

			// Поиск паблика OnStreamEnd
			if (!amx_FindPublic(amx, "OnStreamEnd", &tmp_index) &&
				tmp_index >= 0
			) {
				if (debug_mode) LogDebug("[%s] : find OnStreamEnd : index(%d)", __FUNCTION__, tmp_index);
				new_obj.index_onstreamend = tmp_index;
				scripts_onstreamend.push_back(new_obj);
			}

			// Поиск паблика OnBufferEnd
			if (!amx_FindPublic(amx, "OnBufferEnd", &tmp_index) &&
				tmp_index >= 0
			) {
				if (debug_mode) LogDebug("[%s] : find OnBufferEnd : index(%d)", __FUNCTION__, tmp_index);
				new_obj.index_onbufferend = tmp_index;
				scripts_onbufferend.push_back(new_obj);
			}

		}

		// Получить список скриптов паблика onplayervoice
		static inline std::vector<script>* get_scripts_opv() {
			return &scripts_onplayervoice;
		}

		// Получить список скриптов паблика onstreamend
		static inline std::vector<script>* get_scripts_ose() {
			return &scripts_onstreamend;
		}

		// Получить список скриптов паблика onbufferend
		static inline std::vector<script>* get_scripts_obe() {
			return &scripts_onbufferend;
		}

		// Вызвать паблик OnPlayerVoice
		bool onplayervoice(
			int player_id,
			Packet *packet,
			uint16_t volume
		) {
			int retval;
			amx_Push(this->amx, (cell)(volume));
			amx_Push(this->amx, (cell)(packet));
			amx_Push(this->amx, (cell)(player_id));
			amx_Exec(this->amx, &retval, this->index_onplayervoice);
			return retval;
		}

		// Вызвать паблик OnStreamEnd
		void onstreamend(
			uint32_t stream
		) {
			int retval;
			amx_Push(this->amx, stream);
			amx_Exec(this->amx, &retval, this->index_onstreamend);
		}

		// Вызвать паблик OnBufferEnd
		void onbufferend(
			uint32_t buffer
		) {
			int retval;
			amx_Push(this->amx, buffer);
			amx_Exec(this->amx, &retval, this->index_onbufferend);
		}

	};

	// Инициализировать модуль
	static inline void init(
		handler_interface *handler
	) {
		if (!pawn::handler) {
			pawn::handler = handler;
			if (debug_mode) LogDebug("[%s] : module init", __FUNCTION__);
		}
	}

	// Выгрузить модуль
	static inline void free() {
		if (pawn::handler) {
			delete pawn::handler;
			pawn::handler = nullptr;
			if (debug_mode) LogDebug("[%s] : module free", __FUNCTION__);
		}
	}

}

std::vector<pawn::script> pawn::script::scripts_onplayervoice;
std::vector<pawn::script> pawn::script::scripts_onstreamend;
std::vector<pawn::script> pawn::script::scripts_onbufferend;

#endif