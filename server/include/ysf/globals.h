#pragma once

#include "utils/cvector.h"
#include "utils/cvector2d.h"
#include "utils/memory.h"
#include "addresses.h"
#include "structs.h"

typedef void(*logprintf_t)(const char* format, ...);
extern logprintf_t logprintf;
extern void *pAMXFunctions;
extern void **ppPluginData;
extern CNetGame *pNetGame;
