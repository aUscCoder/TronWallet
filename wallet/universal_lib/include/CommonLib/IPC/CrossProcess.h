// Author: TuotuoXP
#pragma once
#include "DataSerializer.h"


// [TuotuoXP] 多参数跨进程的传递方法
// ****** 注意，如果是临时的wstring对象，要用_SD来搞定 ******
// {
//     CrossProcessDataCollector col;
//     std::wstring str(L"std::wstring object");
//     col << L"string pointer"
//         << str
//         << _SD(std::wstring(L"test wstring")
//         << 12345
//         << _D(pBinaryDataPointer, iBinaryDataLength);
//     col.PostMessage(hwnd, [message]);
// }
//
// 接收方法，在消息循环中写：
//
//    CROSS_PROCESS_HANDLER([message], OnTestCrossProcess)
//
// 函数实现：
//    void OnTestCrossProcess(CDataArgs &args)
//    {
//        LPCWSTR lpszArg0 = pArgs[0].GetStrArg();
//        LPCWSTR lpszArg1 = pArgs[1].GetStrArg();
//        int iArg2 = pArgs[2].GetIntArg();
//        BYTE *pArg3 = pArgs[3].GetBinaryArg();
//        int iArg3Len = pArgs[3].GetBinaryArgLength();
//    }


class CrossProcessDataCollector : public CDataCollector
{

public:

	CrossProcessDataCollector() : m_hTargetProcessHandle(NULL) {}
	~CrossProcessDataCollector()
	{
		if (m_hTargetProcessHandle)
			::CloseHandle(m_hTargetProcessHandle);
	}

	void PostMessage(HWND hwnd, UINT uMsg, LPARAM lParam = NULL)
	{
		HANDLE hMapping = Serialize(uMsg, hwnd);
		if (hMapping)
			if (!::PostMessage(hwnd, uMsg, (WPARAM)hMapping, lParam))
				CloseMappingHandleInTargetProcess(hMapping);
	}

	void SendNotifyMessage(HWND hwnd, UINT uMsg)
	{
		HANDLE hMapping = Serialize(uMsg, hwnd);
		if (hMapping)
			if (!::SendNotifyMessage(hwnd, uMsg, (WPARAM)hMapping, 0))
				CloseMappingHandleInTargetProcess(hMapping);
	}

	LRESULT SendMessage(HWND hwnd, UINT uMsg)
	{
		HANDLE hMapping = Serialize(uMsg, hwnd);
		if (hMapping)
		{
			LRESULT lRes = ::SendMessage(hwnd, uMsg, (WPARAM)hMapping, 0);
			TuoAssert(!CheckHandle(hMapping));
			return lRes;
		}
		return 0;
	}

	void SendMessageAndReturn(HWND hwnd, UINT uMsg, CDataArgs &argReturnValue)
	{
		HANDLE hMapping = Serialize(uMsg, hwnd);
		if (hMapping)
		{
			LRESULT lResult = ::SendMessage(hwnd, uMsg, (WPARAM)hMapping, 0);
			TuoAssert(!CheckHandle(hMapping));
			if (lResult)
				Universal::DataSerializer::UnserializeData((HANDLE)lResult, &argReturnValue);
		}
	}

private:

	void CloseMappingHandleInTargetProcess(HANDLE hMappingHandleInTargetProcess)
	{
		TuoAssert(0 && "出现了极端情况，发送消息失败，需要留意");
		CheckHandle(hMappingHandleInTargetProcess, true);
	}

	bool CheckHandle(HANDLE hMappingHandleInTargetProcess, bool bCloseHandle = false)
	{
		if ((UINT)hMappingHandleInTargetProcess & 0x80000000)		// a handle in backup process
		{
			TuoAssert(m_hTargetProcessHandle == NULL);
			hMappingHandleInTargetProcess = (HANDLE)((UINT)hMappingHandleInTargetProcess & 0x7fffffff);
			return !!::DuplicateHandle(Universal::DataSerializerInit::GetBackupProcess(), hMappingHandleInTargetProcess,
				NULL, NULL, 0, FALSE, DUPLICATE_SAME_ACCESS | (bCloseHandle ? DUPLICATE_CLOSE_SOURCE : 0));
		}
		else
		{
			TuoAssert(m_hTargetProcessHandle);
			return !!::DuplicateHandle(m_hTargetProcessHandle, hMappingHandleInTargetProcess,
				NULL, NULL, 0, FALSE, DUPLICATE_SAME_ACCESS | (bCloseHandle ? DUPLICATE_CLOSE_SOURCE : 0));
		}
	}

