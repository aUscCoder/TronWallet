#include <windows.h>
#pragma warning(push)
#pragma warning(disable : 4091) // VS2015  warning C4091 : 'typedef ' : ignored on left of '' when no variable is declared(compiling source file MiniDumpGenerator.cpp)
#include <imagehlp.h>
#pragma warning(pop)
#include "Dump.h"

#pragma comment(lib, "psapi.lib")

void GenerateMiniDump(wchar_t* path, CrashDumpData *pCrashDumpData, bool bInSupportMode/* =false */)
{
	HANDLE hProcess(::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pCrashDumpData->dwPID));
	if (hProcess == NULL)
		return;

	EXCEPTION_POINTERS exp;
	DWORD dwBytesRead;
	if (::ReadProcessMemory(hProcess, pCrashDumpData->lpExceptionPointerRemote, &exp, sizeof(EXCEPTION_POINTERS), &dwBytesRead)
		&& dwBytesRead == sizeof(EXCEPTION_POINTERS))
	{
		CONTEXT *pContext = new CONTEXT;
		if (::ReadProcessMemory(hProcess, exp.ContextRecord, pContext, sizeof(CONTEXT), &dwBytesRead) && dwBytesRead == sizeof(CONTEXT))
		{
			// exception record至少要能读出来一个，后面的读不出来可以允许
			EXCEPTION_RECORD *pRecord = new EXCEPTION_RECORD;
			if (::ReadProcessMemory(hProcess, exp.ExceptionRecord, pRecord, sizeof(EXCEPTION_RECORD), &dwBytesRead) && dwBytesRead == sizeof(EXCEPTION_RECORD))
			{
				for (EXCEPTION_RECORD *pNext = pRecord; pNext->ExceptionRecord != NULL;)
				{
					EXCEPTION_RECORD *pNextRecord = new EXCEPTION_RECORD;
					if (!::ReadProcessMemory(hProcess, pNext->ExceptionRecord, pNextRecord, sizeof(EXCEPTION_RECORD), &dwBytesRead)
						|| dwBytesRead != sizeof(EXCEPTION_RECORD))
					{
						break;
					}
					pNext->ExceptionRecord = pNextRecord;
					pNext = pNextRecord;
				}
				pCrashDumpData->pExceptionPointerLocal = new EXCEPTION_POINTERS;
				pCrashDumpData->pExceptionPointerLocal->ExceptionRecord = pRecord;
				pCrashDumpData->pExceptionPointerLocal->ContextRecord = pContext;
			}
		}
	}

	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	eInfo.ThreadId = pCrashDumpData->dwTID;
	eInfo.ExceptionPointers = pCrashDumpData->pExceptionPointerLocal;
	eInfo.ClientPointers = FALSE;

	HANDLE hMiniDumpFile = ::CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hMiniDumpFile == INVALID_HANDLE_VALUE)
		return;
	// 检测DBGHelp版本是否足够调用MiniDumpWithFullMemoryInfo
	int miniDumpType = MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithProcessThreadData;
	//miniDumpType |= MiniDumpWithFullMemoryInfo;
	
	::MiniDumpWriteDump(hProcess, pCrashDumpData->dwPID, hMiniDumpFile, (MINIDUMP_TYPE)miniDumpType,
		pCrashDumpData->pExceptionPointerLocal ? &eInfo : NULL, NULL, NULL);

	::CloseHandle(hMiniDumpFile);
}
