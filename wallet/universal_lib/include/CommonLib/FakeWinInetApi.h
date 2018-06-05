// Author:	Yanghao
#pragma once

__declspec(selectany) HMODULE g_hWinInetModule = (HMODULE)-1;


inline HMODULE GetWinInetModule()
{
	if (g_hWinInetModule == (HMODULE)-1)
	{
		OSVERSIONINFO os = { sizeof(OSVERSIONINFO) };
		if (::GetVersionEx(&os) && os.dwMajorVersion >= 5)
			g_hWinInetModule = ::LoadLibraryW(L"Wininet.dll");
		else
			g_hWinInetModule = NULL;
	}
	return g_hWinInetModule;
}

#define WININET_API(TYPE, NAME, ARGS, CALLARGS) \
	typedef TYPE (WINAPI * __t##NAME) ARGS; \
	__declspec(selectany) __t##NAME __f##NAME = (__t##NAME)-1; \
	static TYPE SE_##NAME ARGS \
{ \
	if (__f##NAME == (__t##NAME)-1) \
	{ \
		HMODULE h = ::GetWinInetModule(); \
		if (h) \
			__f##NAME = (__t##NAME)::GetProcAddress(h, #NAME); \
		else \
			__f##NAME = NULL; \
	} \
	if (__f##NAME) \
	{ \
		__try { return __f##NAME CALLARGS; } \
		__except(EXCEPTION_EXECUTE_HANDLER) { } \
	} \
	return (TYPE)(0); \
}

