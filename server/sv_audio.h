#ifndef _SV_AUDIO_H_
#define _SV_AUDIO_H_

#include "sv_header.h"

#include "sv_net.h"
#include "sv_pawn.h"

namespace audio {

	// Базовый объект
	using object = memory::object<0xAABBCCDDDDCCBBAALL>;

	// Базовый ресивер
	class receiver : public virtual object {
	public:
		virtual ~receiver() {};
		virtual void push(Packet *packet) = 0;
	};

	// Базовая группа
	class group : public virtual object {
	public:
		virtual ~group() {};
		virtual void add(void *element) = 0;
		virtual void remove(void *element) = 0;
	};

	// Звуковые эффекты
	namespace effects {

		// Структуры эффектов
		namespace st_param {

			struct st_chorus {
				float fWetDryMix = 50;
				float fDepth = 10;
				float fFeedback = 25;
				float fFrequency = 1.1f;
				uint32_t lWaveform = 1;
				float fDelay = 16;
				uint32_t lPhase = 3;
			};

			struct st_compressor {
				float fGain = 0;
				float fAttack = 10;
				float fRelease = 200;
				float fThreshold = -20;
				float fRatio = 3;
				float fPredelay = 4;
			};

			struct st_distortion {
				float fGain = -18;
				float fEdge = 15;
				float fPostEQCenterFrequency = 2400;
				float fPostEQBandwidth = 2400;
				float fPreLowpassCutoff = 8000;
			};

			struct st_echo {
				float fWetDryMix = 50;
				float fFeedback = 50;
				float fLeftDelay = 500;
				float fRightDelay = 500;
				bool lPanDelay = false;
			};

			struct st_flanger {
				float fWetDryMix = 50;
				float fDepth = 100;
				float fFeedback = -50;
				float fFrequency = 0.25;
				uint32_t lWaveform = 1;
				float fDelay = 2;
				uint32_t lPhase = 2;
			};

			struct st_gargle {
				uint32_t dwRateHz = 20;
				uint32_t dwWaveShape = 0;
			};

			struct st_i3dl2reverb {
				int lRoom = -1000;
				int lRoomHF = -100;
				float flRoomRolloffFactor = 0;
				float flDecayTime = 1.49f;
				float flDecayHFRatio = 0.83f;
				int lReflections = -2602;
				float flReflectionsDelay = 0.007f;
				int lReverb = 200;
				float flReverbDelay = 0.011f;
				float flDiffusion = 100;
				float flDensity = 100;
				float flHFReference = 5000;
			};

			struct st_parameq {
				float fCenter = 1100;
				float fBandwidth = 12;
				float fGain = 0;
			};

			struct st_reverb {
				float fInGain = 0;
				float fReverbMix = 0;
				float fReverbTime = 1000;
				float fHighFreqRTRatio = 0.001f;
			};

		}

		// --------------------------------

		// Интерфейс эффектового класса
		class effectable : public virtual object {
		public:
			virtual ~effectable() {};
			virtual void attach(uint32_t stream, uint16_t player_id) = 0;
			virtual void detach(uint32_t stream, uint16_t player_id) = 0;
		};

		// --------------------------------

		// Базовый эффект
		class effect : public effectable {
		protected:

			// Номер эффекта
			const uint8_t effect_id;

			// Приоритет эффекта
			const int priority;

			// Параметры эффекта
			void *param_ptr;
			uint32_t param_size;

		public:

			// Создать эффект
			effect(
				uint8_t _effect_id,
				int _priority
			) :
				effect_id(_effect_id),
				priority(_priority)
			{}

			// Привязать эффект к потоку игрока
			void attach(
				uint32_t stream,
				uint16_t player_id
			) {
				bitstream bs(e_packet_id::effect_create);
				bs.Write(stream);
				bs.Write((uint32_t)(this));
				bs.Write(this->effect_id);
				bs.Write(this->priority);
				bs.Write((char*)(this->param_ptr), this->param_size);
				net::send(player_id, &bs);
			}

			// Отвязать эффект от потока игрока
			void detach(
				uint32_t stream,
				uint16_t player_id
			) {
				sv_packet::effect_delete st_params = { stream, (uint32_t)(this) };
				net::send(player_id, e_packet_id::effect_delete, &st_params);
			}

			// Удалить эффект
			~effect() {
				std::free(this->param_ptr);
			}

		};

		// Группа эффектов
		class effect_group : public effectable, public group {
		private:

			// Эффекты группы
			std::set<effectable*> effects;

		public:

			// Удалить группу
			~effect_group() {
				for (auto i = this->effects.begin(); i != this->effects.end(); i++) delete (*i);
			}

			// Добавить эффект
			void add(
				void *element
			) {
				this->effects.insert((effectable*)(element));
			}

			// Удалить эффект
			void remove(
				void *element
			) {
				this->effects.erase((effectable*)(element));
			}

			// Привязать поток к группе
			void attach(
				uint32_t stream,
				uint16_t player_id
			) {
				for (auto i = this->effects.begin(); i != this->effects.end(); i++) (*i)->attach(stream, player_id);
			}

			// Отвязать поток от группы
			void detach(
				uint32_t stream,
				uint16_t player_id
			) {
				for (auto i = this->effects.begin(); i != this->effects.end(); i++) (*i)->detach(stream, player_id);
			}

		};

		// --------------------------------

		// Эффект "chorus"
		class chorus : public effect {
		public:

