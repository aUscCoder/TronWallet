// Author: TuotuoXP
#pragma once
#include <winternl.h>  // for NTSTATUS.
#include "tuoassert.h"


__declspec(selectany) BOOL (WINAPI * AlphaBlendProc)(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest, HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn) = NULL; // NOLINT (4.5.66 windows函数定义)
__declspec(selectany) BOOL (WINAPI * GradientFillProc)(HDC hdc, PTRIVERTEX pVertex, ULONG dwNumVertex, PVOID pMesh, ULONG dwNumMesh, ULONG dwMode) = NULL; // NOLINT (4.5.67 windows函数定义)

inline BOOL TuoAlphaBlend(HDC hdcDest, int xoriginDest, int yoriginDest, int wDest, int hDest, HDC hdcSrc, int xoriginSrc, int yoriginSrc, int wSrc, int hSrc, BLENDFUNCTION ftn) // NOLINT (4.5.67 windows函数定义)
{
	if (AlphaBlendProc == NULL)
	{
		HMODULE hGdi32 = ::LoadLibraryW(L"gdi32.dll");
		(FARPROC&)AlphaBlendProc = ::GetProcAddress(hGdi32, "GdiAlphaBlend");
	}
	return AlphaBlendProc(hdcDest, xoriginDest, yoriginDest, wDest, hDest, hdcSrc, xoriginSrc, yoriginSrc, wSrc, hSrc, ftn);
}

inline BOOL TuoGradientFill(HDC hdc, PTRIVERTEX pVertex, ULONG dwNumVertex, PVOID pMesh, ULONG dwNumMesh, ULONG dwMode)
{
	if (GradientFillProc == NULL)
	{
		HMODULE hGdi32 = ::LoadLibraryW(L"gdi32.dll");
		(FARPROC&)GradientFillProc = ::GetProcAddress(hGdi32, "GdiGradientFill");
	}
	return GradientFillProc(hdc, pVertex, dwNumVertex, pMesh, dwNumMesh, dwMode);
}


//////////////////////////////////////////////////////////////////////////


inline DWORD TuoGetThreadID(HANDLE hThread)
{
	typedef NTSTATUS (WINAPI *NtQueryInformationThreadF)(HANDLE ThreadHandle, DWORD ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength, PULONG ReturnLength); // NOLINT (4.5.66 windows函数定义)
	static NtQueryInformationThreadF f = (NtQueryInformationThreadF)-1;
	if (f == (NtQueryInformationThreadF)-1)
	{
		HMODULE hModule = ::GetModuleHandleW(L"ntdll.dll");
		if (hModule)
			f = (NtQueryInformationThreadF)::GetProcAddress(hModule, "NtQueryInformationThread");
		else
			f = NULL;
	}

	if (f)
	{
		struct THREAD_BASIC_INFORMATION
		{
			NTSTATUS ExitStatus;
			PVOID TebBaseAddress;
			struct CLIENT_ID
			{
				HANDLE UniqueProcess;
				HANDLE UniqueThread;
			} ClientId;
			KAFFINITY AffinityMask;
			DWORD Priority;
			DWORD BasePriority;
		} tbi;
		if (f(hThread, 0, &tbi, sizeof(THREAD_BASIC_INFORMATION), NULL) == 0)
			return (DWORD)tbi.ClientId.UniqueThread;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////


#define ENSURE_CALLING_THREAD(wnd)			TuoAssert(::GetCurrentThreadId() == ::GetWindowThreadProcessId(wnd, NULL))


typedef std::vector<std::wstring> StrVec;


#define STR_LEN_A(str)			(sizeof(str) - 1)
#define STR_LEN_W(str)			(_countof(str) - 1)


#define _TODO_STR(x)			#x
#define TODO_STR(x)				_TODO_STR(x)
#define TODO(author, desc)		message(__FILE__ "(" TODO_STR(__LINE__) ") : warning TODO [" author "]: " desc)

#pragma warning(error: 4003)	// 需要保证TODO的参数个数


//////////////////////////////////////////////////////////////////////////


#define TuoPerformanceProbe(key, ...) do { \
	class __tuostr { public: std::wstring _str; \
		__tuostr(const wchar_t *strW, ...) { \
			va_list arglist; \
			va_start(arglist, strW); \
			int len = _vscwprintf(strW, arglist); \
			wchar_t *p = new wchar_t[len + 1]; \
			vswprintf_s(p, len + 1, strW, arglist); \
			_str = p; \
			delete p; \
		} \
		__tuostr(const char *strA, ...) { \
			va_list arglist; \
			va_start(arglist, strA); \
			int len = _vscprintf(strA, arglist); \
			char *p = new char[len + 1]; \
			vsprintf_s(p, len + 1, strA, arglist); \
			wchar_t *wBuf = new wchar_t[len + 1]; \
			int iConvert = ::MultiByteToWideChar(65001, 0, p, len, wBuf, len + 1); \
			wBuf[iConvert] = 0; \
			_str = wBuf; \
			delete wBuf; \
			delete p; \
		} \
	}; \
	__tuostr tuostr(key, __VA_ARGS__); \
	DWORD dwTime = ::GetTickCount() % 60000; \
	se::wstringbuf sb; \
	sb << L"[Perf] [" << (dwTime / 1000) << L"," << (dwTime % 1000) << L"]" \
		<< L" [" << ::GetCurrentProcessId() << L"] " << tuostr._str \
		<< L" (" << __FUNCTIONW__ << L")\n"; \
	::OutputDebugStringW(sb.str()); } while (0)
