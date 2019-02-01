#pragma once

#include <map>
#include <forward_list>
#include <vector>
#include <string>
#include <functional>

#include <audio/bass.h>
#include <audio/opus.h>

#pragma comment(lib, "audio/bass.lib")
#pragma comment(lib, "audio/opus.lib")

#include "logger.h"

namespace audio {

	// Аудиокодек OPUS
	class opus {
	private:

		// Статус загрузки
		static bool status;

		// Энкодер и декодер
		static OpusEncoder* encoder;
		static OpusDecoder* decoder;

		// Параметры кодека
		static uint32_t frequency;
		static uint32_t frame_size;
		static uint32_t bitrate;

	public:

		// Инициализировать кодек
		static bool init(
			uint32_t frequency,					// Звуковая частота (в герцах)
			uint32_t frame_size,				// Размер кадра (в сэмплах)
			uint32_t bitrate					// Битрейт кодека (в бит/с)
		) {

			int error;

			if (opus::status) return false;
			opus::frame_size = frame_size;

			if (frequency != 8000u && frequency != 12000u && frequency != 16000u && frequency != 24000u && frequency != 48000u) {
				LogError("opus", "invalid arguments in function init");
				return false;
			}

			opus::encoder = opus_encoder_create(opus::frequency = frequency, 1, OPUS_APPLICATION_VOIP, &error);
			if (error < 0) {
				LogError("opus", "could not create encoder (core:%d)", error);
				opus::free();
				return false;
			}

			error = opus_encoder_ctl(opus::encoder, OPUS_SET_BITRATE(opus::bitrate = bitrate));
			if (error < 0) {
				LogError("opus", "could not set bitrate for encoder (core:%d)", error);
				opus::free();
				return false;
			}

			error = opus_encoder_ctl(opus::encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
			if (error < 0) {
				LogError("opus", "could not set audiosignal type for encoder (core:%d)", error);
				opus::free();
				return false;
			}

			error = opus_encoder_ctl(opus::encoder, OPUS_SET_COMPLEXITY(10));
			if (error < 0) {
				LogError("opus", "could not set complexity for encoder (core:%d)", error);
				opus::free();
				return false;
			}

			error = opus_encoder_ctl(opus::encoder, OPUS_SET_PREDICTION_DISABLED(0));
			if (error < 0) {
				LogError("opus", "could not enable prediction for encoder (core:%d)", error);
				opus::free();
				return false;
			}

			error = opus_encoder_ctl(opus::encoder, OPUS_SET_LSB_DEPTH(16));
			if (error < 0) {
				LogError("opus", "could not set lsb depth for encoder (core:%d)", error);
				opus::free();
				return false;
			}

			error = opus_encoder_ctl(opus::encoder, OPUS_SET_FORCE_CHANNELS(1));
			if (error < 0) {
				LogError("opus", "could not set count channels for encoder (core:%d)", error);
				opus::free();
				return false;
			}

			opus::decoder = opus_decoder_create(opus::frequency, 1, &error);
			if (error < 0) {
				LogError("opus", "could not create decoder (core:%d)", error);
				opus::free();
				return false;
			}

			return opus::status = true;

		}

		// Упаковать данные
		static inline int encode(
			void * const src,
			void * const dst,
			const uint32_t max_len
		) {
			return (opus::status ? opus_encode(opus::encoder, (opus_int16*)(src), opus::frame_size, (uint8_t*)(dst), max_len) : NULL);
		}

		// Распаковать данные
		static inline int decode(
			void * const src,
			const uint32_t length,
			void * const dst
		) {
			return (opus::status ? opus_decode(opus::decoder, (uint8_t*)(src), length, (opus_int16*)(dst), opus::frame_size, 0) : NULL);
		}

		// Выгрузить кодек
		static void free() {
			if (opus::status) {
				opus_encoder_destroy(opus::encoder);
				opus_decoder_destroy(opus::decoder);
				opus::status = false;
			}
		}

	};

	// Подмодуль воспроизведения
	namespace playback {

		// Звуковые файлы
		namespace sounds {

			// Хранилище файлов
			class store {
			private:

				struct st_table_info {
					const char *url;
					uint32_t last_time;
					bool is_download;
				};

				struct st_download_info {
					const uint32_t index;
					const char *url;
				};

				static uint32_t limit;	// Лимит хранилища
				static uint32_t size;	// Текущий размер

				static std::vector<st_table_info*> table;

			public:

				// Получить статус файла
				static inline bool is_valid(
					uint32_t index
				) {
					return (table[index] && !table[index]->is_download);
				}

				// Получить путь к файлу хранилища
				static inline const char* pmake(
					uint32_t index
				) {

					static char buffer[64] = SV_PATH_STORE "/" SV_NAME_PREFIX;
					snprintf(
						&buffer[sizeof(SV_PATH_STORE "/" SV_NAME_PREFIX) - 1],
						sizeof(buffer) - (sizeof(SV_PATH_STORE "/" SV_NAME_PREFIX) - 1),
						"%u", index
					);

					return buffer;

				}

				// Получить размер файла в хранилище
				static inline const uint32_t fsize(
					uint32_t index
				) {

					uint32_t result = 0;
					if (FILE *file = fopen(pmake(index), "rb")) {
						fseek(file, 0, SEEK_END);
						result = ftell(file);
						fclose(file);
					}

					return result;

				}

				// Освободить request_size байт в хранилище
				static inline void fclear(
					uint32_t request_size
				) {

					request_size += store::size;

					uint32_t min_index;
					while (request_size > limit) {

						min_index = 0;
						for (uint32_t i = 0; i < table.size(); i++)
							if (table[i] && table[i]->last_time < table[min_index]->last_time)
								min_index = i;

						request_size -= fsize(min_index);
						std::free((void*)(table[min_index]->url));
						if (!table[min_index]->is_download) std::remove(pmake(min_index));
						
						delete table[min_index];
						table[min_index] = nullptr;

					}

				}

