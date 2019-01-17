#include <thread>
#include <atomic>
#include <forward_list>

#include "resource.h"

#include "sv_header.h"
#include "sv_addresses.h"
#include "sv_render.h"
#include "sv_samp.h"
#include "sv_net.h"
#include "sv_audio.h"

#define SV_POST_TIMER_INTERVAL			25
#define SV_MAIN_THREAD_SLEEP_INTERVAL	10

namespace core {

	static volatile bool afk_status = false;		// Статус афк
	static volatile bool post_status = false;		// Статус пост-таймера
	static volatile bool plugin_status = false;		// Статус загрузки плагина
	static volatile bool change_status = false;		// Статус редактирования положения иконки
	static volatile bool record_busy = false;		// Занято ли устройство записи сервером
	static volatile bool record_status = false;		// Статус устройства записи (отключено/включено)
	static volatile uint8_t active_key_id = NULL;	// Текущая клавиша активации (0 - нет клавиши)
	static render::texture *icon_micro = nullptr;	// Текстура иконки микрофона
	static uint8_t *packet_voice;					// Голосовой пакет для отправки
	static uint8_t *packet_voice_data;				// Содержимое голосового пакета для отправки
	static uint16_t *ptr_volume_level;				// Указатель на уровень громкости пакета
	static UINT_PTR timer_post;						// Идентификатор пост-таймера

	// Обработчики команд
	namespace commands {

		// Показать справку по командам
		static void help(
			char *args
		) {
			samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : command list:");
			samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : /" SV_COMMAND_HELP " - show this help");
			samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : /" SV_COMMAND_CHANGE " - change icon position");
			samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : /" SV_COMMAND_REJECT " - reject status on/off");
			samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : /" SV_COMMAND_STORE " - change store size (in MB)");
			samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : /" SV_COMMAND_DEBUG " - debug status on/off");
		}

		/// [dependency]
		// Изменить позицию иконки
		static void change(
			char *args
		) {
			static std::thread *change_icon_pos_thread = nullptr;
			change_icon_pos_thread = new std::thread([]() {
				while (samp::pInput->iInputEnabled) Sleep(10);
				samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : click to apply changes...");
				samp::toggle_samp_cursor(true);
				change_status = true;
			});
		}

		// Запретить серверу контроллировать микрофон
		static void reject(
			char *args
		) {
			if (settings::get_settings()->reject_status = !settings::get_settings()->reject_status) {
				samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : reject{" SV_COLOR_STR_SUCCESS "} enable");
				if (audio::record::record_is_active()) audio::record::record_stop();
			} else samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : reject{" SV_COLOR_STR_ERROR "} disable");
		}

		// Изменить размер хранилища
		static void store(
			char *args
		) {
			uint32_t new_size;
			if (sscanf(args, "%u", &new_size)) {
				new_size *= 1048576u;	// Перевод мегабайтов в байты
				settings::get_settings()->store_limit = new_size;
				audio::playback::sounds::store::set_limit(new_size);
				samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : storage size set to %u bytes", new_size);
			}
		}

		// Включить режим отладки
		static void debug(
			char *args
		) {
			if (settings::get_settings()->debug_status = !settings::get_settings()->debug_status) samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : debug{" SV_COLOR_STR_SUCCESS "} enable");
			else samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : debug{" SV_COLOR_STR_ERROR "} disable");
		}

	}

	// Обработчик исходящего аудиопотока
	static void handler_voice(
		uint8_t *buffer
	) {
		if (util::is_active()) {
			*ptr_volume_level = audio::record::get_level();
			int len = audio::opus::encode((opus_int16*)(buffer), packet_voice_data, SV_NET_PACKET_SIZE);
			if (len > 0) net::send(packet_voice, sizeof(sv_packet::header) + sizeof(sv_packet::voice) + len);
		}
	}

