#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include <pawn/amx/amx.h>
#include <raknet/networktypes.h>

#ifdef _WIN32
#define STDCALL __stdcall
#define THISCALL __thiscall
#define FASTCALL __fastcall
#else
#define STDCALL
#define THISCALL
#define FASTCALL
#define CDECL
#endif

// Указатель на игровой класс
extern CNetGame *pNetGame;

// Типы функций сервера
// ===========================================================================================================================

struct ConsoleVariable_s;

// Функции CConsole
typedef void				(THISCALL *console_addstringvariable_t)			(void *pConsole, char *szRule, int flags, char *szString, void *changefunc);
typedef char*				(THISCALL *console_getstringvariable_t)			(void *pConsole, char *szRule);
typedef void				(THISCALL *console_setstringvariable_t)			(void *pConsole, char *szRule, char *szString);
typedef int					(THISCALL *console_getintvariable_t)			(void *pConsole, char *szRule);
typedef void				(THISCALL *console_setintvariable_t)			(void *pConsole, char *szRule, int value);
typedef bool				(THISCALL *console_getboolvariable_t)			(void *pConsole, char *szRule);
typedef void				(THISCALL *console_modifyvariableflags_t)		(void *pConsole, char *szRule, int value);
typedef ConsoleVariable_s*	(THISCALL *console_findvariable_t)				(void *pConsole, char *szRule);
typedef void				(THISCALL *console_sendrules_t)					(void *pConsole, SOCKET s, char* data, const sockaddr_in* to, int tolen);
typedef void				(THISCALL *console_execute_t)					(void *pConsole, const char *pExecLine);

// Функции CFilterScripts
typedef bool				(THISCALL *filterscripts_loadfilterscript_t)	(void *pFilterscriptPool, const char *szName);
typedef bool				(THISCALL *filterscripts_unloadfilterscript_t)	(void *pFilterscriptPool, const char *szName);

// Функции CPlayer
typedef void				(THISCALL *player_spawnforworld_t)				(void *pPlayer);

// Функции CPlayerPool
typedef DWORD				(THISCALL *playerpool_handlevehiclerespawn_t)	(CPlayerPool *pPlayerPool, WORD wVehicleID);

// Функции CObject
typedef void				(THISCALL *object_spawnforplayer_t)				(void *pObject, WORD playerID);

// Функции RakNet
typedef bool				(THISCALL *raknet_start_t)						(void* ppRakServer, unsigned short AllowedPlayers, unsigned int depreciated, int threadSleepTimer, unsigned short port, const char *forceHostAddress);
typedef bool				(THISCALL *raknet_send_t)						(void* ppRakServer, BitStream* parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast);
typedef Packet*				(THISCALL *raknet_receive_t)					(void* ppRakServer);
typedef void				(THISCALL *raknet_deallocatepacket_t)			(void* ppRakServer, Packet *packet);
typedef void				(THISCALL *raknet_setallowedplayers_t)			(void* ppRakServer, unsigned short numberAllowed);
typedef int					(THISCALL *raknet_getlastping_t)				(void* ppRakServer, const PlayerID playerId);
typedef void				(THISCALL *raknet_registerasrpc_t)				(void* ppRakServer, int* uniqueID, void(*functionPointer) (RPCParameters *rpcParms));
typedef void				(THISCALL *raknet_unregisterasrpc_t)			(void* ppRakServer, int* uniqueID);
typedef bool				(THISCALL *raknet_rpc_t)						(void* ppRakServer, BYTE* uniqueID, BitStream* parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp);
typedef const char*			(THISCALL *raknet_getlocalip_t)					(void* ppRakServer, unsigned int index);
typedef int					(THISCALL *raknet_getindexfromplayerid_t)		(void* ppRakServer, PlayerID playerId);
typedef PlayerID			(THISCALL *raknet_getplayeridfromindex_t)		(void* ppRakServer, int index);
typedef void				(THISCALL *raknet_settimeouttime_t)				(void* ppRakServer, RakNetTime timeMS, const PlayerID target);
typedef void				(THISCALL *raknet_addtobanlist_t)				(void* ppRakServer, const char *IP, unsigned int milliseconds);
typedef void				(THISCALL *raknet_removefrombanlist_t)			(void* ppRakServer, const char *IP);
typedef void				(THISCALL *raknet_clearbanlist_t)				(void* ppRakServer);