			chorus(
				int priority,
				float fWetDryMix,
				float fDepth,
				float fFeedback,
				float fFrequency,
				uint32_t lWaveform,
				float fDelay,
				uint32_t lPhase
			) : effect(
				e_effect_id::chorus,
				priority
			) {
				this->param_ptr = malloc(this->param_size = sizeof(st_param::st_chorus));
				((st_param::st_chorus*)(this->param_ptr))->fWetDryMix = fWetDryMix;
				((st_param::st_chorus*)(this->param_ptr))->fDepth = fDepth;
				((st_param::st_chorus*)(this->param_ptr))->fFeedback = fFeedback;
				((st_param::st_chorus*)(this->param_ptr))->fFrequency = fFrequency;
				((st_param::st_chorus*)(this->param_ptr))->lWaveform = lWaveform;
				((st_param::st_chorus*)(this->param_ptr))->fDelay = fDelay;
				((st_param::st_chorus*)(this->param_ptr))->lPhase = lPhase;
			}

		};

		// Эффект "compressor"
		class compressor : public effect {
		public:

			compressor(
				int priority,
				float fGain,
				float fAttack,
				float fRelease,
				float fThreshold,
				float fRatio,
				float fPredelay
			) : effect(
				e_effect_id::compressor,
				priority
			) {
				this->param_ptr = malloc(this->param_size = sizeof(st_param::st_compressor));
				((st_param::st_compressor*)(this->param_ptr))->fGain = fGain;
				((st_param::st_compressor*)(this->param_ptr))->fAttack = fAttack;
				((st_param::st_compressor*)(this->param_ptr))->fRelease = fRelease;
				((st_param::st_compressor*)(this->param_ptr))->fThreshold = fThreshold;
				((st_param::st_compressor*)(this->param_ptr))->fRatio = fRatio;
				((st_param::st_compressor*)(this->param_ptr))->fPredelay = fPredelay;
			}

		};

		// Эффект "distortion"
		class distortion : public effect {
		public:

			distortion(
				int priority,
				float fGain,
				float fEdge,
				float fPostEQCenterFrequency,
				float fPostEQBandwidth,
				float fPreLowpassCutoff
			) : effect(
				e_effect_id::distortion,
				priority
			) {
				this->param_ptr = malloc(this->param_size = sizeof(st_param::st_distortion));
				((st_param::st_distortion*)(this->param_ptr))->fGain = fGain;
				((st_param::st_distortion*)(this->param_ptr))->fEdge = fEdge;
				((st_param::st_distortion*)(this->param_ptr))->fPostEQCenterFrequency = fPostEQCenterFrequency;
				((st_param::st_distortion*)(this->param_ptr))->fPostEQBandwidth = fPostEQBandwidth;
				((st_param::st_distortion*)(this->param_ptr))->fPreLowpassCutoff = fPreLowpassCutoff;
			}

		};

		// Эффект "echo"
		class echo : public effect {
		public:

			echo(
				int priority,
				float fWetDryMix,
				float fFeedback,
				float fLeftDelay,
				float fRightDelay,
				bool lPanDelay
			) : effect(
				e_effect_id::echo,
				priority
			) {
				this->param_ptr = malloc(this->param_size = sizeof(st_param::st_echo));
				((st_param::st_echo*)(this->param_ptr))->fWetDryMix = fWetDryMix;
				((st_param::st_echo*)(this->param_ptr))->fFeedback = fFeedback;
				((st_param::st_echo*)(this->param_ptr))->fLeftDelay = fLeftDelay;
				((st_param::st_echo*)(this->param_ptr))->fRightDelay = fRightDelay;
				((st_param::st_echo*)(this->param_ptr))->lPanDelay = lPanDelay;
			}

		};

		// Эффект "flanger"
		class flanger : public effect {
		public:

			flanger(
				int priority,
				float fWetDryMix,
				float fDepth,
				float fFeedback,
				float fFrequency,
				uint32_t lWaveform,
				float fDelay,
				uint32_t lPhase
			) : effect(
				e_effect_id::flanger,
				priority
			) {
				this->param_ptr = malloc(this->param_size = sizeof(st_param::st_flanger));
				((st_param::st_flanger*)(this->param_ptr))->fWetDryMix = fWetDryMix;
				((st_param::st_flanger*)(this->param_ptr))->fDepth = fDepth;
				((st_param::st_flanger*)(this->param_ptr))->fFeedback = fFeedback;
				((st_param::st_flanger*)(this->param_ptr))->fFrequency = fFrequency;
				((st_param::st_flanger*)(this->param_ptr))->lWaveform = lWaveform;
				((st_param::st_flanger*)(this->param_ptr))->fDelay = fDelay;
				((st_param::st_flanger*)(this->param_ptr))->lPhase = lPhase;
			}

		};

		// Эффект "gargle"
		class gargle : public effect {
		public:

			gargle(
				int priority,
				uint32_t dwRateHz,
				uint32_t dwWaveShape
			) : effect(
				e_effect_id::gargle,
				priority
			) {
				this->param_ptr = malloc(this->param_size = sizeof(st_param::st_gargle));
				((st_param::st_gargle*)(this->param_ptr))->dwRateHz = dwRateHz;
				((st_param::st_gargle*)(this->param_ptr))->dwWaveShape = dwWaveShape;
			}

		};

		// Эффект "i3dl2reverb"
		class i3dl2reverb : public effect {
		public:

			i3dl2reverb(
				int priority,
				int lRoom,
				int lRoomHF,
				float flRoomRolloffFactor,
				float flDecayTime,
				float flDecayHFRatio,
				int lReflections,
				float flReflectionsDelay,
				int lReverb,
				float flReverbDelay,
				float flDiffusion,
				float flDensity,
				float flHFReference
			) : effect(
				e_effect_id::i3dl2reverb,
				priority
			) {
				this->param_ptr = malloc(this->param_size = sizeof(st_param::st_i3dl2reverb));
				((st_param::st_i3dl2reverb*)(this->param_ptr))->lRoom = lRoom;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->lRoomHF = lRoomHF;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->flRoomRolloffFactor = flRoomRolloffFactor;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->flDecayTime = flDecayTime;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->flDecayHFRatio = flDecayHFRatio;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->lReflections = lReflections;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->flReflectionsDelay = flReflectionsDelay;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->lReverb = lReverb;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->flReverbDelay = flReverbDelay;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->flDiffusion = flDiffusion;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->flDensity = flDensity;
				((st_param::st_i3dl2reverb*)(this->param_ptr))->flHFReference = flHFReference;
			}

		};

		// Эффект "parameq"
		class parameq : public effect {
		public:

			parameq(
				int priority,
				float fCenter,
				float fBandwidth,
				float fGain
			) : effect(
				e_effect_id::parameq,
				priority
			) {
				this->param_ptr = malloc(this->param_size = sizeof(st_param::st_parameq));
				((st_param::st_parameq*)(this->param_ptr))->fCenter = fCenter;
				((st_param::st_parameq*)(this->param_ptr))->fBandwidth = fBandwidth;
				((st_param::st_parameq*)(this->param_ptr))->fGain = fGain;
			}

		};

		// Эффект "reverb"
		class reverb : public effect {
		public:

			reverb(
				int priority,
				float fInGain,
				float fReverbMix,
				float fReverbTime,
				float fHighFreqRTRatio
			) : effect(
				e_effect_id::reverb,
				priority
			) {
				this->param_ptr = malloc(this->param_size = sizeof(st_param::st_reverb));
				((st_param::st_reverb*)(this->param_ptr))->fInGain = fInGain;
				((st_param::st_reverb*)(this->param_ptr))->fReverbMix = fReverbMix;
				((st_param::st_reverb*)(this->param_ptr))->fReverbTime = fReverbTime;
				((st_param::st_reverb*)(this->param_ptr))->fHighFreqRTRatio = fHighFreqRTRatio;
			}

		};

	}

	/// [DEPEDENCY]
	// Звуки
	namespace sounds {

		/// [DEPEDENCY]
		// Хранилище звуков
		class store {
		private:

			// Таблица звуков
			static std::vector<const char*> table;

		public:

			// Получить url
			static inline const char* url(
				const uint32_t index
			) {
				if (index < table.size()) return table[index];
				else return nullptr;
			}

			/// [DEPEDENCY]
			// Зарегистрировать звук (-1 если не удалось)
			static const uint32_t sound(
				const char *url
			) {
				
				if (HSTREAM meta = BASS_StreamCreateURL(url, 0, BASS_STREAM_DECODE, nullptr, nullptr)) {

					BASS_StreamFree(meta);

					const char *file_url = (const char*)(malloc(strlen(url) + 1));
					memcpy((void*)(file_url), url, strlen(url) + 1);

					table.push_back(file_url);

					if (pNetGame->pPlayerPool->dwConnectedPlayers) {

						bitstream bs(e_packet_id::sound_register);
						bs.Write((uint32_t)(table.size() - 1));
						bs.Write(file_url, strlen(file_url) + 1);

						for (uint32_t i = 0; i < pNetGame->pPlayerPool->dwConnectedPlayers; i++) if (pNetGame->pPlayerPool->bIsPlayerConnected[i]) net::send(i, &bs);

					}

					return table.size() - 1;

				}

				return -1;

			}

			/// [DEPEDENCY]
			// Отправить таблицу игроку
			static void send(
				const uint16_t player_id
			) {
				if (pNetGame->pPlayerPool->bIsPlayerConnected[player_id])
					for (uint32_t i = 0; i < table.size(); i++) {
						bitstream bs(e_packet_id::sound_register);
						bs.Write(i); bs.Write(table[i], strlen(table[i]) + 1);
						if (!net::send(player_id, &bs)) break;
					}
			}

		};

		// Базовый звук
		class sound {
		public:

			// Параметры звука
			const uint32_t index;			// Индекс звука
			const uint32_t start_time;		// Время начала воспроизведения
			const uint32_t end_time;		// Время конца воспроизведения
			const bool loop;				// Статус повторения

			// Параметры файла
			const char *file_url;			// URL аудиофайла
			const uint32_t url_size;		// Длина строки URL
			const uint32_t file_size;		// Размер (в байтах)
			const uint32_t file_time;		// Длительность (в миллисекундах)

		protected:

			sound(
				const uint32_t _start_time,
				const uint32_t _end_time,
				const bool _loop,
				const char *_file_url,
				const uint32_t _url_size,
				const uint32_t _file_size,
				const uint32_t _file_time,
				const uint32_t _index
			) :
				start_time(_start_time),
				end_time(_end_time),
				loop(_loop),
				file_url(_file_url),
				url_size(_url_size),
				file_size(_file_size),
				file_time(_file_time),
				index(_index)
			{}

		public:

			// Проверить корректность url
			static inline const bool check(
				const char *url
			) {
				if (HSTREAM meta = BASS_StreamCreateURL(url, 0, BASS_STREAM_DECODE, nullptr, nullptr)) {
					BASS_StreamFree(meta); return true;
				} else return false;
			}