				// Получить свободный индекс
				static inline const uint32_t fget() {

					for (uint32_t i = 0; i < table.size(); i++)
						if (!table[i]) {
							table[i] = new st_table_info;
							return i;
						}

					table.push_back(new st_table_info);
					return table.size() - 1;

				}

			private:

				// Процедура загрузки файла
				static DWORD WINAPI download_proc(
					LPVOID _info
				) {

#define info ((st_download_info*)(_info))

					FILE *file;
					HSTREAM stream;
					uint32_t length;
					uint8_t buffer[SV_AUDIO_NET_BUFFER];

					// Открываем соединение
					if (!(stream = BASS_StreamCreateURL(info->url, 0, BASS_SAMPLE_MONO | BASS_STREAM_DECODE | BASS_STREAM_BLOCK, nullptr, nullptr))) {
						std::free(_info);
						return 0;
					}

					table[info->index]->is_download = true;

					table[info->index]->url = (const char*)malloc(strlen(info->url) + 1);
					strcpy((char*)(table[info->index]->url), info->url);

					if (!(file = fopen(pmake(info->index), "wb"))) {
						table[info->index]->is_download = false;
						std::free((void*)(table[info->index]->url));
						BASS_StreamFree(stream);
						std::free(_info);
						return 0;
					}

					// Загружаем файл
					while ((length = BASS_ChannelGetData(stream, buffer, SV_AUDIO_NET_BUFFER)) != -1)
						if (!fwrite(buffer, length, 1, file)) {
							table[info->index]->is_download = false;
							std::free((void*)(table[info->index]->url));
							BASS_StreamFree(stream);
							std::free(_info);
							fclose(file);
							return 0;
						}

					BASS_StreamFree(stream);
					fclose(file);

					table[info->index]->is_download = false;
					if (length == -1) {
						std::free((void*)(table[info->index]->url));
						std::remove(pmake(info->index));
						delete table[info->index];
						table[info->index] = nullptr;
					}

					std::free(_info);
					return 0;

#undef info

				}

			public:

				// Инициализировать хранилище
				static bool init(
					uint32_t store_limit
				) {
					
					uint32_t count;
					uint32_t *index = nullptr;
					uint32_t temp1 = 0, temp2;

					store::size = 0;
					store::limit = store_limit;

					if (FILE *table_file = fopen(SV_PATH_STORE_TABLE, "rb")) {

						if (!fread(&count, sizeof(count), 1, table_file)) {
							fclose(table_file);
							return false;
						}

						if (count) {

							if (!(index = (uint32_t*)malloc(count * sizeof(uint32_t))) ||
								(fread(index, sizeof(uint32_t), count, table_file) != count)
							) {
								if (index) std::free(index);
								fclose(table_file);
								return false;
							}

							for (uint32_t i = 0; i < count; i++)
								if (temp1 < index[i]) temp1 = index[i];

							table.resize(temp1 + 1);
							std::fill(table.begin(), table.end(), nullptr);

							for (uint32_t i = 0; i < count; i++) {

								if (!fread(&temp1, sizeof(temp1), 1, table_file) ||
									!fread(&temp2, sizeof(temp2), 1, table_file)
								) {
									std::free(index);
									fclose(table_file);
									return false;
								}

								if (FILE *file = fopen(pmake(index[i]), "rb")) {

									if (size < limit) {

										fseek(file, 0, SEEK_END);
										size += ftell(file);

										table[index[i]] = new st_table_info;
										table[index[i]]->last_time = temp1;
										table[index[i]]->is_download = false;

										if (!(table[index[i]]->url = (const char*)malloc(temp2)) ||
											!fread((void*)(table[index[i]]->url), temp2, 1, table_file)
										) {

											std::free((void*)(table[index[i]]->url));
											delete table[index[i]];
											table[index[i]] = nullptr;

											std::free(index);
											fclose(table_file);
											return false;

										}

									}

									fclose(file);

								} else {
									fseek(table_file, temp2, SEEK_CUR);
								}

							}

							std::free(index);

						}

						fclose(table_file);

					}

					return true;

				}

				// Выгрузить хранилище
				static bool free() {

					if (FILE *table_file = fopen(SV_PATH_STORE_TABLE, "wb")) {

						uint32_t count = 0;
						for (auto i : table) if (i && !i->is_download) count++;
						if (!fwrite(&count, sizeof(count), 1, table_file)) {
							fclose(table_file);
							return false;
						}

						for (uint32_t i = 0; i < table.size(); i++)
							if (table[i] && !table[i]->is_download) if (!fwrite(&i, sizeof(i), 1, table_file)) {
								fclose(table_file);
								return false;
							}

						uint32_t url_length;
						for (auto i : table) {
							if (i && !i->is_download) {
								if (!fwrite(&i->last_time, sizeof(i->last_time), 1, table_file) ||
									!fwrite(&(url_length = strlen(i->url) + 1), sizeof(url_length), 1, table_file) ||
									!fwrite(i->url, url_length, 1, table_file)
								) {
									fclose(table_file);
									return false;
								}
							}
						}

						fclose(table_file);
						return true;

					}

					return false;

				}

				// Запросить звук (-1 если нет)
				static inline uint32_t request(
					const char *url
				) {
					for (uint32_t i = 0; i < table.size(); i++) if (table[i] && !strcmp(table[i]->url, url)) return i;
					return -1;
				}

				// Загрузить звук
				static bool download(
					const uint32_t index,
					const char *url
				) {

					HSTREAM meta;
					uint32_t file_size;

					if (!(meta = BASS_StreamCreateURL((const char*)(url), 0, BASS_SAMPLE_MONO | BASS_STREAM_DECODE, nullptr, nullptr))) return false;
					file_size = BASS_StreamGetFilePosition(meta, BASS_FILEPOS_END);
					BASS_StreamFree(meta);

					if (file_size) {
						
						if (store::size + file_size > store::limit)
							store::fclear((store::size + file_size) - store::limit);

						CreateThread(0, 0, store::download_proc, new st_download_info{ index, url }, 0, 0);
						return true;

					}

					return false;

				}

