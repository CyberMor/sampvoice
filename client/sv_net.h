#pragma once

#include <Windows.h>
#include <Psapi.h>

#include <raknet/BitStream.h>
#include <raknet/RakClient.h>

#include "logger.h"
#include "memory.hpp"

namespace net {

	// Статус модуля
	static bool status = false;

	// Поток инициализации
	static HANDLE thread_init;

	// Типы обработчиков
	using packet_handler_t		= bool (CALLBACK *)(BYTE id, Packet *packet);
	using rpc_handler_t			= bool (CALLBACK *)(BYTE id, RakNet::BitStream *bs);
	using disconnect_handler_t	= void (CALLBACK *)(void);

	// Обработчики
	static rpc_handler_t		rpc_handler			= nullptr;
	static packet_handler_t		packet_handler		= nullptr;
	static disconnect_handler_t	disconnect_handler	= nullptr;

	// Параметры интерфейса-перехватчика
	static RakClientInterface *pRakClientInterface;
	static RakClientInterface **ppRakClientInterface;

	// Перехватчики
	static memory::hooks::jump_hook *hook_init_rakclient;
	static memory::hooks::call_hook *hook_samp_destruct1;
	static memory::hooks::call_hook *hook_samp_destruct2;

	// Интерфейс-перехватчик
	class RakClientInterfaceHook : public RakClientInterface {
	private:

		RakClientInterface * orig_interface = nullptr;

		packet_handler_t		packet_handler = nullptr;
		rpc_handler_t			rpc_handler = nullptr;
		disconnect_handler_t	disconnect_handler = nullptr;

	public:

		RakClientInterfaceHook(
			RakClientInterface *_orig_interface,
			packet_handler_t _packet_handler = nullptr,
			rpc_handler_t _rpc_handler = nullptr,
			disconnect_handler_t _disconnect_handler = nullptr
		) : orig_interface(_orig_interface),
			packet_handler(_packet_handler),
			rpc_handler(_rpc_handler),
			disconnect_handler(_disconnect_handler)
		{}

		bool RPC(int* uniqueID, RakNet::BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) {
			if (this->rpc_handler) if (!this->rpc_handler(*uniqueID, parameters)) return true;
			return this->orig_interface->RPC(uniqueID, parameters, priority, reliability, orderingChannel, shiftTimestamp);
		}

		bool Send(RakNet::BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel) {
			return this->orig_interface->Send(bitStream, priority, reliability, orderingChannel);
		}

		Packet *Receive(void) {
			Packet *packet{};
			while (packet = this->orig_interface->Receive()) {
				if (!this->packet_handler || this->packet_handler(*packet->data, packet)) break;
				this->orig_interface->DeallocatePacket(packet);
			} return packet;
		}

		bool Connect(const char* host, unsigned short serverPort, unsigned short clientPort, unsigned int depreciated, int threadSleepTimer) {
			return this->orig_interface->Connect(host, serverPort, clientPort, depreciated, threadSleepTimer);
		}

		void Disconnect(unsigned int blockDuration, unsigned char orderingChannel) {
			this->disconnect_handler();
			this->orig_interface->Disconnect(blockDuration, orderingChannel);
		}

		void InitializeSecurity(const char *privKeyP, const char *privKeyQ) {
			this->orig_interface->InitializeSecurity(privKeyP, privKeyQ);
		}

		void SetPassword(const char *_password) {
			this->orig_interface->SetPassword(_password);
		}

		bool HasPassword(void) {
			return this->orig_interface->HasPassword();
		}

		bool Send(const char *data, const int length, PacketPriority priority, PacketReliability reliability, char orderingChannel) {
			return this->orig_interface->Send(data, length, priority, reliability, orderingChannel);
		}

		void DeallocatePacket(Packet *packet) {
			this->orig_interface->DeallocatePacket(packet);
		}

		void PingServer(void) {
			this->orig_interface->PingServer();
		}

		void PingServer(const char* host, unsigned short serverPort, unsigned short clientPort, bool onlyReplyOnAcceptingConnections) {
			this->orig_interface->PingServer(host, serverPort, clientPort, onlyReplyOnAcceptingConnections);
		}

		int GetAveragePing(void) {
			return this->orig_interface->GetAveragePing();
		}

		int GetLastPing(void) {
			return this->orig_interface->GetLastPing();
		}

		int GetLowestPing(void) {
			return this->orig_interface->GetLowestPing();
		}

		int GetPlayerPing(const PlayerID playerId) {
			return this->orig_interface->GetPlayerPing(playerId);
		}

		void StartOccasionalPing(void) {
			this->orig_interface->StartOccasionalPing();
		}

		void StopOccasionalPing(void) {
			this->orig_interface->StopOccasionalPing();
		}