			// Создать внешний звук
			static sound* create(
				const char *_url,
				const bool _loop
			) {
				
				char *file_url;
				uint32_t start_time, end_time, url_size, file_size, file_time;

				file_url = (char*)(malloc(url_size = strlen(_url) + 1));
				memcpy((void*)(file_url), _url, url_size);

				if (HSTREAM meta = BASS_StreamCreateURL(file_url, 0, BASS_STREAM_DECODE, nullptr, nullptr)) {
					file_time = 1000 * BASS_ChannelBytes2Seconds(meta, BASS_ChannelGetLength(meta, BASS_POS_BYTE));
					file_size = BASS_StreamGetFilePosition(meta, BASS_FILEPOS_END);
					BASS_StreamFree(meta); end_time = (start_time = clock()) + file_time;
					return new sound(start_time, end_time, _loop, file_url, url_size, file_size, file_time, -1);
				}

				return nullptr;

			}

			// Создать внутренний звук
			static sound* create(
				const uint32_t index,
				const bool _loop
			) {

				uint32_t start_time, end_time, file_size, file_time;

				if (const char *url = store::url(index))
					if (HSTREAM meta = BASS_StreamCreateURL(url, 0, BASS_STREAM_DECODE, nullptr, nullptr)) {
						file_time = 1000 * BASS_ChannelBytes2Seconds(meta, BASS_ChannelGetLength(meta, BASS_POS_BYTE));
						file_size = BASS_StreamGetFilePosition(meta, BASS_FILEPOS_END);
						BASS_StreamFree(meta); end_time = (start_time = clock()) + file_time;
						return new sound(start_time, end_time, _loop, url, strlen(url) + 1, file_size, file_time, index);
					}

				return nullptr;

			}

			// Проверить активность потока
			inline const bool status(
				const uint32_t time
			) {
				return (this->loop || time < this->end_time);
			}

			// Получить смещение аудиофайла (в байтах)
			inline const uint32_t offset(
				const uint32_t time
			) {
				return this->file_size * ((float)((time - this->start_time) % this->file_time) / (float)this->file_time);
			}

			// Сформировать пакет
			inline BitStream* packet(
				const uint32_t stream_id,
				const uint32_t current_time
			) {
				BitStream *bs = new BitStream;
				bs->Write((uint8_t)(SV_NET_PACKET_ID));
				const bool stat = (this->index == -1);
				if (stat) bs->Write((uint8_t)(e_packet_id::stream_play_sound_external));
				else bs->Write((uint8_t)(e_packet_id::stream_play_sound_internal));
				bs->Write(stream_id); bs->Write(this->offset(current_time));
				if (stat) {
					bs->Write((uint8_t)(this->loop));
					bs->Write(this->file_url, this->url_size);
				} else {
					bs->Write(this->index);
					bs->Write((uint8_t)(this->loop));
				} return bs;
			}

			~sound() {
				std::free((void*)(this->file_url));
			}

		};

		std::vector<const char*> store::table;

	}

	/// [DEPEDENCY]
	// Звуковые потоки
	namespace streams {

		// Интерфейс потокового класса
		class streamable : public receiver {
		public:
			virtual ~streamable() {};
			virtual void set_parameter(uint8_t param_id, float value) = 0;
			virtual void slide_parameter(uint8_t param_id, float start_value, float end_value, uint32_t time) = 0;
			virtual void effect_attach(effects::effectable* effect) = 0;
			virtual void effect_detach(effects::effectable* effect) = 0;
			virtual bool play_sound(uint32_t index, bool loop) = 0;
			virtual bool play_sound(const char *url, bool loop) = 0;
			virtual void stop_sound() = 0;
			virtual void player_attach(uint16_t player_id) = 0;
			virtual void player_detach(uint16_t player_id) = 0;
		};

		// --------------------------------

		// Базовый поток
		class stream : public streamable {
		private:

			// Информацию о скользящем параметре
			struct st_slide_info {
				float ratio;
				uint32_t end_time;
				float end_value;
			};

			// Параметры потока
			std::map<uint8_t, float> parameters;

			// Скользящие параметры потока
			std::map<uint8_t, st_slide_info> slide_parameters;

			// Эффекты привязанные к потоку
			std::set<effects::effectable*> effects;

			// Звук потока
			sounds::sound *sound = nullptr;

		protected:

			// Игроки привязанные к потоку
			std::set<uint16_t> players;

			// Отправить игроку образ потока
			void send_meta(
				uint16_t player_id
			) {
				
				uint32_t current_time = clock();

				// Проверка скользящих параметров
				if (!this->slide_parameters.empty()) {
					uint32_t diff_time;
					for (auto i = this->slide_parameters.begin(); i != this->slide_parameters.end(); i++)
						if ((diff_time = (*i).second.end_time - current_time) > 0) {
							sv_packet::stream_slide_parameter st_params = {
								(uint32_t)(this),
								(*i).first,
								(*i).second.end_value - ((*i).second.ratio * diff_time),
								(*i).second.end_value,
								diff_time
							}; net::send(player_id, e_packet_id::stream_slide_parameter, &st_params);
						} else this->slide_parameters.erase(i);
				}

				// Проверка параметров
				if(!this->parameters.empty())
					for (auto i = this->parameters.begin(); i != this->parameters.end(); i++)
						if (this->slide_parameters.find((*i).first) == this->slide_parameters.end()) {
							sv_packet::stream_set_parameter st_params = {
								(uint32_t)(this),
								(*i).first,
								(*i).second
							}; net::send(player_id, e_packet_id::stream_set_parameter, &st_params);
						}

				// Проверка эффектов
				if (!this->effects.empty())
					for (auto i = this->effects.begin(); i != this->effects.end(); i++)
						(*i)->attach((uint32_t)this, player_id);

				// Проверка звука
				if (this->sound && this->sound->status(current_time)) {
					BitStream *bs = this->sound->packet((uint32_t)(this), current_time);
					net::send(player_id, bs);
					delete bs;
				}

			}