// Прочие функции
typedef int					(THISCALL *packet_weaponsupdate_t)				(void *pNetGame, Packet *p);
typedef int					(THISCALL *packet_statsupdate_t)				(void *pNetGame, Packet *p);
typedef char*				(CDECL    *format_amxstring_t)					(AMX *amx, cell *params, int parm, int &len);

// ===========================================================================================================================

// Функционал RakNet
class raknet {
private:

	static void *rakserver;

	static raknet_start_t					func_start;
	static raknet_send_t					func_send;
	static raknet_receive_t					func_receive;
	static raknet_deallocatepacket_t		func_deallocatepacket;
	static raknet_setallowedplayers_t		func_setallowedplayers;
	static raknet_getlastping_t				func_getlastping;
	static raknet_registerasrpc_t			func_registerasrpc;
	static raknet_unregisterasrpc_t			func_unregisterasrpc;
	static raknet_rpc_t						func_rpc;
	static raknet_getlocalip_t				func_getlocalip;
	static raknet_getindexfromplayerid_t	func_getindexfromplayerid;
	static raknet_getplayeridfromindex_t	func_getplayeridfromindex;
	static raknet_settimeouttime_t			func_settimeouttime;
	static raknet_addtobanlist_t			func_addtobanlist;
	static raknet_removefrombanlist_t		func_removefrombanlist;
	static raknet_clearbanlist_t			func_clearbanlist;

public:

	// Инициализировать адреса функций
	static void init(
		void *rakserver
	) {
		raknet::rakserver = rakserver;
		const auto rakserver_vtable = *reinterpret_cast<DWORD**>(rakserver);
		raknet::func_start = reinterpret_cast<raknet_start_t>(rakserver_vtable[RAKNET_START_OFFSET]);
		raknet::func_send = reinterpret_cast<raknet_send_t>(rakserver_vtable[RAKNET_SEND_OFFSET]);
		raknet::func_receive = reinterpret_cast<raknet_receive_t>(rakserver_vtable[RAKNET_RECEIVE_OFFSET]);
		raknet::func_deallocatepacket = reinterpret_cast<raknet_deallocatepacket_t>(rakserver_vtable[RAKNET_DEALLOCATE_PACKET_OFFSET]);
		raknet::func_setallowedplayers = reinterpret_cast<raknet_setallowedplayers_t>(rakserver_vtable[RAKNET_SET_ALLOWED_PLAYERS_OFFSET]);
		raknet::func_getlastping = reinterpret_cast<raknet_getlastping_t>(rakserver_vtable[RAKNET_GET_LAST_PING_OFFSET]);
		raknet::func_registerasrpc = reinterpret_cast<raknet_registerasrpc_t>(rakserver_vtable[RAKNET_REGISTER_RPC_OFFSET]);
		raknet::func_unregisterasrpc = reinterpret_cast<raknet_unregisterasrpc_t>(rakserver_vtable[RAKNET_UNREGISTER_RPC_OFFSET]);
		raknet::func_rpc = reinterpret_cast<raknet_rpc_t>(rakserver_vtable[RAKNET_RPC_OFFSET]);
		raknet::func_getlocalip = reinterpret_cast<raknet_getlocalip_t>(rakserver_vtable[RAKNET_GET_LOCAL_IP_OFFSET]);
		raknet::func_getindexfromplayerid = reinterpret_cast<raknet_getindexfromplayerid_t>(rakserver_vtable[RAKNET_GET_INDEX_FROM_PLAYERID_OFFSET]);
		raknet::func_getplayeridfromindex = reinterpret_cast<raknet_getplayeridfromindex_t>(rakserver_vtable[RAKNET_GET_PLAYERID_FROM_INDEX_OFFSET]);
		raknet::func_settimeouttime = reinterpret_cast<raknet_settimeouttime_t>(rakserver_vtable[RAKNET_ADD_BAN_OFFSET]);
		raknet::func_addtobanlist = reinterpret_cast<raknet_addtobanlist_t>(rakserver_vtable[RAKNET_REMOVE_BAN_OFFSET]);
		raknet::func_removefrombanlist = reinterpret_cast<raknet_removefrombanlist_t>(rakserver_vtable[RAKNET_CLEAR_BAN_OFFSET]);
		raknet::func_clearbanlist = reinterpret_cast<raknet_clearbanlist_t>(rakserver_vtable[RAKNET_SET_TIMEOUT_OFFSET]);
	}

	static inline bool start(unsigned short allowedplayers, unsigned int depreciated, int threadsleeptimer, unsigned short port, const char *forcehostaddress) {
		return raknet::func_start(rakserver, allowedplayers, depreciated, threadsleeptimer, port, forcehostaddress);
	}

