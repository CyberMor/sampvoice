#ifndef _CPLAYERDATA_H_
#define _CPLAYERDATA_H_

#include <map>
#include <bitset>
#include <chrono>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>

using default_clock = std::chrono::steady_clock;

extern CNetGame *pNetGame;

class CPlayerObjectAttachAddon {
public:
	WORD wObjectID = INVALID_OBJECT_ID;
	WORD wAttachPlayerID = INVALID_PLAYER_ID;
	CVector vecOffset;
	CVector vecRot;
	default_clock::time_point creation_timepoint;
	bool bCreated = false;
	bool bAttached = false;
};

class CPlayerData {
public:

	CPlayerData(WORD playerid) : wPlayerID(playerid) {
		static DWORD dwPlayerColors[100] = {
			0xFF8C13FF,0xC715FFFF,0x20B2AAFF,0xDC143CFF,0x6495EDFF,0xf0e68cFF,0x778899FF,0xFF1493FF,0xF4A460FF,
			0xEE82EEFF,0xFFD720FF,0x8b4513FF,0x4949A0FF,0x148b8bFF,0x14ff7fFF,0x556b2fFF,0x0FD9FAFF,0x10DC29FF,
			0x534081FF,0x0495CDFF,0xEF6CE8FF,0xBD34DAFF,0x247C1BFF,0x0C8E5DFF,0x635B03FF,0xCB7ED3FF,0x65ADEBFF,
			0x5C1ACCFF,0xF2F853FF,0x11F891FF,0x7B39AAFF,0x53EB10FF,0x54137DFF,0x275222FF,0xF09F5BFF,0x3D0A4FFF,
			0x22F767FF,0xD63034FF,0x9A6980FF,0xDFB935FF,0x3793FAFF,0x90239DFF,0xE9AB2FFF,0xAF2FF3FF,0x057F94FF,
			0xB98519FF,0x388EEAFF,0x028151FF,0xA55043FF,0x0DE018FF,0x93AB1CFF,0x95BAF0FF,0x369976FF,0x18F71FFF,
			0x4B8987FF,0x491B9EFF,0x829DC7FF,0xBCE635FF,0xCEA6DFFF,0x20D4ADFF,0x2D74FDFF,0x3C1C0DFF,0x12D6D4FF,
			0x48C000FF,0x2A51E2FF,0xE3AC12FF,0xFC42A8FF,0x2FC827FF,0x1A30BFFF,0xB740C2FF,0x42ACF5FF,0x2FD9DEFF,
			0xFAFB71FF,0x05D1CDFF,0xC471BDFF,0x94436EFF,0xC1F7ECFF,0xCE79EEFF,0xBD1EF2FF,0x93B7E4FF,0x3214AAFF,
			0x184D3BFF,0xAE4B99FF,0x7E49D7FF,0x4C436EFF,0xFA24CCFF,0xCE76BEFF,0xA04E0AFF,0x9F945CFF,0xDCDE3DFF,
			0x10C9C5FF,0x70524DFF,0x0BE472FF,0x8A2CD7FF,0x6152C2FF,0xCF72A9FF,0xE59338FF,0xEEDC2DFF,0xD8C762FF,
			0xD8C762FF
		};

		if (pNetGame->pPlayerPool->pPlayer[playerid])
			pNetGame->pPlayerPool->pPlayer[playerid]->dwNickNameColor = dwPlayerColors[playerid % 100];

		/*if (pNetGame->pPlayerPool->bIsNPC[playerid])
		{
		if (raknet::getplayeridfromindex(playerid).binaryAddress == 0x0100007F)
		iNPCProcessID = CPlugin::Get()->FindNPCProcessID(playerid);
		}*/
	}

	CPlayerData(size_t playerid) : CPlayerData(static_cast<WORD>(playerid)) {}

	~CPlayerData() {
		for (WORD i = 0; i != MAX_OBJECTS; ++i)
			DeleteObjectAddon(i);
	}
	
	WORD wPlayerID;

	bool SetPlayerTeamForPlayer(WORD teamplayerid, int team) {
		m_iTeams[teamplayerid] = team + 1;
		BitStream bs;
		bs.Write((WORD)teamplayerid);
		bs.Write((BYTE)team);
		raknet::rpc(&RPC_SetPlayerTeam, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, raknet::getplayeridfromindex(wPlayerID), 0, 0);
		return true;
	}

	int GetPlayerTeamForPlayer(WORD teamplayerid) {
		CPlayer *p = pNetGame->pPlayerPool->pPlayer[teamplayerid];
		if (m_iTeams[teamplayerid] == 0)
			return p->spawn.byteTeam;
		return m_iTeams[teamplayerid] - 1;
	}

	inline void ResetPlayerTeam(WORD playerid) { m_iTeams[playerid] = 0; }

	bool SetPlayerSkinForPlayer(WORD skinplayerid, int skin) {
		m_iSkins[skinplayerid] = skin + 1;
		BitStream bs;
		bs.Write((int)skinplayerid);
		bs.Write((int)skin);
		raknet::rpc(&RPC_SetPlayerSkin, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, raknet::getplayeridfromindex(wPlayerID), 0, 0);
		return true;
	}

