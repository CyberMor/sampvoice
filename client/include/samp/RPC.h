/*
	This is a SAMP (0.3.7-R1/R3) API project file.
	Developer: LUCHARE <luchare.dev@gmail.com>
	
	See more here https://github.com/LUCHARE/SAMP-API
	
	Copyright (c) 2018 BlastHack Team <BlastHack.Net>. All rights reserved.
*/

#pragma once

#include "commonSA.h"
#include "CNetGame.h"

SAMP_BEGIN

namespace RPC {

	namespace Incoming {}

	namespace Outcoming {

		struct SAMP_API DeathNotification {

			unsigned char m_nReason;
			unsigned short m_nKiller;

			static int ID;

		};

		struct SAMP_API Spawn {

			static int ID;

		};

		struct SAMP_API UpdateVehicleDamage {

			unsigned short m_nVehicle;
			unsigned int m_nPanels;
			unsigned int m_nDoors;
			unsigned char m_nLights;
			unsigned char m_nTyres;

			static int ID;

		};

		struct SAMP_API ClassRequest {

			int m_nClass;

			static int ID;

		};

		struct SAMP_API SpawnRequest {

			static int ID;

		};

		struct SAMP_API InteriorChangeNotification {

			unsigned char m_nId;

			static int ID;

		};

		struct SAMP_API EnterVehicleNotification {

			unsigned short m_nVehicle;
			bool m_bPassenger;

			static int ID;

		};

		struct SAMP_API ExitVehicleNotification {

			unsigned short m_nVehicle;

			static int ID;

		};

		struct SAMP_API UpdatePlayersInfo {

			static int ID;

		};

		struct SAMP_API ClickPlayer {

			unsigned short	m_nPlayer;
			char m_nSource; // wtf

			static int ID;

		};
	}

#if defined(__RAK_CLIENT_H)

	template<class T>
	void Send(T params, bool bEmptyBs = false, PacketPriority nPriority = HIGH_PRIORITY, PacketReliability nReliability = RELIABLE_ORDERED, char nOrderingChannel = 0, bool bShiftTimestamp = false, NetworkID networkId = UNASSIGNED_NETWORK_ID, RakNet::BitStream* pReplyFromTarget = nullptr) {
		RakNet::BitStream bs;

		if (!bEmptyBs)
			bs.Write(params);

		pNetGame->m_pRakClient->RPC(&T::ID, &bs, nPriority, nReliability, nOrderingChannel, bShiftTimestamp, networkId, pReplyFromTarget);
	}

	template<class T>
	T Read(RPCParameters* pParams) {
		RakNet::BitStream bs(pParams->input, pParams->numberOfBitsOfData / 8 + 1, false);
		T params;

		bs.Read(params);

		return params;
	}

#endif

}

SAMP_END