				// Установить предел хранилища
				static inline void set_limit(
					uint32_t new_limit
				) {
					if (new_limit < store::size) store::fclear(store::size - new_limit);
					store::limit = new_limit;
				}
				
			};

			// Текущая сессия
			class session {
			private:

				// Таблица перевода глобальных индексов в локальные
				static std::map<uint32_t, uint32_t> table;

			public:

				// Зарегистрировать звук
				static void sound(
					const uint32_t index,
					const char *url
				) {
					
					uint32_t temp;

					if (table.find(index) == table.end()) {
						if ((temp = store::request(url)) == -1) {
							temp = store::fget();
							store::download(temp, url);
						} table[index] = temp;
					}

				}

				// Получить локальный индекс (-1 если нет)
				static inline uint32_t get_index(
					const uint32_t index
				) {
					auto i = table.find(index);
					if (i != table.end()) return (*i).second;
					else return -1;
				}

			};

			// Класс звука
			class sound {
			protected:

				// Параметры потока
				const HSTREAM channel;		// Основной канал
				volatile bool status;		// Статус воспроизведения
				uint32_t begin;				// Начало воспроизведения
				const bool loop;			// Статус повторения
				float frequency;			// Оригинальная частота

				// Функция создания канала декодирования
				virtual HSTREAM create_channel() = 0;

				static DWORD WINAPI sound_proc(
					LPVOID _this
				) {

#define this ((sound*)(_this))

					uint32_t length;
					HSTREAM sound_stream;
					uint8_t buffer[SV_AUDIO_NET_BUFFER];

					while (this->status) {

						// Создать канал загрузки
						if (!(sound_stream = this->create_channel())) {
							LogError("sound", "could not create channel (code:%d)", BASS_ErrorGetCode());
							return 0;
						}

						BASS_ChannelPlay(this->channel, TRUE);

						// Загрузить файл в буфер основного канала
						while (this->status && (length = BASS_ChannelGetData(sound_stream, buffer, SV_AUDIO_NET_BUFFER)) != -1) {
							BASS_StreamPutData(this->channel, buffer, length);
							Sleep(10);
						}

						// Ждать конца воспроизведения
						while (this->status && BASS_ChannelIsActive(this->channel) == BASS_ACTIVE_PLAYING) Sleep(10);

						BASS_StreamFree(sound_stream);

						if (!this->loop) break;
						if (this->begin) this->begin = 0;

					}

					BASS_ChannelSetAttribute(this->channel, BASS_ATTRIB_FREQ, this->frequency);
					BASS_ChannelPlay(this->channel, TRUE);
					this->status = false;
					return 0;

#undef this

				}

				sound(
					HSTREAM		_channel,
					HSTREAM		_meta,
					uint32_t	_begin,
					bool		_loop
				) :
					channel(_channel),
					begin(_begin),
					loop(_loop),
					status(false)
				{

					float tmp_freq;

					// Получить параметры файла
					BASS_ChannelGetAttribute(_meta, BASS_ATTRIB_FREQ, &tmp_freq);
					BASS_ChannelGetAttribute(this->channel, BASS_ATTRIB_FREQ, &this->frequency);
					BASS_ChannelSetAttribute(this->channel, BASS_ATTRIB_FREQ, tmp_freq);
					BASS_StreamFree(_meta);

				}

			public:

				// Удалить звук
				inline void remove() {
					this->status = false;
				}

				// Получить статус звука
				inline bool get_status() {
					return this->status;
				}

				virtual ~sound() {};

			};

			// Внешний звук
			class esound : public sound {
			private:

				// URL внешнего файла
				const char *url;

				// Создание канала из URL
				HSTREAM create_channel() {
					return BASS_StreamCreateURL(
						this->url, this->begin,
						BASS_SAMPLE_MONO | BASS_STREAM_DECODE | BASS_STREAM_BLOCK,
						nullptr, nullptr
					);
				}

				esound(
					HSTREAM		_channel,
					HSTREAM		_meta,
					uint32_t	_offset,
					bool		_loop,
					const char	*_url
				) : sound(
					_channel,
					_meta,
					_offset,
					_loop
				), url(_url) {
					if (CreateThread(0, 0, sound::sound_proc, this, 0, 0)) this->status = true;
				}

			public:

				// Создать звук (Шаблон "фабрика")
				static esound* create(
					HSTREAM		_channel,
					uint32_t	_offset,
					bool		_loop,
					const char	*_url
				) {

					HSTREAM meta;

					if (meta = BASS_StreamCreateURL(_url, 0, BASS_SAMPLE_MONO | BASS_STREAM_DECODE, nullptr, nullptr)) {
						const char *url = (const char *)(malloc(strlen(_url) + 1));
						memcpy((void*)(url), _url, strlen(_url) + 1);
						return new esound(_channel, meta, _offset, _loop, url);
					}
					
					else return nullptr;

				}

				~esound() {
					std::free((void*)(this->url));
				}

			};

			// Внутренний звук
			class isound : public sound {
			private:

				// Индекс файла в хранилище
				const uint32_t index;

				// Создание канала из файла
				HSTREAM create_channel() {
					return BASS_StreamCreateFile(
						FALSE, store::pmake(index), this->begin, 0,
						BASS_SAMPLE_MONO | BASS_STREAM_DECODE
					);
				}

				isound(
					HSTREAM		_channel,
					HSTREAM		_meta,
					uint32_t	_offset,
					bool		_loop,
					uint32_t	_index
				) : sound(
					_channel,
					_meta,
					_offset,
					_loop
				), index(_index) {
					if (CreateThread(0, 0, sound::sound_proc, this, 0, 0)) this->status = true;
				}

