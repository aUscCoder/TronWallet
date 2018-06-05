// author: TuotuoXP
#pragma once
#include "../Universal.h"


typedef struct __TuoDLL__ {} *HTUOMODULE;


namespace Universal
{
	namespace TuoMemoryDLLLoader
	{
		UNIVERSAL_LIB_API HTUOMODULE LoadLibrary(const void *pDLLData);
		UNIVERSAL_LIB_API FARPROC GetProcAddress(HTUOMODULE hModule, const char *pName);
	}
}
