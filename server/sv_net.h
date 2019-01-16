#ifndef _NET_H_
#define _NET_H_

#include "sv_header.h"

#define RPC_CONNECT				25
#define RPC_WORLDPLAYERADD		32
#define RPC_WORLDPLAYERREMOVE	163
#define RPC_CREATEOBJECT		44
#define RPC_DESTROYOBJECT		47
#define RPC_WORLDVEHICLEADD		164
#define RPC_WORLDVEHICLEREMOVE	165

constexpr char const
#ifdef _WIN32
*pattern = "\x64\xA1\x00\x00" \
"\x00\x00\x50\x64\x89\x25\x00\x00\x00\x00\x51" \
"\x68\x18\x0E\x00\x00\xE8\xFF\xFF\xFF\xFF\x83" \
"\xC4\x04\x89\x04\x24\x85\xC0\xC7\x44\x24\xFF" \
"\x00\x00\x00\x00\x74\x16",
*mask = "xxxxxxxxxxxxxxxx????x????xxxxxxxxxxx?xxxxxx";
#else
*pattern = "\x04\x24\xFF\xFF" \
"\xFF\xFF\x89\x75\xFF\x89\x5D\xFF\xE8\xFF\xFF" \
"\xFF\xFF\x89\x04\x24\x89\xC6\xE8\xFF\xFF\xFF" \
"\xFF\x89\xF0\x8B\x5D\xFF\x8B\x75\xFF\x89\xEC" \
"\x5D\xC3",
*mask = "xx????xx?xx?x????xxxxxx????xxxx?xx?xxxx";
#endif

namespace net {

	class handler_interface {
	public:

		virtual bool packet_handler(uint8_t packet_id, Packet *packet) = 0;

		virtual void rpc_handler_connect(uint16_t sender_id, sv_packet::connect* header) = 0;
		virtual void rpc_handler_vehicle_add(uint16_t sender_id, uint16_t vehicle_id) = 0;
		virtual void rpc_handler_vehicle_remove(uint16_t sender_id, uint16_t vehicle_id) = 0;
		virtual void rpc_handler_player_add(uint16_t sender_id, uint16_t player_id) = 0;
		virtual void rpc_handler_player_remove(uint16_t sender_id, uint16_t player_id) = 0;
		virtual void rpc_handler_object_add(uint16_t sender_id, uint16_t object_id) = 0;
		virtual void rpc_handler_object_remove(uint16_t sender_id, uint16_t object_id) = 0;

	};

	static handler_interface *handler = nullptr;
	static void (*rpc_original_connect)(RPCParameters*);

	// Перехватчик подключения игрока
	static void STDCALL hook_rpc_connect(
		RPCParameters *parameters
	) {

		sv_packet::connect *st_connect = (sv_packet::connect*)(parameters->input + ((parameters->numberOfBitsOfData >> 3) - sizeof(sv_packet::connect)));
		handler->rpc_handler_connect(raknet::getindexfromplayerid(parameters->sender), st_connect);
		if (*(uint32_t*)(st_connect) == SV_NET_CONNECT_SIGNATURE) parameters->numberOfBitsOfData -= sizeof(sv_packet::connect) << 3; // Уменьшаем длину пакета, удаляя тем самым заголовок
		rpc_original_connect(parameters);

	}

	class thiscall_hooks {
	public:

		// Перехватчик регистрации rpc
		static void THISCALL hook_register_as_rpc(
			void *_this,
			uint8_t *rpc_id,
			void(*rpc_handler)(RPCParameters*)
		) {

			if (*rpc_id == RPC_CONNECT) {
				rpc_original_connect = rpc_handler;
				rpc_handler = (void(*)(RPCParameters*))(&hook_rpc_connect);
			}

			raknet::registerasrpc((int*)(rpc_id), rpc_handler);

		}

		// Перехватчик исходящих rpc
		static bool THISCALL hook_rpc(
			void *_this,
			char* rpc_id,
			BitStream *parameters,
			PacketPriority priority,
			PacketReliability reliability,
			char orderingChannel,
			PlayerID playerId,
			bool broadcast,
			bool shiftTimestamp
		) {

			bool return_status;

			// Перехват rpc
			switch (*(uint8_t*)(rpc_id)) {
			case RPC_WORLDVEHICLEADD: {

				return_status = raknet::rpc(
					(BYTE*)(rpc_id),
					parameters,
					priority,
					reliability,
					orderingChannel,
					playerId,
					broadcast,
					shiftTimestamp
				);

				handler->rpc_handler_vehicle_add(
					raknet::getindexfromplayerid(playerId),
					*(uint16_t*)(parameters->GetData())
				);

				return return_status;

			} break;
			case RPC_WORLDVEHICLEREMOVE: {

				handler->rpc_handler_vehicle_remove(
					raknet::getindexfromplayerid(playerId),
					*(uint16_t*)(parameters->GetData())
				);

			} break;
			case RPC_WORLDPLAYERADD: {

				return_status = raknet::rpc(
					(BYTE*)(rpc_id),
					parameters,
					priority,
					reliability,
					orderingChannel,
					playerId,
					broadcast,
					shiftTimestamp
				);

				handler->rpc_handler_player_add(
					raknet::getindexfromplayerid(playerId),
					*(uint16_t*)(parameters->GetData())
				);

				return return_status;

			} break;
			case RPC_WORLDPLAYERREMOVE: {

				handler->rpc_handler_player_remove(
					raknet::getindexfromplayerid(playerId),
					*(uint16_t*)(parameters->GetData())
				);

			} break;
			case RPC_CREATEOBJECT: {

				return_status = raknet::rpc(
					(BYTE*)(rpc_id),
					parameters,
					priority,
					reliability,
					orderingChannel,
					playerId,
					broadcast,
					shiftTimestamp
				);

				handler->rpc_handler_object_add(
					raknet::getindexfromplayerid(playerId),
					*(uint16_t*)(parameters->GetData())
				);

				return return_status;

			} break;
			case RPC_DESTROYOBJECT: {

				handler->rpc_handler_object_remove(
					raknet::getindexfromplayerid(playerId),
					*(uint16_t*)(parameters->GetData())
				);

			} break;
			}

			// Отправка rpc
			return raknet::rpc(
				(BYTE*)(rpc_id),
				parameters,
				priority,
				reliability,
				orderingChannel,
				playerId,
				broadcast,
				shiftTimestamp
			);

		}

