// Author: TuotuoXP
#pragma once
#include <windows.h>

struct CrashDumpData
{
	LPCWSTR lpszExceptionName;				// %temp%目录下的文件名和发送请求的key
	LPCWSTR lpszVersion;					// 产品的版本号

	bool bByUnhandledExceptionFilter;		// = false表示是try-catch
	DWORD dwPID;
	DWORD dwTID;
  LPVOID lpExceptionPointerRemote;
	EXCEPTION_POINTERS *pExceptionPointerLocal;

	LPCWSTR lpszWndProcessNameList;

	// 如果需要用户输入一些信息，则需要创建一个event，等待strUserNote里面被填满内容
	HANDLE hWaitingForUserNote;		// == NULL 表示无需接收user note

	// 崩溃上传比例, 取值范围 [1, 100]
	int nReportPercent;
};

// 技术支持模式  HKEY_CURRENT_USER\\Software\\SogouExplorer下"SgDebug"的值为1
//				 如果HKCUR存在SgDebug的项值，则产生一次全Dump
void GenerateCrashDumpFiles(CrashDumpData *pDumpData, bool bInSupportMode);
void CrashDump(LPCWSTR lpszCmdLine);

inline HANDLE MakeEvent(DWORD dwPID, DWORD dwTID)
{
	wchar_t sz[64];
	wsprintfW(sz, L"___TronWallet___CRASH_EVT_%d_%d", dwPID, dwTID);
	return ::CreateEvent(NULL, FALSE, FALSE, sz);
}