		bool IsConnected(void) {
			return this->orig_interface->IsConnected();
		}

		unsigned int GetSynchronizedRandomInteger(void) {
			return this->orig_interface->GetSynchronizedRandomInteger();
		}

		bool GenerateCompressionLayer(unsigned int inputFrequencyTable[256], bool inputLayer) {
			return this->orig_interface->GenerateCompressionLayer(inputFrequencyTable, inputLayer);
		}

		bool DeleteCompressionLayer(bool inputLayer) {
			return this->orig_interface->DeleteCompressionLayer(inputLayer);
		}

		void RegisterAsRemoteProcedureCall(int* uniqueID, void(*functionPointer) (RPCParameters *rpcParms)) {
			this->orig_interface->RegisterAsRemoteProcedureCall(uniqueID, functionPointer);
		}

		void RegisterClassMemberRPC(int* uniqueID, void *functionPointer) {
			this->orig_interface->RegisterClassMemberRPC(uniqueID, functionPointer);
		}

		void UnregisterAsRemoteProcedureCall(int* uniqueID) {
			this->orig_interface->UnregisterAsRemoteProcedureCall(uniqueID);
		}

		bool RPC(int* uniqueID, const char *data, unsigned int bitLength, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp) {
			return this->orig_interface->RPC(uniqueID, data, bitLength, priority, reliability, orderingChannel, shiftTimestamp);
		}

		void SetTrackFrequencyTable(bool b) {
			this->orig_interface->SetTrackFrequencyTable(b);
		}

		bool GetSendFrequencyTable(unsigned int outputFrequencyTable[256]) {
			return this->orig_interface->GetSendFrequencyTable(outputFrequencyTable);
		}

		float GetCompressionRatio(void) {
			return this->orig_interface->GetCompressionRatio();
		}

		float GetDecompressionRatio(void) {
			return this->orig_interface->GetDecompressionRatio();
		}

		void AttachPlugin(void *messageHandler) {
			this->orig_interface->AttachPlugin(messageHandler);
		}

		void DetachPlugin(void *messageHandler) {
			this->orig_interface->DetachPlugin(messageHandler);
		}

		RakNet::BitStream * GetStaticServerData(void) {
			return this->orig_interface->GetStaticServerData();
		}

		void SetStaticServerData(const char *data, const int length) {
			this->orig_interface->SetStaticServerData(data, length);
		}

		RakNet::BitStream * GetStaticClientData(const PlayerID playerId) {
			return this->orig_interface->GetStaticClientData(playerId);
		}

		void SetStaticClientData(const PlayerID playerId, const char *data, const int length) {
			this->orig_interface->SetStaticClientData(playerId, data, length);
		}

		void SendStaticClientDataToServer(void) {
			this->orig_interface->SendStaticClientDataToServer();
		}

		PlayerID GetServerID(void) {
			return this->orig_interface->GetServerID();
		}

		PlayerID GetPlayerID(void) {
			return this->orig_interface->GetPlayerID();
		}

		PlayerID GetInternalID(void) {
			return this->orig_interface->GetInternalID();
		}

		const char* PlayerIDToDottedIP(const PlayerID playerId) {
			return this->orig_interface->PlayerIDToDottedIP(playerId);
		}

		void PushBackPacket(Packet *packet, bool pushAtHead) {
			this->orig_interface->PushBackPacket(packet, pushAtHead);
		}

		void SetRouterInterface(void *routerInterface) {
			this->orig_interface->SetRouterInterface(routerInterface);
		}
		void RemoveRouterInterface(void *routerInterface) {
			this->orig_interface->RemoveRouterInterface(routerInterface);
		}

		void SetTimeoutTime(RakNetTime timeMS) {
			this->orig_interface->SetTimeoutTime(timeMS);
		}

		bool SetMTUSize(int size) {
			return this->orig_interface->SetMTUSize(size);
		}

		int GetMTUSize(void) {
			return this->orig_interface->GetMTUSize();
		}

		void AllowConnectionResponseIPMigration(bool allow) {
			this->orig_interface->AllowConnectionResponseIPMigration(allow);
		}

		void AdvertiseSystem(const char *host, unsigned short remotePort, const char *data, int dataLength) {
			this->orig_interface->AdvertiseSystem(host, remotePort, data, dataLength);
		}

		RakNetStatisticsStruct* GetStatistics(void) {
			return this->orig_interface->GetStatistics();
		}

		void ApplyNetworkSimulator(double maxSendBPS, unsigned short minExtraPing, unsigned short extraPingVariance) {
			this->orig_interface->ApplyNetworkSimulator(maxSendBPS, minExtraPing, extraPingVariance);
		}

		bool IsNetworkSimulatorActive(void) {
			return this->orig_interface->IsNetworkSimulatorActive();
		}