			public:

				// Создать звук (Шаблон "фабрика")
				static isound* create(
					HSTREAM		_channel,
					uint32_t	_offset,
					bool		_loop,
					uint32_t	_index
				) {

					HSTREAM meta;
					uint32_t temp;

					if (((temp = session::get_index(_index)) != -1)) {
						if (store::is_valid(temp) && (meta = BASS_StreamCreateFile(FALSE, store::pmake(temp), 0, 0, BASS_SAMPLE_MONO | BASS_STREAM_DECODE)))
							return new isound(_channel, meta, _offset, _loop, temp);
					} else {
						RakNet::BitStream bs;
						bs.Write((uint8_t)(SV_NET_PACKET_ID));
						bs.Write((uint8_t)(e_packet_id::sound_request));
						bs.Write(_index);
						net::send(&bs);
					}
					
					return nullptr;

				}

			};

			uint32_t store::limit;
			uint32_t store::size;
			std::vector<store::st_table_info*> store::table;
			std::map<uint32_t, uint32_t> session::table;

		}

		// Звуковые эффекты
		namespace effects {

			struct st_chorus {
				float fWetDryMix = 50;
				float fDepth = 10;
				float fFeedback = 25;
				float fFrequency = 1.1f;
				DWORD lWaveform = 1;
				float fDelay = 16;
				DWORD lPhase = BASS_DX8_PHASE_90;
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
				BOOL lPanDelay = FALSE;
			};

			struct st_flanger {
				float fWetDryMix = 50;
				float fDepth = 100;
				float fFeedback = -50;
				float fFrequency = 0.25;
				DWORD lWaveform = 1;
				float fDelay = 2;
				DWORD lPhase = BASS_DX8_PHASE_ZERO;
			};

			struct st_gargle {
				DWORD dwRateHz = 20;
				DWORD dwWaveShape = 0;
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

			// Класс эффекта
			class effect {
			private:

				const uint8_t etype;
				const int epriority;
				void *eparams;

				std::map<HSTREAM, HFX> fx_effects;

			public:

				effect(
					const uint8_t effect_type,
					const void *parameters,
					const int priority
				) :
					etype(effect_type),
					epriority(priority)
				{

					// Копируем параметры
					switch (effect_type) {
					case e_effect_id::chorus: {
						eparams = malloc(sizeof(st_chorus));
						memcpy(eparams, parameters, sizeof(st_chorus));
					} break;
					case e_effect_id::compressor: {
						eparams = malloc(sizeof(st_compressor));
						memcpy(eparams, parameters, sizeof(st_compressor));
					} break;
					case e_effect_id::distortion: {
						eparams = malloc(sizeof(st_distortion));
						memcpy(eparams, parameters, sizeof(st_distortion));
					} break;
					case e_effect_id::echo: {
						eparams = malloc(sizeof(st_echo));
						memcpy(eparams, parameters, sizeof(st_echo));
					} break;
					case e_effect_id::flanger: {
						eparams = malloc(sizeof(st_flanger));
						memcpy(eparams, parameters, sizeof(st_flanger));
					} break;
					case e_effect_id::gargle: {
						eparams = malloc(sizeof(st_gargle));
						memcpy(eparams, parameters, sizeof(st_gargle));
					} break;
					case e_effect_id::i3dl2reverb: {
						eparams = malloc(sizeof(st_i3dl2reverb));
						memcpy(eparams, parameters, sizeof(st_i3dl2reverb));
					} break;
					case e_effect_id::parameq: {
						eparams = malloc(sizeof(st_parameq));
						memcpy(eparams, parameters, sizeof(st_parameq));
					} break;
					case e_effect_id::reverb: {
						eparams = malloc(sizeof(st_reverb));
						memcpy(eparams, parameters, sizeof(st_reverb));
					} break;
					}

				}

				// Подключить эффект к каналу
				inline bool attach(
					const HSTREAM channel
				) {
					return (
						(fx_effects.find(channel) == fx_effects.end()) &&
						BASS_FXSetParameters(
							fx_effects[channel] = BASS_ChannelSetFX(channel, etype, epriority),
							eparams
						)
					);
				}

				// Отключить эффект от канала
				inline void detach(
					const HSTREAM channel
				) {
					const auto iter = fx_effects.find(channel);
					if (iter != fx_effects.end()) {
						BASS_ChannelRemoveFX(channel, (*iter).second);
						fx_effects.erase(iter);
					}
				}

				~effect() {
					
					for (auto i = fx_effects.begin(); i != fx_effects.end(); i++)
						BASS_ChannelRemoveFX((*i).first, (*i).second);

					std::free(eparams);

				}

			};

		}

		// Звуковые потоки
		namespace streams {

			// Базовый поток
			class stream {
			protected:

				// Информация о скользящем параметре
				struct st_slide {
					float ratio;
					uint32_t end_time;
					float end_value;
				};

				static constexpr uint32_t c_max_count		= 5;	// Количество канальных ячеек
				static constexpr uint32_t c_timer_interval	= 100;	// Интервал таймера каналов
				static constexpr uint32_t c_duration		= 500;	// Время владения каналом

				const HSTREAM channel;
				const uint32_t hstream;

				// Параметры ячеек
				bool c_status[c_max_count] = { false };	// Статусы ячеек
				uint16_t c_id[c_max_count];				// ID каналов
				uint32_t c_ltime[c_max_count];			// Время последнего пакета
				HSTREAM c_handle[c_max_count];			// Дескрипторы каналов

				// Таблица параметров
				bool param_status[e_sparam_id::enum_end] = { false };
				float param_value[e_sparam_id::enum_end];