	int GetPlayerSkinForPlayer(WORD skinplayerid) {
		CPlayer *p = pNetGame->pPlayerPool->pPlayer[skinplayerid];
		if (m_iSkins[skinplayerid] == 0)
			return p->spawn.iSkin;
		return m_iSkins[skinplayerid] - 1;
	}

	inline void ResetPlayerSkin(WORD playerid) { m_iSkins[playerid] = 0; }

	bool SetPlayerNameForPlayer(WORD nameplayerid, const char *name) {
		m_PlayerNames[nameplayerid] = std::string(name);
		BYTE len = static_cast<BYTE>(strlen(name));
		BitStream bs;
		bs.Write((WORD)nameplayerid);
		bs.Write((BYTE)len);
		bs.Write(name, len);
		bs.Write((BYTE)1);
		raknet::rpc(&RPC_SetPlayerName, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, raknet::getplayeridfromindex(wPlayerID), 0, 0);
		return true;
	}

	const char *GetPlayerNameForPlayer(WORD nameplayerid) {
		auto n = m_PlayerNames.find(nameplayerid);
		if (n == m_PlayerNames.end())
			return pNetGame->pPlayerPool->szName[nameplayerid];
		return n->second.c_str();
	}

	inline void ResetPlayerName(WORD playerid) { m_PlayerNames.erase(playerid); }

	bool SetPlayerFightingStyleForPlayer(WORD styleplayerid, int style) {
		m_iFightingStyles[styleplayerid] = style + 1;
		BitStream bs;
		bs.Write((WORD)styleplayerid);
		bs.Write((BYTE)style);
		raknet::rpc(&RPC_SetFightingStyle, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, raknet::getplayeridfromindex(wPlayerID), 0, 0);
		return true;
	}

	int GetPlayerFightingStyleForPlayer(WORD styleplayerid) {
		CPlayer *p = pNetGame->pPlayerPool->pPlayer[styleplayerid];
		if (m_iFightingStyles[styleplayerid] == 0)
			return p->byteFightingStyle;
		return m_iFightingStyles[styleplayerid] - 1;
	}

	inline void ResetPlayerFightingStyle(WORD playerid){ m_iFightingStyles[playerid] = 0; }

	void ResetPlayerMarkerForPlayer(WORD resetplayerid) {
		CPlayer *p = pNetGame->pPlayerPool->pPlayer[resetplayerid];
		BitStream bs;
		bs.Write(resetplayerid);
		bs.Write(p->dwNickNameColor);
		raknet::rpc(&RPC_SetPlayerColor, &bs, MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, raknet::getplayeridfromindex(wPlayerID), 0, 0);
	}

	WORD GetGangZoneIDFromClientSide(WORD zoneid, bool bPlayer = false) {
		if (!bPlayer) for (WORD wZone = 0; wZone != MAX_GANG_ZONES; ++wZone)
				if (wClientSideGlobalZoneID[wZone] == zoneid) return wZone;
		else for (WORD wZone = 0; wZone != MAX_GANG_ZONES; ++wZone)
				if (wClientSidePlayerZoneID[wZone] == zoneid) return wZone;
		return 0xFFFF;
	}

	bool DestroyObject(WORD objectid) {
		BitStream bs;
		bs.Write(objectid);
		raknet::rpc(&RPC_DestroyObject, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, raknet::getplayeridfromindex(wPlayerID), 0, 0);
		return 1;
	}

	void Process(void) {};
	bool HiddenInQuery() { return bCustomNameInQuery && strNameInQuery.empty(); }

	void DeleteObjectAddon(WORD objectid) {
		auto it = m_PlayerObjectsAddon.find(static_cast<WORD>(objectid));
		if (it != m_PlayerObjectsAddon.end()) {
			m_PlayerObjectsAttachQueue.erase(it->first);
			m_PlayerObjectsAddon.erase(it);
		}
	}

	void ShowObject(WORD objectid, bool sync) {
		m_HiddenObjects.erase(objectid);
		if (sync) {
			//CSAMPraknet::SpawnObjectForPlayer(objectid, wPlayerID);
		}
	}

	void HideObject(WORD objectid, bool sync) {
		if (sync) {
			BitStream bs;
			bs.Write(objectid);
			raknet::rpc(&RPC_DestroyObject, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, raknet::getplayeridfromindex(wPlayerID), 0, 0);
		} m_HiddenObjects.insert(objectid);
	}

	bool NewObjectsHidden() const {
		return m_HideNewObjects;
	}

	void HideNewObjects(bool toggle) {
		m_HideNewObjects = toggle;
	}

	bool IsObjectHidden(WORD objectid) const {
		CObjectPool *pObjectPool = pNetGame->pObjectPool;
		if (!pObjectPool->bObjectSlotState[objectid]) return false;
		CObject *pObject = pNetGame->pObjectPool->pObjects[objectid];
		if (!pObject) return false;
		if (m_HideNewObjects)
			return true;
		return m_HiddenObjects.find(objectid) != m_HiddenObjects.end();
	}

