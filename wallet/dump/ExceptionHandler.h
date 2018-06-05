#pragma once
#include <windows.h>

typedef void (*BrokerDumpExceptionInfo)(wchar_t* sz, DWORD dwPID, DWORD dwTID, HANDLE* targetDumpProcess, HANDLE* phWaitingEvt);

namespace Common
{
	namespace ExceptionHandler
	{
		void InstallExceptionHandler();
		void DumpExceptionInfo(PEXCEPTION_POINTERS pExceptPtrs, bool bFromExceptionHandler);
	}
}
