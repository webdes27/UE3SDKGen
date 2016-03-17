#pragma once
#pragma comment(lib, "Psapi.lib")

#include <Windows.h>
#include <Psapi.h>

MODULEINFO GetModuleInfo(LPCTSTR lpModuleName);
DWORD FindPattern(DWORD startAddres, DWORD fileSize, PBYTE pattern, char mask[]);
DWORD FindPattern(MODULEINFO module, PBYTE pattern, char mask[], DWORD offset);