		public:

			// Удалить поток
			~stream() {
				
				delete sound;

				if (!this->players.empty()) {
					sv_packet::stream_delete st_params = { (uint32_t)(this) };
					bitstream bs(e_packet_id::stream_delete, &st_params, sizeof(st_params));
					for (auto i = this->players.begin(); i != this->players.end(); i++)
						net::send(*i, &bs);
				}

			}

			// Отправить пакет в поток
			void push(
				Packet *packet
			) {
				if (!this->players.empty()) {
					BitStream bs(packet->data, packet->length, false);
					((sv_packet::header*)(packet->data))->get<sv_packet::voice>()->stream = (uint32_t)(this);
					for (auto i = this->players.begin(); i != this->players.end(); i++) net::send(*i, &bs);
				}
			}

			// Установить параметр
			void set_parameter(
				uint8_t param_id,
				float value
			) {

				this->parameters[param_id] = value;
				this->slide_parameters.erase(param_id);

				if (!this->players.empty()) {
					sv_packet::stream_set_parameter st_params = { (uint32_t)(this), param_id, value };
					bitstream bs(e_packet_id::stream_set_parameter, &st_params, sizeof(st_params));
					for (auto i = this->players.begin(); i != this->players.end(); i++)
						net::send(*i, &bs);
				}

			}

			// Задать скольжение параметра
			void slide_parameter(
				uint8_t param_id,
				float start_value,
				float end_value,
				uint32_t time
			) {

				this->parameters[param_id] = end_value;
				this->slide_parameters[param_id] = {
					(end_value - start_value) / time,
					clock() + time,
					end_value
				};

				if (!this->players.empty()) {
					sv_packet::stream_slide_parameter st_params = { (uint32_t)(this), param_id, start_value, end_value, time };
					bitstream bs(e_packet_id::stream_slide_parameter, &st_params, sizeof(st_params));
					for (auto i = this->players.begin(); i != this->players.end(); i++)
						net::send(*i, &bs);
				}

			}

			// Привязать эффект к потоку
			void effect_attach(
				effects::effectable* target
			) {
				if (this->effects.insert(target).second)
					for (auto i = this->players.begin(); i != this->players.end(); i++)
						target->attach((uint32_t)(this), *i);
			}

			// Отвязать эффект от потока
			void effect_detach(
				effects::effectable* target
			) {
				if (this->effects.erase(target))
					for (auto i = this->players.begin(); i != this->players.end(); i++)
						target->detach((uint32_t)(this), *i);
			}

			// Воспроизвести звук из хранилища
			bool play_sound(
				uint32_t index,
				bool loop
			) {

				if (!this->sound && (this->sound = sounds::sound::create(index, loop))) {

					if (!this->players.empty()) {
						BitStream *bs = this->sound->packet((uint32_t)(this), clock());
						for (auto i = this->players.begin(); i != this->players.end(); i++) net::send(*i, bs);
						delete bs;
					}

					return true;

				}

				return false;

			}

			// Воспроизвести звук по url
			bool play_sound(
				const char *url,
				bool loop
			) {

				if (!this->sound && (this->sound = sounds::sound::create(url, loop))) {

					if (!this->players.empty()) {
						BitStream *bs = this->sound->packet((uint32_t)(this), clock());
						for (auto i = this->players.begin(); i != this->players.end(); i++) net::send(*i, bs);
						delete bs;
					}

					return true;

				}

				return false;

			}

			// Остановить воспроизведение
			void stop_sound() {

				if (this->sound) {

					delete this->sound;
					this->sound = nullptr;

					if (!this->players.empty()) {
						sv_packet::stream_stop_audio st_params = { (uint32_t)(this) };
						bitstream bs(e_packet_id::stream_stop_audio, &st_params, sizeof(st_params));
						for (auto i = this->players.begin(); i != this->players.end(); i++)
							net::send(*i, &bs);
					}

				}

			}

		};

		// Группа потоков
		class stream_group : public streamable, public group {
		private:

			// Потоки группы
			std::set<streamable*> streams;

		public:

			// Удалить группу
			~stream_group() {
				for (auto i = this->streams.begin(); i != this->streams.end(); i++) delete (*i);
			}

			// Добавить поток
			void add(
				void *element
			) {
				this->streams.insert((streamable*)(element));
			}

			// Удалить поток
			void remove(
				void *element
			) {
				this->streams.erase((streamable*)(element));
			}

			// Отправить пакет в группу
			void push(
				Packet *packet
			) {
				for (auto i = this->streams.begin(); i != this->streams.end(); i++)
					(*i)->push(packet);
			}

			// Установить параметр
			void set_parameter(
				uint8_t param_id,
				float value
			) {
				for (auto i = this->streams.begin(); i != this->streams.end(); i++)
					(*i)->set_parameter(param_id, value);
			}

			// Задать скольжение параметра
			void slide_parameter(
				uint8_t param_id,
				float start_value,
				float end_value,
				uint32_t time
			) {
				for (auto i = this->streams.begin(); i != this->streams.end(); i++)
					(*i)->slide_parameter(param_id, start_value, end_value, time);
			}

			// Привязать эффект к потоку
			void effect_attach(
				effects::effectable* target
			) {
				for (auto i = this->streams.begin(); i != this->streams.end(); i++)
					(*i)->effect_attach(target);
			}

			// Отвязать эффект от потока
			void effect_detach(
				effects::effectable* target
			) {
				for (auto i = this->streams.begin(); i != this->streams.end(); i++)
					(*i)->effect_detach(target);
			}

