#ifndef _CVEHICLEDATA_H_
#define _CVEHICLEDATA_H_

#include <unordered_map>

class CVehicleData {
public:
	WORD wVehicleID;
	CVehicleData(WORD vehicleid) : wVehicleID(vehicleid) {}
	CVehicleData(size_t vehicleid) : CVehicleData(static_cast<WORD>(vehicleid)) {}
	bool bChangedVehicleColor = false;
	bool customSpawnData;
	CVehicleSpawn spawnData = {};
};

#endif
