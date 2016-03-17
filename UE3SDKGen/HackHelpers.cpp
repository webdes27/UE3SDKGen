#include "stdafx.h"
#include "HackHelpers.h"

MODULEINFO GetModuleInfo(LPCTSTR lpModuleName)
{
	MODULEINFO miInfos = { NULL };

	HMODULE hmModule = GetModuleHandle(lpModuleName);

	if (hmModule)
	{
		GetModuleInformation(GetCurrentProcess(), hmModule, &miInfos, sizeof(MODULEINFO));
	}

	return miInfos;
}

DWORD FindPattern(DWORD startAddres, DWORD fileSize, PBYTE pattern, char mask[])
{
	DWORD pos = 0;
	int searchLen = strlen(mask) - 1;

	for (DWORD retAddress = startAddres; retAddress < startAddres + fileSize; retAddress++)
	{
		if (*(PBYTE)retAddress == pattern[pos] || mask[pos] == '?')
		{
			if (mask[pos + 1] == '\0')
			{
				return (retAddress - searchLen);
			}

			pos++;
		}
		else
		{
			pos = 0;
		}
	}

	return NULL;
}

DWORD FindPattern(MODULEINFO module, PBYTE pattern, char mask[], DWORD offset) {
	return FindPattern((unsigned long)module.lpBaseOfDll, module.SizeOfImage, pattern, mask) + offset;
}