			// Воспроизвести аудио из хранилища
			bool play_sound(
				uint32_t index,
				bool loop
			) {

				const char *url = sounds::store::url(index);
				if (url && sounds::sound::check(url)) {
					for (auto i = this->streams.begin(); i != this->streams.end(); i++) (*i)->play_sound(loop, index);
					return true;
				}

				return false;

			}

			// Воспроизвести аудио по url
			bool play_sound(
				const char *url,
				bool loop
			) {

				if (sounds::sound::check(url)) {
					for (auto i = this->streams.begin(); i != this->streams.end(); i++) (*i)->play_sound(loop, url);
					return true;
				}

				return false;

			}

			// Остановить воспроизведение
			void stop_sound() {
				for (auto i = this->streams.begin(); i != this->streams.end(); i++)
					(*i)->stop_sound();
			}

			// Привязать игрока к группе
			void player_attach(
				uint16_t player_id
			) {
				for (auto i = this->streams.begin(); i != this->streams.end(); i++)
					(*i)->player_attach(player_id);
			}

			// Отвязать игрока от группы
			void player_detach(
				uint16_t player_id
			) {
				for (auto i = this->streams.begin(); i != this->streams.end(); i++)
					(*i)->player_detach(player_id);
			}

		};

		// --------------------------------

		// Статический поток
		class stream_static : public stream {
		public:

			// Отвязать игрока от потока
			void player_detach(
				uint16_t player_id
			) {
				if (this->players.erase(player_id)) {
					sv_packet::stream_delete st_params = { (uint32_t)(this) };
					net::send(player_id, e_packet_id::stream_delete, &st_params);
				}
			}

		};

		// Динамический поток
		class stream_dynamic : public stream {
		public:

			// Проверить вхождение игрока в стрим объекта
			virtual bool is_in_stream(
				uint16_t player_id
			) = 0;

			// Привязать игрока к потоку
			virtual void _player_attach(
				uint16_t player_id
			) = 0;

			// Отвязать игрока от потока
			void _player_detach(
				uint16_t player_id
			) {
				if (this->players.erase(player_id)) {
					sv_packet::stream_delete st_params = { (uint32_t)(this) };
					net::send(player_id, e_packet_id::stream_delete, &st_params);
				}
			}

			// [API] Привязать игрока к потоку
			void player_attach(
				uint16_t player_id
			) {}

			// [API] Отвязать игрока от потока
			void player_detach(
				uint16_t player_id
			) {}

		};

		// --------------------------------

		// Статический глобальный поток
		class stream_static_global : public stream_static {
		public:

			// Привязать игрока к потоку
			void player_attach(
				uint16_t player_id
			) {
				if (this->players.insert(player_id).second) {
					sv_packet::gstream_create st_params = { (uint32_t)(this) };
					if (net::send(player_id, e_packet_id::gstream_create, &st_params))
						this->send_meta(player_id);
				}
			}

		};

		// --------------------------------

		// Статический локальный поток с привязкой к точке
		class stream_static_local_at_point : public stream_static {
		private:

			// Параметры потока
			CVector position;		// Позиция потока
			CVector orientation;	// Направление потока
			CVector velocity;		// Скорость потока

		public:

			// Создать поток
			stream_static_local_at_point(
				float px, float py, float pz,
				float ox, float oy, float oz,
				float vx, float vy, float vz
			) :
				position({ px, py, pz }),
				orientation({ ox, oy, oz }),
				velocity({ vx, vy, vz })
			{}

			// Привязать игрока к потоку
			void player_attach(
				uint16_t player_id
			) {

				if (this->players.insert(player_id).second) {

					sv_packet::slstream_params st_params = {
						(uint32_t)(this),
						this->position.fX, this->position.fY, this->position.fZ,
						this->orientation.fX, this->orientation.fY, this->orientation.fZ,
						this->velocity.fX, this->velocity.fY, this->velocity.fZ
					};

					if (net::send(player_id, e_packet_id::lstream_create_at_point, &st_params))
						this->send_meta(player_id);

				}

			}

			// Обновить позицию потока
			void update(
				float px, float py, float pz,
				float ox, float oy, float oz,
				float vx, float vy, float vz
			) {

				this->position = { px, py, pz };
				this->orientation = { ox, oy, oz };
				this->velocity = { vx, vy, vz };

				if (!this->players.empty()) {
					sv_packet::slstream_params st_params = { (uint32_t)(this), px, py, pz, ox, oy, oz, vx, vy, vz };
					bitstream bs(e_packet_id::lstream_update, &st_params, sizeof(st_params));
					for (auto i = this->players.begin(); i != this->players.end(); i++)
						net::send(*i, &bs);
				}

			}

		};

		// Статический поток с привязкой к автомобилю
		class stream_static_local_at_vehicle : public stream_static {
		private:

			// ID автомобиля привязки
			const uint16_t vehicle_id;

		public:

			// Создать поток
			stream_static_local_at_vehicle(
				uint16_t _vehicle_id
			) :
				vehicle_id(_vehicle_id)
			{}

			// Привязать игрока к потоку
			void player_attach(
				uint16_t player_id
			) {
				if (this->players.insert(player_id).second) {
					sv_packet::dlstream_create st_params = { (uint32_t)(this), this->vehicle_id };
					if (net::send(player_id, e_packet_id::lstream_create_at_vehicle, &st_params))
						this->send_meta(player_id);
				}
			}

		};

		// Статический поток с привязкой к игроку
		class stream_static_local_at_player : public stream_static {
		private:

			// ID игрока привязки
			const uint16_t player_id;

		public:

			// Создать поток
			stream_static_local_at_player(
				uint16_t _player_id
			) :
				player_id(_player_id)
			{}

