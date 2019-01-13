#ifndef YSF_MEMORY_H
#define YSF_MEMORY_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#else
#include <unistd.h>
#include <sys/mman.h>
#endif

bool Unlock(void *address, size_t len) {
#ifdef _WIN32
	DWORD oldp;
	return !!VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, &oldp);
#else
	size_t iPageSize = getpagesize(), iAddr = ((reinterpret_cast <uint32_t>(address) / iPageSize) * iPageSize);
	return !mprotect(reinterpret_cast <void*>(iAddr), len, PROT_READ | PROT_WRITE | PROT_EXEC);
#endif
}

bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask) {
	for (; *mask; ++mask, ++data, ++pattern)
		if (*mask == 'x' && *data != *pattern) return false;
	return (*mask) == NULL;
}

DWORD FindPattern(const char *pattern, const char *mask) {
	DWORD i;
	DWORD size;
	DWORD address;
#ifdef _WIN32
	MODULEINFO info = { 0 };
	address = (DWORD)GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
	size = (DWORD)info.SizeOfImage;
#else
	address = 0x804b480;
	size = 0x8128B80 - address;
#endif
	for (i = 0; i < size; ++i)
		if (memory_compare((BYTE *)(address + i), (BYTE *)pattern, mask)) return (DWORD)(address + i);
	return 0;
}

void InstallJump(unsigned long addr, void *func) {
#ifdef WIN32
	unsigned long dwOld;
	VirtualProtect((LPVOID)addr, 5, PAGE_EXECUTE_READWRITE, &dwOld);
#else
	int pagesize = sysconf(_SC_PAGE_SIZE);
	void *unpraddr = (void *)(((int)addr + pagesize - 1) & ~(pagesize - 1)) - pagesize;
	mprotect(unpraddr, pagesize, PROT_WRITE);
#endif
	*(unsigned char *)addr = 0xE9;
	*(unsigned long *)((unsigned long)addr + 0x1) = (unsigned long)func - (unsigned long)addr - 5;
#ifdef WIN32
	VirtualProtect((LPVOID)addr, 5, dwOld, &dwOld);
#else
	mprotect(unpraddr, pagesize, PROT_READ | PROT_EXEC);
#endif
}

#endif