	// Обработчик входящих пакетов
	static bool CALLBACK handler_packet(
		BYTE id,
		Packet *packet
	) {

		if (id != SV_NET_PACKET_ID) return true;

		sv_packet::header *p_header = reinterpret_cast<sv_packet::header*>(packet->data);
		switch (p_header->packet_id) {
		case e_packet_id::init: {
			sv_packet::init *st_data = p_header->get<sv_packet::init>();
			if (settings::get_settings()->debug_status) LogDebug("sv_init : bitrate(%u) frequency(%hu) voice_rate(%hhu) factor_distance(%f) factor_rolloff(%f) factor_doppler(%f)", st_data->bitrate, st_data->frequency, st_data->voice_rate, st_data->factor_distance, st_data->factor_rolloff, st_data->factor_doppler);
			if (!audio::init(handler_voice, st_data->bitrate, st_data->frequency, st_data->voice_rate, settings::get_settings()->store_limit, st_data->factor_distance, st_data->factor_rolloff, st_data->factor_doppler))
				LogError("main", "could not initialize audio module");
		} return false;
		case e_packet_id::set_key: {
			sv_packet::set_key *st_data = p_header->get<sv_packet::set_key>();
			if (settings::get_settings()->debug_status) LogDebug("set_key : key_id(%hhX)", st_data->key_id);
			active_key_id = st_data->key_id;
		} return false;
		case e_packet_id::voice: {
			if (!post_status) {
				sv_packet::voice *st_data = p_header->get<sv_packet::voice>();
				if (settings::get_settings()->debug_status) WriteLog("voice : sender_id(%hu) length(%d) stream(%u) volume(%hu)", packet->playerIndex, packet->length - (sizeof(sv_packet::header) + sizeof(sv_packet::voice)), st_data->stream, st_data->volume);
				audio::playback::push_enc(
					st_data->stream, packet->playerIndex, st_data->data,
					packet->length - (sizeof(sv_packet::header) + sizeof(sv_packet::voice))
				);
			}
		} return false;
		case e_packet_id::record_start: {
			if (!settings::get_settings()->reject_status) {
				if (settings::get_settings()->debug_status) LogDebug("record_start");
				record_status = record_busy = true;
				audio::record::record_start();
			} else {
				samp::add_message_to_chat(SV_COLOR_INFO, "[sampvoice] : microphone activation attempt rejected");
			}
		} return false;
		case e_packet_id::record_stop: {
			if (!settings::get_settings()->reject_status) {
				if (settings::get_settings()->debug_status) LogDebug("record_stop");
				record_status = record_busy = false;
				audio::record::record_stop();
			}
		} return false;
		case e_packet_id::record_volume: {
			if (!settings::get_settings()->reject_status) {
				sv_packet::record_volume *st_data = p_header->get<sv_packet::record_volume>();
				if (settings::get_settings()->debug_status) LogDebug("record_volume: volume(%f)", st_data->volume);
				audio::record::set_volume(st_data->volume);
			}
		} return false;
		case e_packet_id::effect_create: {
			sv_packet::effect_create *st_data = p_header->get<sv_packet::effect_create>();
			if (settings::get_settings()->debug_status) LogDebug("effect_create: stream(%u) effect(%u) effect_id(%hhu) parameters(%p) priority(%d)", st_data->stream, st_data->effect, st_data->effect_id, st_data->parameters, st_data->priority);
			audio::playback::get_stream(st_data->stream)->effect_create(
				st_data->effect,
				st_data->effect_id,
				st_data->parameters,
				st_data->priority
			);
		} return false;
		case e_packet_id::effect_delete: {
			sv_packet::effect_delete *st_data = p_header->get<sv_packet::effect_delete>();
			if (settings::get_settings()->debug_status) LogDebug("effect_delete: stream(%u) effect(%u)", st_data->stream, st_data->effect);
			audio::playback::get_stream(st_data->stream)->effect_delete(
				st_data->effect
			);
		} return false;
		case e_packet_id::stream_set_parameter: {
			sv_packet::stream_set_parameter *st_data = p_header->get<sv_packet::stream_set_parameter>();
			if (settings::get_settings()->debug_status) LogDebug("stream_set_parameter: stream(%u) param_id(%hhu) value(%f)", st_data->stream, st_data->param_id, st_data->value);
			audio::playback::get_stream(st_data->stream)->set_parameter(
				st_data->param_id, st_data->value
			);
		} return false;
		case e_packet_id::stream_slide_parameter: {
			sv_packet::stream_slide_parameter *st_data = p_header->get<sv_packet::stream_slide_parameter>();
			if (settings::get_settings()->debug_status) LogDebug("stream_slide_parameter: stream(%u) param_id(%hhu) start_value(%f) end_value(%f) time(%u)", st_data->stream, st_data->param_id, st_data->start_value, st_data->end_value, st_data->time);
			audio::playback::get_stream(st_data->stream)->slide_parameter(
				st_data->param_id, st_data->start_value, st_data->end_value, st_data->time
			);
		} return false;
		case e_packet_id::stream_play_sound_internal: {
			sv_packet::stream_play_audio_internal *st_data = p_header->get<sv_packet::stream_play_audio_internal>();
			if (settings::get_settings()->debug_status) LogDebug("stream_play_audio: index(%u) offset(%u) loop(%hhu)", st_data->index, st_data->offset, st_data->loop);
			audio::playback::get_stream(st_data->stream)->play_sound(
				st_data->index,
				st_data->offset,
				st_data->loop
			);
		} return false;
		case e_packet_id::stream_play_sound_external: {
			sv_packet::stream_play_audio_external *st_data = p_header->get<sv_packet::stream_play_audio_external>();
			if (settings::get_settings()->debug_status) LogDebug("stream_play_audio: url(%s) offset(%u) loop(%hhu)", st_data->url, st_data->offset, st_data->loop);
			audio::playback::get_stream(st_data->stream)->play_sound(
				st_data->url,
				st_data->offset,
				st_data->loop
			);
		} return false;
		case e_packet_id::stream_stop_sound: {
			sv_packet::stream_stop_audio *st_data = p_header->get<sv_packet::stream_stop_audio>();
			if (settings::get_settings()->debug_status) LogDebug("stream_stop_audio: stream(%u)", st_data->stream);
			audio::playback::get_stream(st_data->stream)->stop_sound();
		} return false;
		case e_packet_id::stream_delete: {
			sv_packet::stream_delete *st_data = p_header->get<sv_packet::stream_delete>();
			if (settings::get_settings()->debug_status) LogDebug("stream_delete: stream(%u)", st_data->stream);
			audio::playback::attach::remove(st_data->stream);
			audio::playback::delete_stream(st_data->stream);
		} return false;
		case e_packet_id::gstream_create: {
			sv_packet::gstream_create *st_data = p_header->get<sv_packet::gstream_create>();
			if (settings::get_settings()->debug_status) LogDebug("gstream_create: stream(%u)", st_data->stream);
			audio::playback::create_stream_global(st_data->stream);
		} return false;
		case e_packet_id::lstream_update: {
			sv_packet::slstream_params *st_data = p_header->get<sv_packet::slstream_params>();
			if (settings::get_settings()->debug_status) LogDebug("lstream_update: stream(%u) pos(%f:%f:%f) ornt(%f:%f:%f) vel(%f:%f:%f)",
				st_data->stream,
				st_data->position[0], st_data->position[1], st_data->position[2],
				st_data->orientation[0], st_data->orientation[1], st_data->orientation[2],
				st_data->velocity[0], st_data->velocity[1], st_data->velocity[2]
			);
			audio::playback::get_local_stream(st_data->stream)->update(
				st_data->position, st_data->orientation, st_data->velocity
			);
		} return false;
		case e_packet_id::lstream_create_at_point: {
			sv_packet::slstream_params *st_data = p_header->get<sv_packet::slstream_params>();
			if (settings::get_settings()->debug_status) LogDebug("lstream_create_at_point: stream(%u) pos(%f:%f:%f) ornt(%f:%f:%f) vel(%f:%f:%f)",
				st_data->stream,
				st_data->position[0], st_data->position[1], st_data->position[2],
				st_data->orientation[0], st_data->orientation[1], st_data->orientation[2],
				st_data->velocity[0], st_data->velocity[1], st_data->velocity[2]
			);
			audio::playback::create_stream_local(
				st_data->stream, st_data->position, st_data->orientation, st_data->velocity
			);
		} return false;
		case e_packet_id::lstream_create_at_vehicle: {
			sv_packet::dlstream_create *st_data = p_header->get<sv_packet::dlstream_create>();
			if (settings::get_settings()->debug_status) LogDebug("lstream_create_at_vehicle: stream(%u) target(%hu)", st_data->stream, st_data->target);
			audio::playback::create_stream_local(st_data->stream, &CVector{ 0, 0, 0 }, &CVector{ 0, 0, 0 }, &CVector{ 0, 0, 0 });
			audio::playback::attach::attach_vehicle_streams.push_front({ st_data->target, st_data->stream });
		} return false;
		case e_packet_id::lstream_create_at_player: {
			sv_packet::dlstream_create *st_data = p_header->get<sv_packet::dlstream_create>();
			if (settings::get_settings()->debug_status) LogDebug("lstream_create_at_player: stream(%u) target(%hu)", st_data->stream, st_data->target);
			audio::playback::create_stream_local(st_data->stream, &CVector{ 0, 0, 0 }, &CVector{ 0, 0, 0 }, &CVector{ 0, 0, 0 });
			audio::playback::attach::attach_player_streams.push_front({ st_data->target, st_data->stream });
		} return false;
		case e_packet_id::lstream_create_at_object: {
			sv_packet::dlstream_create *st_data = p_header->get<sv_packet::dlstream_create>();
			if (settings::get_settings()->debug_status) LogDebug("lstream_create_at_object: stream(%u) target(%hu)", st_data->stream, st_data->target);
			audio::playback::create_stream_local(st_data->stream, &CVector{ 0, 0, 0 }, &CVector{ 0, 0, 0 }, &CVector{ 0, 0, 0 });
			audio::playback::attach::attach_object_streams.push_front({ st_data->target, st_data->stream });
		} return false;
		case e_packet_id::sound_register: {
			sv_packet::sound_register *st_data = p_header->get<sv_packet::sound_register>();
			if (settings::get_settings()->debug_status) LogDebug("sound_register: index(%u) url(%s)", st_data->index, st_data->url);
			audio::playback::sounds::session::sound(st_data->index, st_data->url);
		} return false;
		default: return true;
		}

	}

