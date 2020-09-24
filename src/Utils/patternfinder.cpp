#include "patternfinder.h"
#include "../hooker.h"
#include "../Utils/util.h"

// original code by dom1n1k and Patrick at GameDeception
inline bool Compare(const unsigned char* pData, const unsigned char* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;

	return (*szMask) == 0;
}

uintptr_t PatternFinder::FindPattern(uintptr_t dwAddress, uintptr_t dwLen, unsigned char* bMask, const char* szMask)
{
	for (uintptr_t i = 0; i < dwLen; i++)
		if (Compare((unsigned char*)(dwAddress + i), bMask, szMask))
			return (uintptr_t)(dwAddress + i);

	return 0;
}

uintptr_t PatternFinder::FindPatternInModule(const char* moduleName, unsigned char* bMask, const char* szMask)
{
	uintptr_t baseAddress;
	size_t memSize;

	if (!Hooker::GetLibraryInformation(moduleName, &baseAddress, &memSize)){
		Util::Log("(FindPatternInModule): ERROR: Could Not Get info for Module %s\n", moduleName);
		return 0;
	}

	uintptr_t ret = FindPattern(baseAddress, memSize, bMask, szMask);
	if( !ret ){
		Util::Log("(FindPatternInModule): ERROR: Could not find pattern (%s)(%s)\n", moduleName, szMask );
	}
	return ret;
}