				// Таблица скользящих параметров
				bool slide_status[e_sparam_id::enum_end] = { false };
				st_slide slide_value[e_sparam_id::enum_end];

				// Таблица эффектов
				std::map<uint32_t, effects::effect*> effects;

				// Звук потока
				sounds::sound *csound = nullptr;

				// -----------------------------------

				// Создать канал
				virtual HSTREAM c_create() {
					return NULL;
				}

				// Получить индекс канала
				inline uint32_t c_index(
					const uint16_t id
				) {
					for (uint32_t i = 0; i < c_max_count; i++) if (c_status[i] && c_id[i] == id) return i;
					return -1;
				}

				// Получить свободный индекс
				inline uint32_t c_ifree() {
					for (uint32_t i = 0; i < c_max_count; i++) if (!c_status[i]) return i;
					return -1;
				}

			public:

				// Тик потока
				inline void tick(
					const uint32_t current_time
				) {
					
					// Удаление лишних каналов
					for (uint32_t i = 0; i < c_max_count; i++)
						if (c_status[i] && (current_time - c_ltime[i] > c_duration)) {
							if (settings::get_settings()->debug_status) LogDebug("channel_delete : %u", c_handle[i]);
							BASS_StreamFree(c_handle[i]);
							c_status[i] = false;
						}

					// Проверка звука
					if (this->csound && !this->csound->get_status()) {
						delete this->csound;
						this->csound = nullptr;
					}

				}

				// Перезагрузить каналы
				inline void reset() {
					BASS_ChannelPlay(this->channel, TRUE);
					for (uint32_t i = 0; i < c_max_count; i++)
						if (c_status[i]) BASS_ChannelPlay(c_handle[i], TRUE);
				}

				// -----------------------------------

				// Создать поток
				stream(
					const uint32_t _hstream
				) :
					hstream(_hstream),
					channel(c_create())
				{
					BASS_ChannelSetAttribute(this->channel, BASS_ATTRIB_NOBUFFER, TRUE);
				}

				// Создать эффект
				inline void effect_create(
					uint32_t effect,
					uint8_t effect_type,
					void *parameters,
					int priority
				) {

					const auto e_ptr = effects[effect] = new effects::effect(effect_type, parameters, priority);

					e_ptr->attach(this->channel);

					for (uint32_t i = 0; i < c_max_count; i++)
						if (c_status[i]) e_ptr->attach(c_handle[i]);

				}

				// Удалить эффект
				inline void effect_delete(
					uint32_t effect
				) {

					auto iter = effects.find(effect);
					if (iter != effects.end()) {
						delete (*iter).second;
						effects.erase(iter);
					}

				}

				// Воспроизвести музыку из файла
				inline void play_sound(
					const uint32_t index,
					uint32_t offset,
					bool loop
				) {
					if (!this->csound) this->csound = sounds::isound::create(this->channel, offset, loop, index);
				}

				// Воспроизвести музыку из url
				inline void play_sound(
					const char *url,
					uint32_t offset,
					bool loop
				) {
					if (!this->csound) this->csound = sounds::esound::create(this->channel, offset, loop, url);
				}

				// Остановить воспроизведение музыки
				inline void stop_sound() {
					if (this->csound) this->csound->remove();
				}

				// Установить параметр потока
				inline void set_parameter(
					uint8_t param_id,
					float value
				) {

					param_value[param_id] = value;
					param_status[param_id] = true;

					BASS_ChannelSetAttribute(this->channel, param_id, value);

					for (uint32_t i = 0; i < c_max_count; i++)
						if (c_status[i]) BASS_ChannelSetAttribute(c_handle[i], param_id, value);

				}

				// Задать скольжение параметра
				inline void slide_parameter(
					uint8_t param_id,
					float start_value,
					float end_value,
					uint32_t time
				) {

					slide_value[param_id].ratio = (end_value - start_value) / time;
					slide_value[param_id].end_time = clock() + time;
					slide_value[param_id].end_value = end_value;
					slide_status[param_id] = true;

					BASS_ChannelSetAttribute(this->channel, param_id, start_value);
					BASS_ChannelSlideAttribute(this->channel, param_id, end_value, time);

					for (uint32_t i = 0; i < c_max_count; i++)
						if (c_status[i]) {
							BASS_ChannelSetAttribute(c_handle[i], param_id, start_value);
							BASS_ChannelSlideAttribute(c_handle[i], param_id, end_value, time);
						}

				}

				// Вставить аудиоданные в поток
				inline void push(
					const uint16_t id,
					const void *data,
					const uint32_t size
				) {

					uint32_t cid;

					if ((cid = c_index(id)) == -1) {

						if ((cid = c_ifree()) == -1) return;

						c_id[cid] = id;
						BASS_ChannelSetAttribute(
							c_handle[cid] = c_create(),
							BASS_ATTRIB_NOBUFFER, TRUE
						);
						
						int delta_time;
						uint32_t current_time = clock();

						// Проверка обычных и скользящих параметров
						for (uint8_t i = 0; i < e_sparam_id::enum_end; i++) {
							
							// Применить скользящие параметры
							if (slide_status[i]) {
								if ((delta_time = slide_value[i].end_time - current_time) > 0) {
									BASS_ChannelSetAttribute(c_handle[cid], i, slide_value[i].end_value - (slide_value[i].ratio * delta_time));
									BASS_ChannelSlideAttribute(c_handle[cid], i, slide_value[i].end_value, delta_time);
									continue;
								} else slide_status[i] = false;
							}
							
							// Применить обычные параметры, если нет скользящих
							if (param_status[i]) BASS_ChannelSetAttribute(c_handle[cid], i, param_value[i]);

						}

						// Применение эффектов
						for (auto i = effects.begin(); i != effects.end(); i++)
							(*i).second->attach(c_handle[cid]);

						BASS_ChannelPlay(c_handle[cid], FALSE);
						c_status[cid] = true;

					}

					BASS_StreamPutData(c_handle[cid], data, size);
					c_ltime[cid] = clock();

				}