	// Обработчик исходящих rpc
	static bool CALLBACK handler_rpc(
		BYTE id,
		RakNet::BitStream *bs
	) {

		if (id == 25u) {
			sv_packet::connect st_connect;
			st_connect.has_micro = audio::record::has_microphone();
			bs->Write((char*)&st_connect, sizeof(st_connect));
		}

		return true;

	}

	// Обработчик отключения от сервера
	static void CALLBACK handler_disconnect() {
		audio::free();
	}

	// Обработчик пост-таймера
	void CALLBACK handler_post_timer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {

		if (!audio::playback::table_stream.empty())
			for (auto i = audio::playback::table_stream.begin(); i != audio::playback::table_stream.end(); i++) (*i).second->reset();

		if (audio::record::record_is_active()) audio::record::record_start();

		post_status = false;
		KillTimer(hwnd, idEvent);

	}

	// Главный поток логики
	static DWORD WINAPI main_thread(
		LPVOID parameters
	) {

		while (plugin_status) {

			// Проверка режима афк для отклонения пакетов
			if (!util::is_active() && !afk_status) {
				if (post_status) KillTimer(render::get_hwnd(), timer_post);
				else post_status = true; afk_status = true;
			} else if (util::is_active() && afk_status) {
				timer_post = SetTimer(render::get_hwnd(), NULL, SV_POST_TIMER_INTERVAL, handler_post_timer);
				afk_status = false;
			}

			if (util::is_active()) {

				audio::tick();

				// Проверяем статус клавиши активации
				if (active_key_id && !samp::pInput->iInputEnabled && !record_busy && !record_status &&
					util::is_key_down(active_key_id)
				) {
					audio::record::record_start();
					record_status = true;
				} else if (active_key_id && !record_busy && record_status &&
					!util::is_key_down(active_key_id)
				) {
					audio::record::record_stop();
					record_status = false;
				}

			}

			Sleep(SV_MAIN_THREAD_SLEEP_INTERVAL);

		}

		return 0;

	}