			// Привязать игрока к потоку
			void player_attach(
				uint16_t player_id
			) {
				if (this->players.insert(player_id).second) {
					sv_packet::dlstream_create st_params = { (uint32_t)(this), this->player_id };
					if (net::send(player_id, e_packet_id::lstream_create_at_player, &st_params))
						this->send_meta(player_id);
				}
			}

		};

		// Статический поток с привязкой к объекту
		class stream_static_local_at_object : public stream_static {
		private:

			// ID объекта привязки
			const uint16_t object_id;

		public:

			// Создать поток
			stream_static_local_at_object(
				uint16_t _object_id
			) :
				object_id(_object_id)
			{}

			// Привязать игрока к потоку
			void player_attach(
				uint16_t player_id
			) {
				if (this->players.insert(player_id).second) {
					sv_packet::dlstream_create st_params = { (uint32_t)(this), this->object_id };
					if (net::send(player_id, e_packet_id::lstream_create_at_object, &st_params))
						this->send_meta(player_id);
				}
			}

		};

		// --------------------------------

		// Динамический локальный поток с привязкой к точке
		class stream_dynamic_local_at_point : public stream_dynamic {
		private:

			// Параметры потока
			CVector position;		// Позиция потока
			CVector orientation;	// Направление потока
			CVector velocity;		// Скорость потока

		public:

			// Создать поток
			stream_dynamic_local_at_point(
				float px, float py, float pz,
				float ox, float oy, float oz,
				float vx, float vy, float vz
			) :
				position({ px, py, pz }),
				orientation({ ox, oy, oz }),
				velocity({ vx, vy, vz })
			{
				if (pNetGame->pPlayerPool->dwConnectedPlayers)
					for (uint16_t i = 0; i <= pNetGame->pPlayerPool->dwPlayerPoolSize; i++)
						if (this->is_in_stream(i)) this->_player_attach(i);
			}

			// Проверить статус
			bool is_in_stream(
				const uint16_t player_id
			) {
				return pNetGame->pPlayerPool->pPlayer[player_id] && ((pNetGame->pPlayerPool->pPlayer[player_id]->vecPosition - position).Length() < stream_distance);
			}

			// Привязать игрока к потоку
			void _player_attach(
				uint16_t player_id
			) {

				if (this->players.insert(player_id).second) {

					sv_packet::slstream_params st_params = {
						(uint32_t)(this),
						this->position.fX, this->position.fY, this->position.fZ,
						this->orientation.fX, this->orientation.fY, this->orientation.fZ,
						this->velocity.fX, this->velocity.fY, this->velocity.fZ
					};

					if (net::send(player_id, e_packet_id::lstream_create_at_point, &st_params))
						this->send_meta(player_id);

				}

			}

			// Обновить позицию потока
			void update(
				float px, float py, float pz,
				float ox, float oy, float oz,
				float vx, float vy, float vz
			) {

				this->position = { px, py, pz };
				this->orientation = { ox, oy, oz };
				this->velocity = { vx, vy, vz };

				if (!this->players.empty()) {
					sv_packet::slstream_params st_params = { (uint32_t)(this), px, py, pz, ox, oy, oz, vx, vy, vz };
					bitstream bs(e_packet_id::lstream_update, &st_params, sizeof(st_params));
					for (auto i = this->players.begin(); i != this->players.end(); i++)
						net::send(*i, &bs);
				}

			}

		};

		// Динамический поток с привязкой к автомобилю
		class stream_dynamic_local_at_vehicle : public stream_dynamic {
		public:

			// ID автомобиля привязки
			const uint16_t vehicle_id;

			// Создать поток
			stream_dynamic_local_at_vehicle(
				uint16_t _vehicle_id
			) :
				vehicle_id(_vehicle_id)
			{
				if (pNetGame->pPlayerPool->dwConnectedPlayers)
					for (uint16_t i = 0; i <= pNetGame->pPlayerPool->dwPlayerPoolSize; i++)
						if (this->is_in_stream(i)) this->_player_attach(i);
			}

			// Проверить статус
			bool is_in_stream(
				uint16_t player_id
			) {
				return pNetGame->pPlayerPool->pPlayer[player_id] && pNetGame->pPlayerPool->pPlayer[player_id]->byteVehicleStreamedIn[vehicle_id];
			}

			// Привязать игрока к потоку
			void _player_attach(
				uint16_t player_id
			) {
				if (this->players.insert(player_id).second) {
					sv_packet::dlstream_create st_params = { (uint32_t)(this), this->vehicle_id };
					if (net::send(player_id, e_packet_id::lstream_create_at_vehicle, &st_params))
						this->send_meta(player_id);
				}
			}

		};

		// Динамический поток с привязкой к игроку
		class stream_dynamic_local_at_player : public stream_dynamic {
		public:

			// ID игрока привязки
			const uint16_t player_id;

			// Создать поток
			stream_dynamic_local_at_player(
				uint16_t _player_id
			) :
				player_id(_player_id)
			{
				if (pNetGame->pPlayerPool->dwConnectedPlayers)
					for (uint16_t i = 0; i <= pNetGame->pPlayerPool->dwPlayerPoolSize; i++)
						if (this->is_in_stream(i)) this->_player_attach(i);
			}

			// Проверить статус
			bool is_in_stream(
				uint16_t player_id
			) {
				return pNetGame->pPlayerPool->pPlayer[player_id] && pNetGame->pPlayerPool->pPlayer[player_id]->byteStreamedIn[this->player_id];
			}

