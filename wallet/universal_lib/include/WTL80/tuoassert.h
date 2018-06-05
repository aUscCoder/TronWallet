// Author: TuotuoXP
#pragma once
#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
//
// TuoAssert 是存在一些问题的，全局变量初始化时一定要慎用！
// yangqi，2015/12/22
//
// 如果在模块加载过程中 assert 失败，比如初始化全局变量时，会导致整个进程卡死
//
// 原因：
// TuoAssert 失败时尝试创建线程，然后等待该线程结束
// 但是，该线程在 Loadlibrary 完成之前是不会被执行的
//
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682453(v=vs.85).aspx
// During process startup and DLL initialization routines, new threads can be created,
// but they do not begin execution until DLL initialization is done for the process
//
//////////////////////////////////////////////////////////////////////////

#ifndef TuoAssert

#ifdef _DEBUG

class CTuoAssertInternal
{

public:

	static int __TuoCrtDbgReportW(const wchar_t *lpszFile, int nLine, const wchar_t *lpszMsg, const wchar_t *lpszAddtional)
	{
		struct TuoDbgData
		{
			LPCWSTR lpszMsg;
			LPCWSTR lpszFileName;
			LPCWSTR lpszAdditional;
			int iLine;
			int iRetValue;
			HANDLE hEvent;

			static DWORD WINAPI Proc(LPVOID lParam)
			{
				TuoDbgData *pDbgData = (TuoDbgData*)lParam;
				pDbgData->iRetValue = TuoAssertDbgReport(pDbgData->lpszFileName, pDbgData->iLine, pDbgData->lpszMsg, pDbgData->lpszAdditional);
				::SetEvent(pDbgData->hEvent);
				return 0;
			}
		};

		// 这里就没必要创建线程了，yangqi，2015/1222
		if (AssertDisabled(lpszFile, nLine, lpszMsg, lpszAddtional))
			return 0;

		// show assert dialog in a new thread
		TuoDbgData dbgData = { lpszMsg, lpszFile, lpszAddtional, nLine };
		dbgData.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		HANDLE hThread = ::CreateThread(NULL, 0, TuoDbgData::Proc, &dbgData, 0, NULL);
		::WaitForSingleObject(dbgData.hEvent, INFINITE);
		::CloseHandle(hThread);
		::CloseHandle(dbgData.hEvent);
		return dbgData.iRetValue;
	}

private:

	enum AssertType
	{
		AssertType_ShowDialog,
		AssertType_OutputDebugString,
	};

	static wchar_t* AllocBuffer(LPCWSTR lpszFile, int nLine, LPCWSTR lpszMsg, LPCWSTR lpszAdditionalMsg, AssertType eType)
	{
		int iLenFile = wcslen(lpszFile);
		int iLen0 = wcslen(lpszMsg);
		int iLen1 = lpszAdditionalMsg ? wcslen(lpszAdditionalMsg) : 0;
		int iCount = 128 + iLenFile + iLen0 + iLen1;
		wchar_t *pBuf = new wchar_t[iCount];

		if (eType == AssertType_OutputDebugString)
		{
			// 输出中加个 warning: 是为了好在 VS output 中分辨出来，如果有 VSCommands 配合，效果会更好，yangqi，2015/12/22
			if (lpszAdditionalMsg)
				swprintf_s(pBuf, iCount, L"%s(%d): warning: [TuoAssert] %s, Additional: [%s]\r\n", lpszFile, nLine, lpszMsg, lpszAdditionalMsg);
			else
				swprintf_s(pBuf, iCount, L"%s(%d): warning: [TuoAssert] %s\r\n", lpszFile, nLine, lpszMsg);
		}
		else // AssertType_ShowDialog
		{
			if (lpszAdditionalMsg)
			{
				swprintf_s(pBuf, iCount, L"Debug Assertion Failed!\r\n\r\nFile: %s\r\nLine: %d\r\n\r\nExpression: %s\r\n\r\nAddtional: %s",
					lpszFile, nLine, lpszMsg, lpszAdditionalMsg);
			}
			else
			{
				swprintf_s(pBuf, iCount, L"Debug Assertion Failed!\r\n\r\nFile: %s\r\nLine: %d\r\n\r\nExpression: %s",
					lpszFile, nLine, lpszMsg);
			}
		}
		return pBuf;
	}

	static void ReleaseBuffer(wchar_t*& pBuf)
	{
		delete[] pBuf;
		pBuf = nullptr;
	}