	// Обработчик цикла отрисовки
	void handler_render(
		IDirect3DDevice9 *device,
		CONST RECT *source_rect,
		HWND hwnd
	) {
		
		if (util::is_active()) {

			if (change_status) {
				POINT cur_pos;
				GetCursorPos(&cur_pos);
				settings::get_settings()->icon_x = cur_pos.x - icon_micro->width() / 2;
				settings::get_settings()->icon_y = cur_pos.y - icon_micro->height() / 2;
				if (util::is_key_down(0x01)) {
					samp::toggle_samp_cursor(false);
					samp::add_message_to_chat(SV_COLOR_SUCCESS, "[sampvoice] : changes saved");
					change_status = false;
				} icon_micro->draw(source_rect, settings::get_settings()->icon_x, settings::get_settings()->icon_y);
				return;
			}

			if (record_status) icon_micro->draw(source_rect, settings::get_settings()->icon_x, settings::get_settings()->icon_y);

		}
		
	}

	// Обработчик инициализации устройства отрисовки
	void handler_init(
		IDirect3DDevice9 *device,
		D3DPRESENT_PARAMETERS *parameters
	) {

		if (!icon_micro) {
			HGLOBAL icon_resource;
			void *addr; uint32_t size;
			if (util::load_resource(IDB_PNG1, "PNG", &icon_resource, &addr, &size)) {
				icon_micro = new render::texture(device, addr, size);
				util::free_resource(icon_resource);
			}
		}

	}

	// Обработчик сброса устройства отрисовки
	void handler_reset() {

		if (icon_micro) {
			delete icon_micro;
			icon_micro = nullptr;
		}

	}

