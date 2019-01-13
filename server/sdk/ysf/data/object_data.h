#ifndef _OBJECTDATA_H_
#define _OBJECTDATA_H_

#include <unordered_map>

class CObjectData {
public:
	WORD wObjectID;
	CObjectData(WORD objectid) : wObjectID(objectid) {}
	CObjectData(size_t objectid) : CObjectData(static_cast<WORD>(objectid)) {}
	WORD attachedToPlayer = INVALID_PLAYER_ID;
};

#endif