	static inline bool send(BitStream* parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingchannel, PlayerID playerid, bool broadcast) {
		return raknet::func_send(rakserver, parameters, priority, reliability, orderingchannel, playerid, broadcast);
	}

	static inline Packet* receive() {
		return raknet::func_receive(rakserver);
	}

	static inline void deallocate_packet(Packet *packet) {
		raknet::func_deallocatepacket(rakserver, packet);
	}

	static inline void setallowedplayers(unsigned short numberallowed) {
		return raknet::func_setallowedplayers(rakserver, numberallowed);
	}

	static inline int getlastping(const PlayerID playerid) {
		return raknet::func_getlastping(rakserver, playerid);
	}

	static inline void registerasrpc(int* rpc_id, void(*rpc_handler)(RPCParameters*)) {
		raknet::func_registerasrpc(rakserver, rpc_id, rpc_handler);
	}

	static inline void unregisterasrpc(int* rpc_id) {
		raknet::func_unregisterasrpc(rakserver, rpc_id);
	}

	static inline bool rpc(BYTE* rpc_id, BitStream* parameters, PacketPriority priority, PacketReliability reliability, unsigned orderingchannel, PlayerID playerid, bool broadcast, bool shift_timestamp) {
		return raknet::func_rpc(rakserver, rpc_id, parameters, priority, reliability, orderingchannel, playerid, broadcast, shift_timestamp);
	}

	static inline const char* getlocalip(unsigned int index) {
		return raknet::func_getlocalip(rakserver, index);
	}

	static inline int getindexfromplayerid(PlayerID playerid) {
		return raknet::func_getindexfromplayerid(rakserver, playerid);
	}

	static inline PlayerID getplayeridfromindex(int index) {
		return raknet::func_getplayeridfromindex(rakserver, index);
	}

	static inline void settimeouttime(RakNetTime time_ms, const PlayerID target) {
		raknet::func_settimeouttime(rakserver, time_ms, target);
	}

	static inline void addtobanlist(const char *ip, unsigned int milliseconds) {
		raknet::func_addtobanlist(rakserver, ip, milliseconds);
	}

	static inline void removefrombanlist(const char *ip) {
		raknet::func_removefrombanlist(rakserver, ip);
	}

	static inline void clearbanlist() {
		raknet::func_clearbanlist(rakserver);
	}

};

// Функционал Console
class console {
private:

	// Указатель на консоль
	static void *pconsole;

public:

	// Инициализировать консоль
	static void init(
		void *pConsole
	) {
		console::pconsole = pConsole;
	}

	// Выполнить команду
	static inline void execute(
		const char *exec_line
	) {
		if (pconsole)
			((console_execute_t)(CAddress::FUNC_CConsole__Execute))(pconsole, exec_line);
	}

};

// Функционал SAMP
class samp {
public:

	// Проверить наличие игрока
	static inline bool player_is_connect(
		uint16_t player_id
	) {
		return pNetGame->pPlayerPool->bIsPlayerConnected[player_id];
	}

	// Получить текущий тик сервера
	static inline uint32_t get_tick() {
		return pNetGame->dwSomethingTick;
	}

	// Получить ник игрока
	static inline const char* player_get_name(
		uint16_t player_id
	) {
		return pNetGame->pPlayerPool->szName[player_id];
	}



};

void							*raknet::rakserver = nullptr;
raknet_start_t					raknet::func_start;
raknet_send_t					raknet::func_send;
raknet_receive_t				raknet::func_receive;
raknet_deallocatepacket_t		raknet::func_deallocatepacket;
raknet_setallowedplayers_t		raknet::func_setallowedplayers;
raknet_getlastping_t			raknet::func_getlastping;
raknet_registerasrpc_t			raknet::func_registerasrpc;
raknet_unregisterasrpc_t		raknet::func_unregisterasrpc;
raknet_rpc_t					raknet::func_rpc;
raknet_getlocalip_t				raknet::func_getlocalip;
raknet_getindexfromplayerid_t	raknet::func_getindexfromplayerid;
raknet_getplayeridfromindex_t	raknet::func_getplayeridfromindex;
raknet_settimeouttime_t			raknet::func_settimeouttime;
raknet_addtobanlist_t			raknet::func_addtobanlist;
raknet_removefrombanlist_t		raknet::func_removefrombanlist;
raknet_clearbanlist_t			raknet::func_clearbanlist;

void							*console::pconsole = nullptr;

#endif