	HANDLE Serialize(UINT uMsg, HWND hWnd)
	{
		TuoAssert(m_hTargetProcessHandle == NULL);
		DWORD dwPID = 0;
		::GetWindowThreadProcessId(hWnd, &dwPID);
		if (dwPID == ::GetCurrentProcessId())
			m_hTargetProcessHandle = ::GetCurrentProcess();
		else
			m_hTargetProcessHandle = ::OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwPID);
		if (m_hTargetProcessHandle)
			return Universal::DataSerializer::SerializeDataToAnotherProcess(this, uMsg, m_hTargetProcessHandle);
		else if (::GetLastError() == ERROR_ACCESS_DENIED)		// in sandbox, 将mapping保存到主进程
			return Universal::DataSerializer::SerializeDataToBackupProcess(this, uMsg);
		return NULL;
	}

	HANDLE m_hTargetProcessHandle;
};


namespace Universal
{
	namespace CrossProcess
	{
		inline void PostCrossProcessString(HWND hwnd, UINT uMsg, LPCWSTR szContent)
		{
			CrossProcessDataCollector col;
			col << szContent;
			col.PostMessage(hwnd, uMsg);
		}

		inline void PostCrossProcessBinary(HWND hwnd, UINT uMsg, void *pContent, int iLen)
		{
			CrossProcessDataCollector col;
			col << _D(pContent, iLen);
			col.PostMessage(hwnd, uMsg);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


// LRESULT OnCrossProcess(CDataArgs &args);
#define CROSS_PROCESS_HANDLER(msg, func) \
	if (uMsg == msg) \
	{ \
		CDataArgs dataArgs; \
		Universal::DataSerializer::UnserializeData((HANDLE)wParam, &dataArgs); \
		TuoAssert(dataArgs.Valid()); \
		TuoAssert(dataArgs.GetMessage() == msg); \
		if (dataArgs.Valid()) \
			lResult = func(dataArgs); \
		else \
			lResult = 0; \
		return TRUE; \
	}

#define CROSS_PROCESS_HANDLER_WITH_MAPPING_HANDLE(msg, func) \
	if (uMsg == msg) \
	{ \
		CDataArgs dataArgs; \
		Universal::DataSerializer::UnserializeData((HANDLE)wParam, &dataArgs, true); \
		TuoAssert(dataArgs.Valid()); \
		TuoAssert(dataArgs.GetMessage() == msg); \
		if (dataArgs.Valid()) \
			lResult = func(dataArgs, (HANDLE)wParam); \
		else \
			lResult = 0; \
		CloseHandle((HANDLE)wParam); \
		return TRUE; \
	}

// void OnCrossProcessAndReturnData(CDataArgs &args, CDataCollector &colRet);
#define CROSS_PROCESS_AND_RETURN_HANDLER(msg, func) \
	if (uMsg == msg) \
	{ \
		CDataArgs dataArgs; \
		Universal::DataSerializer::UnserializeData((HANDLE)wParam, &dataArgs); \
		TuoAssert(dataArgs.Valid()); \
		TuoAssert(dataArgs.GetMessage() == msg); \
		lResult = 0; \
		if (dataArgs.Valid()) \
		{ \
			CDataCollector col; \
			func(dataArgs, col); \
			if (!col.Empty()) \
				lResult = (LRESULT)Universal::DataSerializer::SerializeDataToBackupProcess(&col, 0x80000000 | msg); \
		} \
		return TRUE; \
	}

// void OnCrossProcessStringHandler(LPCWSTR lpszString);
#define CROSS_PROCESS_STRING_HANDLER(msg, func) \
	if (uMsg == msg) \
	{ \
		CDataArgs dataArgs; \
		Universal::DataSerializer::UnserializeData((HANDLE)wParam, &dataArgs); \
		TuoAssert(dataArgs.Valid()); \
		TuoAssert(dataArgs.GetMessage() == msg); \
		TuoAssert(dataArgs.GetArgCount() == 1); \
		if (dataArgs.Valid()) \
			func(dataArgs[0].GetStrArg()); \
		lResult = 0; \
		return TRUE; \
	}

// void OnCrossProcessDataHandler(const void *pData, int iSize);
#define CROSS_PROCESS_DATA_HANDLER(msg, func) \
	if (uMsg == msg) \
	{ \
		CDataArgs dataArgs; \
		Universal::DataSerializer::UnserializeData((HANDLE)wParam, &dataArgs); \
		TuoAssert(dataArgs.Valid()); \
		TuoAssert(dataArgs.GetMessage() == msg); \
		TuoAssert(dataArgs.GetArgCount() == 1); \
		if (dataArgs.Valid()) \
			func(dataArgs[0].GetBinaryArg(), dataArgs[0].GetBinaryArgLength()); \
		lResult = 0; \
		return TRUE; \
	}