				// Удалить поток
				~stream() {

					this->stop_sound();		/// [WARNING!!!] : Утечка памяти sound никогда не будет удалён (delete) так как sound->tick() больше не вызывается
					for (auto i = this->effects.begin(); i != this->effects.end(); i++) delete (*i).second;
					for (uint32_t i = 0; i < c_max_count; i++) if (c_status[i]) BASS_StreamFree(c_handle[i]);
					BASS_StreamFree(this->channel);

				}

			};

			// Глобальный поток
			class gstream : public stream {
			private:

				uint16_t frequency;

			protected:

				HSTREAM c_create() {
					return BASS_StreamCreate(this->frequency, 1, 0, STREAMPROC_PUSH, nullptr);
				}

			public:

				gstream(
					uint16_t frequency,
					uint32_t stream_handle
				) : stream(
					stream_handle
				) {
					this->frequency = frequency;
				}

			};

			// Локальный поток
			class lstream : public stream {
			private:

				uint16_t frequency;
				const void *vec_position;
				const void *vec_orientation;
				const void *vec_velocity;

			protected:

				HSTREAM c_create() {

					HSTREAM handle = BASS_StreamCreate(this->frequency, 1, BASS_SAMPLE_3D, STREAMPROC_PUSH, nullptr);
					BASS_ChannelSet3DAttributes(handle, BASS_3DMODE_NORMAL, -1, -1, -1, -1, -1);
					BASS_ChannelSet3DPosition(
                        handle,
						(BASS_3DVECTOR*)(this->vec_position),
						(BASS_3DVECTOR*)(this->vec_orientation),
						(BASS_3DVECTOR*)(this->vec_velocity)
					);

					BASS_Apply3D();
					return handle;

				}

			public:

				lstream(
					uint16_t frequency,
					uint32_t stream_handle,
					const void *vec_position,
					const void *vec_orientation,
					const void *vec_velocity
				) : stream(
					stream_handle
				) {

					this->frequency = frequency;

					this->vec_position		= malloc(sizeof(float[3]));
					this->vec_orientation	= malloc(sizeof(float[3]));
					this->vec_velocity		= malloc(sizeof(float[3]));

					memcpy((void*)(this->vec_position), vec_position, sizeof(float[3]));
					memcpy((void*)(this->vec_orientation), vec_orientation, sizeof(float[3]));
					memcpy((void*)(this->vec_velocity), vec_velocity, sizeof(float[3]));

				}

				inline void update(
					const void *vec_position,
					const void *vec_orientation,
					const void *vec_velocity
				) {

					memcpy((void*)(this->vec_position), vec_position, sizeof(float[3]));
					memcpy((void*)(this->vec_orientation), vec_orientation, sizeof(float[3]));
					memcpy((void*)(this->vec_velocity), vec_velocity, sizeof(float[3]));

					BASS_ChannelSet3DPosition(
						this->channel,
						(BASS_3DVECTOR*)(vec_position),
						(BASS_3DVECTOR*)(vec_orientation),
						(BASS_3DVECTOR*)(vec_velocity)
					);

					for (uint32_t i = 0; i < c_max_count; i++)
						if (c_status[i]) BASS_ChannelSet3DPosition(
							c_handle[i],
							(BASS_3DVECTOR*)(vec_position),
							(BASS_3DVECTOR*)(vec_orientation),
							(BASS_3DVECTOR*)(vec_velocity)
						);

				}

				~lstream() {
					std::free((void*)(this->vec_position));
					std::free((void*)(this->vec_orientation));
					std::free((void*)(this->vec_velocity));
				}

			};

		}

		// Класс слушателя
		class listener {
		public:

			// Обновить координаты
			static void update() {

				BASS_Set3DPosition(
					(BASS_3DVECTOR*)&TheCamera.GetPosition(),
					&BASS_3DVECTOR{0, 0, 0},
					(BASS_3DVECTOR*)&TheCamera.GetMatrix()->at,
					(BASS_3DVECTOR*)&TheCamera.GetMatrix()->up
				);

			}

		};

		// ----------------------------

		static bool status = false;

		static uint16_t frequency;
		static opus_int16 *dec_buffer_ptr;
		static uint32_t dec_buffer_size;
		static memory::hooks::jump_hook *hook_change_volume;

		static std::map<uint32_t, streams::stream*> table_stream;

		// Перехватчики thiscall
		class thiscall_hooks {
		public:

			// Перехватчик изменения громкости звука
			static void __thiscall hook_func_change_volume(CMenuManager *_this) {

				hook_change_volume->disable();

				BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, (float)(FrontEndMenuManager.m_nSfxVolume) * (10000.f / 64.f));

				((void(__thiscall*)(CMenuManager*))(hook_change_volume->get_original_addr()))(_this);
				hook_change_volume->enable();

			}

		};

		// ----------------------------

		// Инициализировать подмодуль
		static bool init(
			uint16_t frequency,
			uint32_t frame_size,
			uint32_t store_limit,
			float factor_distance,
			float factor_rolloff,
			float factor_doppler
		) {

			BASS_Free();
			playback::frequency = frequency;

			if (BASS_Init(-1, frequency, BASS_DEVICE_MONO | BASS_DEVICE_3D | BASS_DEVICE_FREQ, 0, NULL)) {

				if (!(playback::dec_buffer_ptr = (opus_int16*)malloc(
					playback::dec_buffer_size = frame_size * sizeof(opus_int16)
				))) {
					LogError("playback", "could not allocate memory for dec_buffer");
					return false;
				}

				if (!sounds::store::init(store_limit)) {
					LogError("playback", "could not init store");
					return false;
				}

				BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, (float)(FrontEndMenuManager.m_nSfxVolume) * (10000.f / 64.f));
				BASS_Set3DFactors(factor_distance, factor_rolloff, factor_doppler);
				BASS_SetConfig(BASS_CONFIG_3DALGORITHM, BASS_3DALG_LIGHT);
				BASS_Set3DPosition(
					(BASS_3DVECTOR*)&TheCamera.GetPosition(),
					&BASS_3DVECTOR{ 0, 0, 0 },
					(BASS_3DVECTOR*)&TheCamera.GetMatrix()->at,
					(BASS_3DVECTOR*)&TheCamera.GetMatrix()->up
				);

				if (!(hook_change_volume = new memory::hooks::jump_hook(0x57C660u, thiscall_hooks::hook_func_change_volume)))
					return false;

				return playback::status = true;

			}

