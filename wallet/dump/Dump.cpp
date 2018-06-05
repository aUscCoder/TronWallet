#include "Dump.h"
#include <windows.h>
#include <stdio.h>

void GenerateMiniDump(wchar_t* path, CrashDumpData *pCrashDumpData, bool bInSupportMode/* =false */);
void GenerateCrashDumpFiles(CrashDumpData *pDumpData, bool bInSupportMode)
{
  const DWORD dwTickCount = ::GetTickCount();
  const bool bReport = ((dwTickCount % 100) + 1) <= (DWORD)(pDumpData->nReportPercent);

  wchar_t szCrashDumpDir[MAX_PATH];
  ::GetTempPath(_countof(szCrashDumpDir), szCrashDumpDir);

  wchar_t path[MAX_PATH];
  swprintf_s(path, L"%s\\tronwallet_%d_%d.dmp", szCrashDumpDir, pDumpData->dwPID, pDumpData->dwTID);

  GenerateMiniDump(path, pDumpData, bInSupportMode);
}

void CrashDump(LPCWSTR lpszCmdLine)
{
  // crash dump 命令行参数
  // SogouExplorer.exe -dump [exception type] [是否显示crash提示窗口] [是否来自unhandled exception filter] [PID] [TID] [exception pointer] [额外文件1] [额外文件2] ... [额外文件n]
  int nArgs;
  LPWSTR *szArglist = ::CommandLineToArgvW(lpszCmdLine, &nArgs);
  if (szArglist == NULL || nArgs < 6)
    return;

  // 需要用户输入额外信息, 主进程
  const bool bShowNotifyWindow = _wtoi(szArglist[2]) != 0;

  CrashDumpData data;

  data.lpszExceptionName = L"tronwallet";
  data.lpszVersion = L"1.0.0.0";

  data.bByUnhandledExceptionFilter = _wtoi(szArglist[3]) != 0;
  data.dwPID = _wtoi(szArglist[4]);
  data.dwTID = _wtoi(szArglist[5]);
  data.lpExceptionPointerRemote = (LPVOID)_wtoi(szArglist[6]);

  data.pExceptionPointerLocal = NULL;
  data.lpszWndProcessNameList = L"tronwallet.exe";

  // crash catch percent
  data.nReportPercent = 100;

  data.hWaitingForUserNote = NULL;
  GenerateCrashDumpFiles(&data, false);
}