		PlayerIndex GetPlayerIndex(void) {
			return this->orig_interface->GetPlayerIndex();
		}

		bool RPC_(int* uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID) {
			if (this->rpc_handler) if (!this->rpc_handler(*uniqueID, bitStream)) return true;
			return this->orig_interface->RPC_(uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID);
		}

	};

	// Перехватчик деструктора samp_info
	static void __declspec(naked) hook_func_samp_destruct() {
		
		static void *ret_addr;
		__asm {
			push eax
			mov eax, dword ptr [esp + 4]
			mov ret_addr, eax
			pop eax
		}

		__asm {
			pushad
			mov ebp, esp
			sub esp, __LOCAL_SIZE
		}

		delete hook_samp_destruct1;
		delete hook_samp_destruct2;

		delete (RakClientInterfaceHook*)(*ppRakClientInterface);
		*ppRakClientInterface = pRakClientInterface;

		net::status = false;

		__asm {
			mov esp, ebp
			popad
			jmp ret_addr
		}
		
	}

	// Вспомогательная функция перехватчика инициализации
	static inline RakClientInterfaceHook* aux_func_hook_init() {
		return new RakClientInterfaceHook(pRakClientInterface, packet_handler, rpc_handler, disconnect_handler);
	}

	// Перехватчик инициализации RakClientInterface
	static volatile bool return_result = false;
	static void __declspec(naked) hook_func_init_rakclient() {
		
		static void *ret_addr;
		static RakClientInterface *temp;

		__asm {
			pushad
			mov ebp, esp
			sub esp, __LOCAL_SIZE
		}

		__asm mov pRakClientInterface, eax;

		ret_addr = hook_init_rakclient->get_original_addr();
		delete hook_init_rakclient;

		__asm {
			mov ebx, ret_addr
			add ebx, 2
			mov ebx, dword ptr [ebx]
			add ebx, esi
			mov ppRakClientInterface, ebx
		}

		if (temp = aux_func_hook_init())
			return_result = status = true;

		ResumeThread(thread_init);

		__asm {
			mov esp, ebp
			popad
			mov eax, temp;
			jmp ret_addr
		}
		
	}

	// Инициализировать сетевой модуль
	static bool init(
		packet_handler_t packet_handler = nullptr,
		rpc_handler_t rpc_handler = nullptr,
		disconnect_handler_t disconnect_handler = nullptr
	) {
		
		if (net::status) return false;
		net::return_result = false;

		net::rpc_handler		= rpc_handler;
		net::packet_handler		= packet_handler;
		net::disconnect_handler	= disconnect_handler;

		if (!addresses::code_init_rakclient_addr ||
			!addresses::code_call_destructor1_addr ||
			!addresses::code_call_destructor2_addr
		) {
			LogError("net", "[error] : [net] : [init] : addresses not initialize");
			return false;
		}

		if (!(hook_init_rakclient = new memory::hooks::jump_hook(
			addresses::code_init_rakclient_addr,
			hook_func_init_rakclient
		))) {
			LogError("net", "could not allocate memory for hook_init_rakclient");
			return false;
		} if (!(hook_samp_destruct1 = new memory::hooks::call_hook(
			addresses::code_call_destructor1_addr,
			hook_func_samp_destruct
		))) {
			LogError("net", "could not allocate memory for hook_samp_destruct1");
			return false;
		} if (!(hook_samp_destruct2 = new memory::hooks::call_hook(
			addresses::code_call_destructor2_addr,
			hook_func_samp_destruct
		))) {
			LogError("net", "could not allocate memory for hook_samp_destruct2");
			return false;
		}

		// Входим в режим ожидания
		if (DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &thread_init, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
			if (SuspendThread(thread_init) == -1) {
				LogError("net", "could not suspend thread");
				CloseHandle(thread_init);
				return false;
			} CloseHandle(thread_init);
		} else {
			LogError("net", "could not duplicate handle current thread");
			return false;
		}

		return return_result;

	}

	// Освободить сетевой модуль
	static inline void free() {
		
		if (!net::status) return;

		delete hook_samp_destruct1;
		delete hook_samp_destruct2;

		delete (RakClientInterfaceHook*)(*ppRakClientInterface);
		*ppRakClientInterface = pRakClientInterface;

		net::status = false;

	}

	// Отправить пакет
	static inline bool send(
		void *data,
		unsigned int size
	) {
		if (!net::status) return false;
		RakNet::BitStream bs(reinterpret_cast<unsigned char *>(data), size, false);
		return pRakClientInterface->Send(&bs, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, '\0');
	}

	// Отправить пакет
	static inline bool send(
		RakNet::BitStream *bs
	) {
		if (!net::status) return false;
		return pRakClientInterface->Send(bs, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, '\0');
	}

}