			return false;

		}

		static void create_stream_local(
			uint32_t stream_handle,
			const void *vec_position,
			const void *vec_orientation,
			const void *vec_velocity
		) {
			playback::table_stream[stream_handle] = new streams::lstream(playback::frequency, stream_handle, vec_position, vec_orientation, vec_velocity);
		}

		static void create_stream_global(
			uint32_t stream_handle
		) {
			playback::table_stream[stream_handle] = new streams::gstream(playback::frequency, stream_handle);
		}

		static inline streams::stream* get_stream(
			uint32_t stream_handle
		) {
			return playback::table_stream[stream_handle];
		}

		static inline streams::lstream* get_local_stream(
			uint32_t stream_handle
		) {
			return reinterpret_cast<streams::lstream*>(playback::table_stream[stream_handle]);
		}

		static inline streams::gstream* get_global_stream(
			uint32_t stream_handle
		) {
			return reinterpret_cast<streams::gstream*>(playback::table_stream[stream_handle]);
		}

		static inline void delete_stream(
			uint32_t stream_handle
		) {

			auto iter = table_stream.find(stream_handle);
			if (iter != table_stream.end()) {
				delete (*iter).second;
				table_stream.erase(iter);
			}

		}

		static inline void push_enc(
			uint32_t stream_handle,
			uint16_t id,
			void *data,
			uint32_t size
		) {
			const auto iter = playback::table_stream.find(stream_handle);
			if (iter != playback::table_stream.end()) {
				int len = opus::decode(data, size, playback::dec_buffer_ptr);
				if ((len *= sizeof(opus_int16)) > 0) (*iter).second->push(id, playback::dec_buffer_ptr, len);
			}
		}

		namespace attach {

			struct st_attach_table_info {
				uint32_t attach_id;
				uint32_t stream_id;
			};

			static std::forward_list<st_attach_table_info> attach_vehicle_streams;
			static std::forward_list<st_attach_table_info> attach_player_streams;
			static std::forward_list<st_attach_table_info> attach_object_streams;

			static void update() {

				CVector coords[3];

				for (auto i = attach_vehicle_streams.begin(); i != attach_vehicle_streams.end(); i++) {
					if (!samp::pSamp->pPools->pVehicle->pGTA_Vehicle[(*i).attach_id]) continue;
					coords[0] = samp::pSamp->pPools->pVehicle->pGTA_Vehicle[(*i).attach_id]->GetPosition();
					samp::pSamp->pPools->pVehicle->pGTA_Vehicle[(*i).attach_id]->GetOrientation(coords[1].x, coords[1].y, coords[1].z);
					coords[2] = samp::pSamp->pPools->pVehicle->pGTA_Vehicle[(*i).attach_id]->GetSpeed(coords[1]);
					audio::playback::get_local_stream((*i).stream_id)->update(&coords[0], &coords[1], &coords[2]);
				}

				for (auto i = attach_player_streams.begin(); i != attach_player_streams.end(); i++) {
					if (!samp::pSamp->pPools->pPlayer->pRemotePlayer[(*i).attach_id]) continue;
					coords[0] = samp::pSamp->pPools->pPlayer->pRemotePlayer[(*i).attach_id]->pPlayerData->pSAMP_Actor->pGTA_Ped->GetPosition();
					samp::pSamp->pPools->pPlayer->pRemotePlayer[(*i).attach_id]->pPlayerData->pSAMP_Actor->pGTA_Ped->GetOrientation(coords[1].x, coords[1].y, coords[1].z);
					coords[2] = samp::pSamp->pPools->pPlayer->pRemotePlayer[(*i).attach_id]->pPlayerData->pSAMP_Actor->pGTA_Ped->GetSpeed(coords[1]);
					audio::playback::get_local_stream((*i).stream_id)->update(&coords[0], &coords[1], &coords[2]);
				}

				for (auto i = attach_object_streams.begin(); i != attach_object_streams.end(); i++) {
					if (!samp::pSamp->pPools->pObject->object[(*i).attach_id]) continue;
					coords[0] = samp::pSamp->pPools->pObject->object[(*i).attach_id]->pGTAEntity->GetPosition();
					samp::pSamp->pPools->pObject->object[(*i).attach_id]->pGTAEntity->GetOrientation(coords[1].x, coords[1].y, coords[1].z);
					coords[2] = samp::pSamp->pPools->pObject->object[(*i).attach_id]->pGTAEntity->GetSpeed(coords[1]);
					audio::playback::get_local_stream((*i).stream_id)->update(&coords[0], &coords[1], &coords[2]);
				}

			}

			static void remove(
				uint32_t stream_id
			) {

				attach_vehicle_streams.remove_if([=](st_attach_table_info &e) {
					return e.stream_id == stream_id;
				});

				attach_player_streams.remove_if([=](st_attach_table_info &e) {
					return e.stream_id == stream_id;
				});

				attach_object_streams.remove_if([=](st_attach_table_info &e) {
					return e.stream_id == stream_id;
				});

			}

		};

