#include "ExceptionHandler.h"
#include "Dump.h"


static BrokerDumpExceptionInfo g_pBrokerDumpExceptionInfo = NULL;

void Common::ExceptionHandler::DumpExceptionInfo(PEXCEPTION_POINTERS pExceptPtrs, bool bFromExceptionHandler)
{
	DWORD dwPID = ::GetCurrentProcessId();
	DWORD dwTID = ::GetCurrentThreadId();

  wchar_t exe[MAX_PATH] = { 0 };
  ::GetModuleFileNameW(NULL, exe, MAX_PATH);

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = { 0 };
	HANDLE hWaitingEvt = NULL;
	wchar_t sz[1024];
	wsprintfW(sz, L"\"%s\" -dump %d %d %d %d %d %d", exe, 0, 0, 0, dwPID, dwTID, pExceptPtrs);

	// 考虑到崩溃时，需要尽量少调用CRT的函数，所以我们在这里不做进程保护.
	if (::CreateProcess(NULL, sz, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		hWaitingEvt = MakeEvent(dwPID, dwTID);
#ifdef _DEBUG
		::WaitForSingleObject(hWaitingEvt, INFINITE);
#else
		HANDLE hEvts[] = { hWaitingEvt, pi.hProcess };
		::WaitForMultipleObjects(2, hEvts, FALSE, INFINITE);
#endif
	}

  ::CloseHandle(hWaitingEvt);
}


LONG CALLBACK SogouExceptionHandler(PEXCEPTION_POINTERS pExceptPtrs) // NOLINT (2.2.2 本函数需)
{
	Common::ExceptionHandler::DumpExceptionInfo(pExceptPtrs, true);

	::TerminateProcess(::GetCurrentProcess(), 100);
	return EXCEPTION_EXECUTE_HANDLER;
}


static void CRTExceptionHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
{
	throw 1;
}

void Common::ExceptionHandler::InstallExceptionHandler()
{
		::SetUnhandledExceptionFilter(SogouExceptionHandler);
		_set_invalid_parameter_handler(CRTExceptionHandler);
}