			// Привязать игрока к потоку
			void _player_attach(
				uint16_t player_id
			) {
				if (this->players.insert(player_id).second) {
					sv_packet::dlstream_create st_params = { (uint32_t)(this), this->player_id };
					if (net::send(player_id, e_packet_id::lstream_create_at_player, &st_params))
						this->send_meta(player_id);
				}
			}

		};

		// Динамический поток с привязкой к объекту
		class stream_dynamic_local_at_object : public stream_dynamic {
		public:

			// ID объекта привязки
			const uint16_t object_id;

			// Создать поток
			stream_dynamic_local_at_object(
				uint16_t _object_id
			) :
				object_id(_object_id)
			{
				if (pNetGame->pPlayerPool->dwConnectedPlayers)
					for (uint16_t i = 0; i <= pNetGame->pPlayerPool->dwPlayerPoolSize; i++)
						if (this->is_in_stream(i)) this->_player_attach(i);
			}

			// Проверить статус
			bool is_in_stream(
				uint16_t player_id
			) {
				return pNetGame->pObjectPool->bPlayerObjectSlotState[player_id][this->object_id];
			}

			// Привязать игрока к потоку
			void _player_attach(
				uint16_t player_id
			) {
				if (this->players.insert(player_id).second) {
					sv_packet::dlstream_create st_params = { (uint32_t)(this), this->object_id };
					if (net::send(player_id, e_packet_id::lstream_create_at_object, &st_params))
						this->send_meta(player_id);
				}
			}

		};

	}

	// Звуковой буфер
	class buffer : public receiver {
	private:

		// Потоки к которым привязан буфер
		std::forward_list<streams::streamable*> streams;

		// Пакеты сохранённые в буфере
		std::vector<Packet*> packets;

		// Поток трансляции
		volatile bool status_broadcast = false;
		static void func_thread_broadcast(
			buffer *_this,
			bool loop
		) {
			
			do {
				for (auto i = _this->packets.begin(); i != _this->packets.end(); i++) {
					if (!_this->status_broadcast) break;
					for (auto j = _this->streams.begin(); j != _this->streams.end(); j++) (*j)->push(*i);
					std::this_thread::sleep_for(std::chrono::milliseconds(settings.voice_rate));
				}
			} while (_this->status_broadcast && loop);

			// Если трансляция закончилась естественно
			if (_this->status_broadcast) {

				// Вызов паблика OnBufferEnd
				if (!pawn::script::get_scripts_obe()->empty())
					for (auto i = pawn::script::get_scripts_obe()->begin(); i != pawn::script::get_scripts_obe()->end(); i++)
						(*i).onbufferend((uint32_t)(_this));

				_this->status_broadcast = false;

			}

		}

	public:

		// Удалить буфер
		~buffer() {
			this->stop();
			this->clear();
		}

		// Сохранить пакет
		void push(
			Packet *packet
		) {
			if (!this->status_broadcast) this->packets.push_back(packet);
		}

		// Очистить буфер
		void clear() {
			if (!this->status_broadcast) {
				for (auto i : this->packets) i->deleteData = true;
				this->packets.clear();
			}
		}

		// Сохранить буфер в файл
		bool save(
			const char *filename
		) {

			if (FILE *output = fopen(filename, "wb")) {

				uint32_t packet_count = this->packets.size();
				fwrite(&packet_count, sizeof(packet_count), 1, output);

				for (auto i = this->packets.begin(); i != this->packets.end(); i++) {
					fwrite(&(*i)->playerIndex, sizeof(PlayerIndex), 1, output);
					fwrite(&(*i)->playerId, sizeof(PlayerID), 1, output);
					fwrite(&(*i)->length, sizeof(uint32_t), 1, output);
					fwrite((*i)->data, sizeof(uint8_t), (*i)->length, output);
				}

				fflush(output);
				fclose(output);
				return true;

			}

			return false;

		}

		// Загрузить буфер из файла
		bool load(
			const char *filename
		) {

			if (this->status_broadcast) return false;

			if (FILE *input = fopen(filename, "rb")) {

				Packet *packet;

				for (auto i : this->packets) i->deleteData = true;
				this->packets.clear();

				uint32_t packet_count;
				fread(&packet_count, sizeof(packet_count), 1, input);

				for (uint32_t i = 0; i < packet_count; i++) {

					packet = new Packet;

					fread(&packet->playerIndex, sizeof(PlayerIndex), 1, input);
					fread(&packet->playerId, sizeof(PlayerID), 1, input);
					fread(&packet->length, sizeof(uint32_t), 1, input);

					packet->data = reinterpret_cast<uint8_t*>(malloc(packet->length));
					fread(packet->data, sizeof(uint8_t), packet->length, input);

					packet->bitSize = packet->length << 3;
					packet->deleteData = false;

					this->packets.push_back(packet);

				}

				fclose(input);
				return true;

			}

			return false;

		}

		// Привязать буфер к потоку
		void attach(
			streams::streamable* stream
		) {
			if (!this->status_broadcast) this->streams.push_front(stream);
		}

		// Отвязать буфер от потока
		void detach(
			streams::streamable* stream
		) {
			if (!this->status_broadcast) this->streams.remove(stream);
		}

		// Отвязать буфер от всех потоков
		void detach_all() {
			if (!this->status_broadcast) this->streams.clear();
		}

		// Начать трансляцию пакетов
		void play(
			bool loop
		) {

			if (!this->status_broadcast && !this->streams.empty() && !this->packets.empty()) {
				this->status_broadcast = true;
				std::thread thr_bc(func_thread_broadcast, this, loop);
				thr_bc.detach();
			}

		}

		// Остановить трансляцию пакетов
		void stop() {
			this->status_broadcast = false;
		}

	};

}

#endif