		static void free() {

			playback::status = false;

			for (auto i = playback::table_stream.begin(); i != playback::table_stream.end(); i++)
				delete (*i).second;

			std::free(playback::dec_buffer_ptr);
			playback::table_stream.clear();
			delete hook_change_volume;
			sounds::store::free();
			BASS_Free();

		}

	}

	// Подмодуль записи
	class record {
	private:

		// Статус загрузки
		static bool status;

		// Канал модуля
		static HRECORD stream;

		// Буфер записи
		static memory::ringb *rbuffer;

		// Перехватчик звукового потока
		static BOOL CALLBACK record_hook(
			HRECORD handle,
			const void *buffer,
			DWORD length,
			void *user
		) {
			rbuffer->push((uint8_t*)buffer, length);
			return BASS_ChannelIsActive(handle);
		}

	public:

		// Инициализировать модуль записи
		static bool init(
			void(*record_handler)(uint8_t*),	// Обработчик звукового потока
			uint32_t frequency,					// Частота звука (в герцах)
			uint16_t frame_dur,					// Длительность кадра (в миллисекундах)
			uint32_t rbuf_size					// Размер буфера записи (в кадрах)
		) {

			if (record::status) return false;
			if ((!record_handler) ||
				(frequency != 8000u && frequency != 12000u && frequency != 16000u && frequency != 24000u && frequency != 48000u) ||
				(frame_dur != 40u && frame_dur != 60u && frame_dur != 80u && frame_dur != 100u && frame_dur != 120u)
			) {
				LogError("record", "invalid arguments in function init");
				return false;
			}

			if (!record::has_microphone()) {
				LogError("record", "could not find microphone");
				return false;
			}

			if (!(record::rbuffer = new memory::ringb(record_handler, frequency * frame_dur / 1000 * sizeof(opus_int16), rbuf_size))) {
				LogError("record", "could not allocate memory for record buffer");
				return false;
			}

			// Инициализация устройства записи
			if (BASS_RecordInit(-1)) {
				if (!(record::stream = BASS_RecordStart(frequency, 1, MAKELONG(BASS_RECORD_PAUSE, frame_dur), record::record_hook, nullptr))) {
					LogError("record", "failed BASS_RecordStart (code:%d)", BASS_ErrorGetCode());
					BASS_RecordFree();
					delete record::rbuffer;
					return false;
				} BASS_ChannelSetAttribute(record::stream, BASS_ATTRIB_VOL, 1);
				return record::status = true;
			} else {
				LogError("record", "failed BASS_RecordInit (code:%d)", BASS_ErrorGetCode());
				delete record::rbuffer;
				return false;
			}

		}

		// Функции управления каналом записи
		// ---------------------------------------------------------

		// Начать запись
		static inline void record_start() {
			if (record::status) BASS_ChannelPlay(record::stream, TRUE);
		}

		// Остановить запись
		static inline void record_stop() {
			if (record::status) BASS_ChannelPause(record::stream);
		}

		// Получить статус канала записи
		static inline bool record_is_active() {
			return record::status && BASS_ChannelIsActive(record::stream) == BASS_ACTIVE_PLAYING;
		}

		// Функции управления параметрами
		// ---------------------------------------------------------

		// Получить уровень сигнала
		static inline uint16_t get_level() {

			if (record::status) return BASS_ChannelGetLevel(record::stream) & 0xFFFFu;
			else return NULL;

		}

		// Установить чувствительность микрофона
		static inline void set_volume(
			float volume						// Усиление чувствительности микрофона
		) {
			if (record::status) BASS_RecordSetInput(-1, BASS_INPUT_ON, volume);
		}

		// ---------------------------------------------------------

		// Проверить наличие микрофона
		static bool has_microphone() {

			bool is_already = false;
			if (BASS_RecordInit(-1) || (is_already = (BASS_ErrorGetCode() == BASS_ERROR_ALREADY))) {
				if (!is_already) BASS_RecordFree();
				return true;
			}
			
			return false;

		}

		// Выгрузить модуль записи
		static void free() {

			if (record::status) {
				if (record::record_is_active()) record::record_stop();
				BASS_StreamFree(record::stream);
				delete record::rbuffer;
				BASS_RecordFree();
			}

		}

	};

	// Инициализировать аудиосистему
	static bool init(
		void(*record_handler)(uint8_t*),	// Функция обработки исходящего аудиопотока
		uint16_t bitrate,					// Битрейт (в бит/с)
		uint16_t frequency,					// Частота звука
		uint16_t frame_dur,					// Длительность одного звукового кадра (в миллисекундах)
		uint32_t store_limit,				// Размер хранилища звуковых файлов
		float factor_distance,				// Фактор дистанции звука
		float factor_rolloff,				// Фактор затухания звука
		float factor_doppler				// Фактор эффекта Допплера
	) {

		uint32_t frame_size = frequency * frame_dur / 1000;
		record::init(record_handler, frequency, frame_dur, 32u);

		// Модуль record не обязателен для инициализации
		return (playback::init(frequency, frame_size, store_limit, factor_distance, factor_rolloff, factor_doppler) && opus::init(frequency, frame_size, bitrate));

	}

	// Итерация основного цикла
	static void tick() {

		if (playback::status) {
			
			// Проверить каналы потоков
			uint32_t current_time = clock();
			for (auto i = playback::table_stream.begin(); i != playback::table_stream.end(); i++)
				(*i).second->tick(current_time);

			playback::listener::update();
			playback::attach::update();
			BASS_Apply3D();

		}

	}

	// Выгрузить аудиосистему
	static inline void free() {
		playback::free();
		record::free();
		opus::free();
	}

	bool opus::status = false;
	OpusEncoder* opus::encoder = nullptr;
	OpusDecoder* opus::decoder = nullptr;
	uint32_t opus::frequency;
	uint32_t opus::frame_size;
	uint32_t opus::bitrate;

	HRECORD record::stream;
	bool record::status = false;
	memory::ringb* record::rbuffer = nullptr;

}
