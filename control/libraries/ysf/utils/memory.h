#pragma once

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#endif

bool Unlock(void *address, size_t len);
bool memory_compare(const unsigned char *data, const unsigned char *pattern, const char *mask);
unsigned long FindPattern(const char *pattern, const char *mask);
