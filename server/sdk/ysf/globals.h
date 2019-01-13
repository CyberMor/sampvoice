#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "includes/net.h"
#include "includes/platform.h"

#include "utils/bimap.h"
#include "utils/cvector.h"
#include "utils/cvector2d.h"
#include "utils/memory.h"

#include "rpc.h"
#include "addresses.h"
#include "structs.h"
#include "functions.h"

#include "data/vehicle_data.h"
#include "data/player_data.h"
#include "data/object_data.h"

typedef void(*logprintf_t)(const char* format, ...);
logprintf_t logprintf;

extern void *pAMXFunctions;
void **ppPluginData;

CNetGame *pNetGame;
void *pConsole;
void *pRakServer;

#endif