	//
	// 检查是否禁掉了 assert，无论是否禁掉，都会有 OutputDebugStringW 输出
	// 一旦发生上面的卡死时至少有个地方看到信息
	// 这个函数名起的不太好
	// yangqi，2015/12/22
	//
	static bool AssertDisabled(LPCWSTR lpszFile, int nLine, LPCWSTR lpszMsg, LPCWSTR lpszAdditionalMsg)
	{
		// --disable_assert 命令行可以关掉 assert, 主要是跑 UT 时使用, yangqi, 20160510
		static LPWSTR lpszCmdLine = nullptr;
		static bool bDisableAssert = false;

		if (!lpszCmdLine)
		{
			lpszCmdLine = GetCommandLine();
			bDisableAssert = lpszCmdLine && wcsstr(lpszCmdLine, L"--disable_assert");
		}
		if (bDisableAssert)
		{
			return true;
		}

		// 通过注册表判断
		DWORD dwValue = 0;
		HKEY hKey = NULL;
		::RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\SogouExplorer", 0, KEY_QUERY_VALUE, &hKey);
		if (hKey)
		{
			DWORD dwType = 0;
			DWORD cbSize = sizeof(dwValue);
			::RegQueryValueExW(hKey, L"DisableAssert", 0, &dwType, (BYTE*)&dwValue, &cbSize);
			::RegCloseKey(hKey);
		}

		wchar_t *pBuf = AllocBuffer(lpszFile, nLine, lpszMsg, lpszAdditionalMsg, AssertType_OutputDebugString);
		::OutputDebugStringW(pBuf);
		ReleaseBuffer(pBuf);

		return dwValue == 1;
	}

	static int TuoAssertDbgReport(LPCWSTR lpszFile, int nLine, LPCWSTR lpszMsg, LPCWSTR lpszAdditionalMsg)		// NOLINT (1.3.2 全局调试函数，仅debug下使用)
	{
		wchar_t *pBuf = AllocBuffer(lpszFile, nLine, lpszMsg, lpszAdditionalMsg, AssertType_ShowDialog);
		const int iRet = ::MessageBoxW(NULL, pBuf, L"TuoAssert", MB_ICONERROR | MB_ABORTRETRYIGNORE);
		ReleaseBuffer(pBuf);

		if (iRet == IDABORT)
		{
			::TerminateProcess(::GetCurrentProcess(), 255);
		}
		else if (iRet == IDRETRY)
		{
			if (!::IsDebuggerPresent())
			{
				HMODULE hModule = NULL;
				::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)TuoAssertDbgReport, &hModule);

				UINT uMsg = ::RegisterWindowMessage(L"WM_SE_GET_ATTACH_WEIGHT");

				int iMaxWeight = -1;
				HWND hMaxWeightWnd = NULL;
				for (HWND hWnd = NULL;;)
				{
					hWnd = ::FindWindowExW(NULL, hWnd, L"SePlusAttachComm", NULL);
					if (hWnd == NULL)
						break;
					int iWeight = ::SendMessage(hWnd, uMsg, ::GetCurrentProcessId(), (LPARAM)hModule);
					if (iWeight > iMaxWeight)
					{
						iMaxWeight = iWeight;
						hMaxWeightWnd = hWnd;
					}
				}

				if (hMaxWeightWnd)
				{
					UINT uMsgAttach = ::RegisterWindowMessage(L"WM_SE_ATTACH_PROCESS");
					::SendMessage(hMaxWeightWnd, uMsgAttach, ::GetCurrentProcessId(), 0);
				}
			}

			return 1;
		}
		return 0;
	}
};


#define _TuoAsserte(expr, msg, additional) \
	(void) ((!!(expr)) || (1 != CTuoAssertInternal::__TuoCrtDbgReportW(_CRT_WIDE(__FILE__), __LINE__, msg, additional)) || (DebugBreak(), 0))

#define TuoAssert(expr)			_TuoAsserte((expr), _CRT_WIDE(#expr), NULL)
#define TuoAssert2(expr, msg)	_TuoAsserte((expr), _CRT_WIDE(#expr), msg)

#else

#define TuoAssert(expr)				do {} while (0)
#define TuoAssert2(expr, msg)		do {} while (0)

#endif	// _DEBUG

// The COMPILE_ASSERT macro can be used to verify that a compile time
// expression is true. For example, you could use it to verify the
// size of a static array:
//
//   COMPILE_ASSERT(ARRAYSIZE_UNSAFE(content_type_names) == CONTENT_NUM_TYPES,
//                  content_type_names_incorrect_size);
//
// or to make sure a struct is smaller than a certain size:
//
//   COMPILE_ASSERT(sizeof(foo) < 128, foo_too_large);
//
// The second argument to the macro is the name of the variable. If
// the expression is false, most compilers will issue a warning/error
// containing the name of the variable.

template <bool>
struct CompileAssert {
};

#define COMPILE_ASSERT(expr, msg) \
	typedef CompileAssert<(bool(expr))> msg[bool(expr) ? 1 : -1]

#endif	// TuoAssert