	void SetBuildingsRemoved(int modelid, const CVector &pos, float range) {
		m_RemovedBuildings.emplace_back(modelid, pos, range);
	}

	bool GetBuildingsRemoved() const {
		return !m_RemovedBuildings.empty();
	}

	bool IsBuildingRemoved(int modelid, const CVector &pos, float range) const {
		for (auto &building : m_RemovedBuildings)
			if (modelid == -1 || building.ModelId == -1 || modelid == building.ModelId) {
				CVector d = pos - building.Position;
				if (d.Length() <= range + building.Range)
					return true;
			}
		return false;
	}

	int iNPCProcessID = -1;
	WORD wSurfingInfo = 0;
	WORD wDialogID = -1;
	bool bBroadcastTo = false;
	WORD wDisabledKeys = 0;
	WORD wDisabledKeysUD = 0;
	WORD wDisabledKeysLR = 0;
	float fGravity = 0.0f;
	BYTE byteWeather = 0;
	float fBounds[4] = { 20000.0f, -20000.0f, 20000.0f, -20000.0f };
	std::bitset<MAX_PLAYERS> bCustomQuat;
	std::unordered_map <WORD, std::unique_ptr<CVector>> customPos;
	float fCustomQuat[MAX_PLAYERS][4] = {};

	std::shared_ptr<CPlayerObjectAttachAddon> GetObjectAddon(WORD objectid) {
		std::shared_ptr<CPlayerObjectAttachAddon> pAddon;
		try {
			auto it = m_PlayerObjectsAddon.find(static_cast<WORD>(objectid));
			if (it == m_PlayerObjectsAddon.end()) {
				pAddon = std::make_shared<CPlayerObjectAttachAddon>();
				m_PlayerObjectsAddon.emplace(objectid, pAddon);
			} else pAddon = it->second;
		} catch (...) {}
		return pAddon;
	}

	std::shared_ptr<CPlayerObjectAttachAddon> const FindObjectAddon(WORD objectid) {
		auto it = m_PlayerObjectsAddon.find(static_cast<WORD>(objectid));
		if (it == m_PlayerObjectsAddon.end())
			return NULL;
		return it->second;
	}

	std::unordered_map<WORD, std::shared_ptr<CPlayerObjectAttachAddon>> m_PlayerObjectsAddon;
	std::set<WORD> m_PlayerObjectsAttachQueue;
	std::multimap<WORD, std::pair<BYTE, std::string>> m_PlayerObjectMaterialText;
	bool bCustomNameInQuery = false;
	std::string strNameInQuery;
	class CGangZone *pPlayerZone[MAX_GANG_ZONES] = {};
	BYTE byteClientSideZoneIDUsed[MAX_GANG_ZONES] = {};
	WORD wClientSideGlobalZoneID[MAX_GANG_ZONES] = {};
	WORD wClientSidePlayerZoneID[MAX_GANG_ZONES] = {};
	std::bitset<MAX_GANG_ZONES> bInGangZone;
	std::bitset<MAX_GANG_ZONES> bIsGangZoneFlashing;
	DWORD dwClientSideZoneColor[MAX_GANG_ZONES] = {};
	DWORD dwClientSideZoneFlashColor[MAX_GANG_ZONES] = {};
#ifdef NEW_PICKUP_SYSTEM
	PickupMap ClientPlayerPickups;
	std::bitset<MAX_PICKUPS> bClientPickupSlots;
	std::bitset<MAX_PICKUPS> bClientPickupStreamedIn;
	PickupMap PlayerPickups;
	std::bitset<MAX_PICKUPS> bPlayerPickup;
#endif
	DWORD dwFakePingValue = 0;
	default_clock::time_point LastUpdateTick;
	bool bObjectsRemoved = false;
	bool bWidescreen = false;
	bool bUpdateScoresPingsDisabled = false;
	bool bFakePingToggle = false;
	bool bAFKState = false;
	bool bEverUpdated = false;
	bool bControllable = false;
	bool bAttachedObjectCreated = false;
#ifdef NEW_PLAYER_OBJECT_SYSTEM
	aux::bimap<WORD, WORD> localObjects;
#endif

private:
	struct RemovedBuilding {
		int ModelId;
		CVector Position;
		float Range;
		RemovedBuilding(int modelid, const CVector &pos, float range) : ModelId(modelid), Position(pos), Range(range) {}
	};
	int m_iTeams[MAX_PLAYERS] = {};
	int m_iSkins[MAX_PLAYERS] = {};
	int m_iFightingStyles[MAX_PLAYERS] = {};
	std::unordered_map<WORD, std::string> m_PlayerNames;
	std::unordered_set<WORD> m_HiddenObjects;
	bool m_HideNewObjects = false;
	std::vector<RemovedBuilding> m_RemovedBuildings;
};

#endif
