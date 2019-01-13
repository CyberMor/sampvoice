#ifndef YSF_TYPES_H
#define YSF_TYPES_H

#include <stddef.h>

#define MAX_PATH 260
#define NULL 0
//typedef unsigned long DWORD;
typedef int BOOL;
//typedef unsigned char BYTE;
//typedef unsigned short WORD;
typedef float FLOAT;

#ifdef _WIN32
#define OVERRIDE override
#else
#define OVERRIDE
#endif

#endif