		// Перехватчик входящих пакетов
		static Packet* THISCALL hook_receive(void *_this) {

			Packet *packet = nullptr;
			while (packet = raknet::receive()) {
				if (*packet->data != SV_NET_PACKET_ID) break;
				if (handler->packet_handler(*(packet->data + 1), packet))
					raknet::deallocate_packet(packet);
			}
			
			return packet;

		}

	};

	// ==========================================================

	// Перехватчик создания RakServer
	static memory::address_t addr_pawn_raknet = memory::null;
	static memory::hooks::jump_hook *hook_rakserver;
	static void* STDCALL func_hook_getrakserver() {
		
		void *rakserver;

		hook_rakserver->disable();
		if (addr_pawn_raknet) rakserver = ((void*(STDCALL*)())(addr_pawn_raknet))();
		else rakserver = ((void*(*)())(hook_rakserver->get_original_addr()))();
		hook_rakserver->enable();

		if (rakserver) {

			raknet::init(rakserver);

			auto vtable = *((void***)(rakserver));

			memory::unprotect_scope rpc(&vtable[RAKNET_RPC_OFFSET], sizeof(void*));
			memory::unprotect_scope scope_receive(&vtable[RAKNET_RECEIVE_OFFSET], sizeof(void*));
			memory::unprotect_scope scope_register(&vtable[RAKNET_REGISTER_RPC_OFFSET], sizeof(void*));

			vtable[RAKNET_RPC_OFFSET] = reinterpret_cast<void*>(thiscall_hooks::hook_rpc);
			vtable[RAKNET_RECEIVE_OFFSET] = reinterpret_cast<void*>(thiscall_hooks::hook_receive);
			vtable[RAKNET_REGISTER_RPC_OFFSET] = reinterpret_cast<void*>(thiscall_hooks::hook_register_as_rpc);

		}

		return rakserver;

	}

	// Инициализировать модуль
	static bool init(
		void *addr_server,
		handler_interface *handler
	) {
		
		if (!(net::handler = handler)) return false;

		void *addr; memory::dword_t size;
		if (!memory::getmoduleinfo(addr_server, addr, size)) return false;
		memory::scanner net_scanner(addr, size);

		if (uint8_t *func_ptr = (uint8_t*)(net_scanner.find(pattern, mask))) {
			func_ptr -= 7; // Вычитаем смещение сигнатуры
			if (*func_ptr == 0xE9) addr_pawn_raknet = (uint32_t)(func_ptr) + *(uint32_t*)(func_ptr + 1) + 5;
			if (hook_rakserver = new memory::hooks::jump_hook(func_ptr, func_hook_getrakserver)) return true;
		}

		return false;

	}

	// Выгрузить модуль
	static void free() {

		if (hook_rakserver) delete hook_rakserver;
		if (net::handler) {
			delete net::handler;
			net::handler = nullptr;
		}

	}

	// ==========================================================

	static inline bool send(
		uint16_t player_id,
		BitStream *bs
	) {
		return raknet::send(
			bs, PacketPriority::HIGH_PRIORITY,
			PacketReliability::RELIABLE_ORDERED,
			'\0', raknet::getplayeridfromindex(player_id),
			player_id == -1
		);
	}

	static inline bool send(
		uint16_t player_id,
		uint8_t packet_id
	) {
		bitstream bs(packet_id);
		return raknet::send(
			&bs, PacketPriority::HIGH_PRIORITY,
			PacketReliability::RELIABLE_ORDERED,
			'\0', raknet::getplayeridfromindex(player_id),
			player_id == -1
		);
	}

	template<class T = uint32_t>
	static inline bool send(
		uint16_t player_id,
		uint8_t packet_id,
		T *params
	) {
		bitstream bs(packet_id);
		bs.Write((const char *)(params), sizeof(T));
		return raknet::send(
			&bs, PacketPriority::HIGH_PRIORITY,
			PacketReliability::RELIABLE_ORDERED,
			'\0', raknet::getplayeridfromindex(player_id),
			player_id == -1
		);
	}

};

#endif