	// Обработчик деструктора samp
	static void CALLBACK handler_exit() {

		if (!settings::save(SV_PATH_CONFIG))
			LogError("main", "could not save configs");

		audio::free();
		std::free(packet_voice);
		handler_reset();
		logger::free();
		plugin_status = false;

	}

	// Главная функция плагина
	static void start() {
		
		if (!settings::load(SV_PATH_CONFIG)) {
			settings::get_settings()->icon_x = render::get_parameters()->BackBufferWidth - (icon_micro->width() + SV_RENDER_PADDING_MICRO_X);
			settings::get_settings()->icon_y = render::get_parameters()->BackBufferHeight - (icon_micro->height() + SV_RENDER_PADDING_MICRO_Y);
			settings::get_settings()->store_limit = 268435456u;	// 256 МБайт
			settings::get_settings()->reject_status = false;
			settings::get_settings()->debug_status = false;
		}

		samp::add_client_command(SV_COMMAND_HELP, commands::help);
		samp::add_client_command(SV_COMMAND_CHANGE, commands::change);
		samp::add_client_command(SV_COMMAND_REJECT, commands::reject);
		samp::add_client_command(SV_COMMAND_STORE, commands::store);
		samp::add_client_command(SV_COMMAND_DEBUG, commands::debug);

		// Выделяем место под голосовой пакет отправки
		packet_voice = reinterpret_cast<uint8_t*>(malloc(sizeof(sv_packet::header) + sizeof(sv_packet::voice) + SV_NET_PACKET_SIZE));
		packet_voice_data = packet_voice + sizeof(sv_packet::header) + sizeof(sv_packet::voice);
		ptr_volume_level = &reinterpret_cast<sv_packet::header*>(packet_voice)->get<sv_packet::voice>()->volume;

		reinterpret_cast<sv_packet::header*>(packet_voice)->samp_packet_id = SV_NET_PACKET_ID;
		reinterpret_cast<sv_packet::header*>(packet_voice)->packet_id = e_packet_id::voice;
		reinterpret_cast<sv_packet::header*>(packet_voice)->get<sv_packet::voice>()->stream = 0;

		plugin_status = true;
		CreateThread(0, 0, main_thread, 0, 0, 0);

	}

}

// -----------------------------------------

static volatile uint8_t status_net = 2ui8;
static volatile uint8_t status_samp = 2ui8;

static DWORD WINAPI NetInit(
	LPVOID net_init
) {
	status_net = net::init(core::handler_packet, core::handler_rpc, core::handler_disconnect);
	return 0;
}

static DWORD WINAPI SampInit(
	LPVOID samp_init
) {
	status_samp = samp::init(core::handler_exit);
	return 0;
}

static DWORD WINAPI ThreadInit(
	LPVOID parameters
) {

	if (!CreateDirectory(SV_PATH_SVDIR, NULL) ||
		!CreateDirectory(SV_PATH_STORE, NULL)
	) {
		FILE *output = fopen("sv_log.txt", "wt");
		fprintf(output, "could not create directories (code:%u)\n", GetLastError());
		fclose(output);
	}

	if (!logger::init()) return 0;
	addresses::init();

	if (!render::core::init(core::handler_render, core::handler_init, core::handler_reset)) {
		LogError("main", "could not initialize independent modules");
		logger::free(); return 0;
	}

	if (!CreateThread(0, 0, SampInit, 0, 0, 0) ||
		!CreateThread(0, 0, NetInit, 0, 0, 0)
	) {
		LogError("main", "could not create threads dependent modules");
		render::core::free(); logger::free(); return 0;
	}

	while (status_samp == 2 || status_net == 2) Sleep(10);

	if (!status_samp) {
		LogError("main", "could not initialize samp");
		render::core::free(); logger::free(); return 0;
	} if (!status_net) {
		LogError("main", "could not initialize net");
		render::core::free(); logger::free(); return 0;
	}

	samp::add_message_to_chat(0xFFA9C4E4, "{FFFFFF}SAMPVOICE {B9C9BF}2.0 {FFFFFF}Started");
	samp::add_message_to_chat(0xFFA9C4E4, "{FFFFFF}/svhelp - help on commands");

	core::start();
	return 0;

}

BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD dwReasonForCall,
	LPVOID lpReserved
) {
	switch (dwReasonForCall) {
		case DLL_PROCESS_ATTACH:
			util::h_module = hModule;
			return (BOOL)(CreateThread(0, 0, ThreadInit, 0, 0, 0));
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	} return TRUE;
}
