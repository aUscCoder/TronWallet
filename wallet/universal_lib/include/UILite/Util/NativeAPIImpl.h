// Author: KeJinjin
// 修改此文件后，请告知以下使用方
//    1、输入法，用到了 version 0、1 API
//    2、皮肤编辑器模块，用到了 version 2 API
#pragma once


#define VERIFY_CODE		0xf2964893

// 以下请不要随意改数值不要随意换顺序
enum UILiteNativeAPICode
{
	UILITE_NAPI_VERIFY_API = 0,				// version = 0
	UILITE_NAPI_GET_FOCUSED_CLASSNAME,		// version = 0
	UILITE_NAPI_GET_FOCUSED_TEXT,			// version = 0
	UILITE_NAPI_GET_VERSION,				// version = 1
	UILITE_NAPI_GET_FOCUSED_TYPE_STRING,	// version = 1
	UILITE_NAPI_GET_FOCUSED_EDIT_SEL,		// version = 1
	UILITE_NAPI_GET_FOCUSED_EDIT_SEL_TEXT,	// version = 1
	UILITE_NAPI_GET_OBJECT_FROM_POINT,		// version = 2
	UILITE_NAPI_GET_OBJECT_PARENT,			// version = 2
	UILITE_NAPI_RELEASE_OBJECT_ID,			// version = 2
	UILITE_NAPI_GET_OBJECT_TYPE,			// version = 2
	UILITE_NAPI_HIGHLIGHT_OBJECT,			// version = 2

	UILITE_NAPI_END
};

_declspec(selectany) DWORD WM_UILITE_NATIVE_API = ::RegisterWindowMessageW(L"_uilite_native_api_");
std::wstring __SendAndGetString(HWND hWnd, DWORD dwAPI, DWORD dwMaxLength);
bool __SendAndGetIntArray(HWND hWnd, DWORD dwAPI, int* pOut, DWORD dwCount);
bool __SendIntArray(HWND hWnd, DWORD dwAPI, const int *pIn, DWORD dwInCount);
bool __SendIntArrayAndGetIntArray(HWND hWnd, DWORD dwAPI, const int *pIn, DWORD dwInCount, int *pOut, DWORD dwOutCount);

namespace UILiteAPI
{
	inline bool IsSupportNativeAPI(HWND hWnd)
	{
		return ::GetProp(hWnd, L"_is_uilite_") == (HANDLE)VERIFY_CODE
			&& ::SendMessage(hWnd, WM_UILITE_NATIVE_API, UILITE_NAPI_VERIFY_API, NULL) == VERIFY_CODE;
	}

	inline int GetVersion(HWND hWnd)
	{
		if (::GetProp(hWnd, L"_is_uilite_") == (HANDLE)VERIFY_CODE)
			return (int)::SendMessage(hWnd, WM_UILITE_NATIVE_API, UILITE_NAPI_GET_VERSION, NULL);
		else
			return 0;
	}

	inline std::wstring GetFocusedObjectClassName(HWND hWnd)
	{
		// 我给一个1k的buffer， classname不可能超过511个字符。
		return __SendAndGetString(hWnd, UILITE_NAPI_GET_FOCUSED_CLASSNAME, 512);
	}

	inline std::wstring GetFocusedObjectType(HWND hWnd)
	{
		// 我给一个1k的buffer， type不可能超过511个字符。
		return __SendAndGetString(hWnd, UILITE_NAPI_GET_FOCUSED_TYPE_STRING, 512);
	}

	inline std::wstring GetFocusedObjectText(HWND hWnd)
	{
		// 超过1M长度文本去死吧。
		return __SendAndGetString(hWnd, UILITE_NAPI_GET_FOCUSED_TEXT, 1024 * 1024);
	}

	inline bool GetFocusedEditSelRange(HWND hWnd, int* pStart, int* pEnd)
	{
		int out[2] = { 0 };
		if (__SendAndGetIntArray(hWnd, UILITE_NAPI_GET_FOCUSED_EDIT_SEL, out, _countof(out)))
		{
			*pStart = out[0];
			*pEnd = out[1];
			return true;
		}

		return false;
	}

	inline std::wstring GetFocusedEditSelText(HWND hWnd)
	{
		// 超过1M长度文本去死吧。
		return __SendAndGetString(hWnd, UILITE_NAPI_GET_FOCUSED_EDIT_SEL_TEXT, 1024 * 1024);
	}

	inline int GetObjectFromPoint(HWND hWnd, POINT ptScreen)	// 返回值是 ObjectId，不用时需要调用 ReleaseObjectID 释放
	{
		int iObjectId = 0;
		__SendIntArrayAndGetIntArray(hWnd, UILITE_NAPI_GET_OBJECT_FROM_POINT, (const int *)&ptScreen, 2, &iObjectId, 1);
		return iObjectId;
	}

	inline int GetObjectParent(HWND hWnd, int iObjectId)	// 返回值是 ObjectId，不用时需要调用 ReleaseObjectID 释放
	{
		int iParentId = 0;
		__SendIntArrayAndGetIntArray(hWnd, UILITE_NAPI_GET_OBJECT_PARENT, &iObjectId, 1, &iParentId, 1);
		return iParentId;
	}

	inline void ReleaseObjectID(HWND hWnd, int iObjectId)
	{
		__SendIntArray(hWnd, UILITE_NAPI_RELEASE_OBJECT_ID, &iObjectId, 1);
	}

	inline int GetObjectType(HWND hWnd, int iObjectId)
	{
		int iObjectType = 0;
		__SendIntArrayAndGetIntArray(hWnd, UILITE_NAPI_GET_OBJECT_TYPE, &iObjectId, 1, &iObjectType, 1);
		return iObjectType;
	}

	inline void HighlightObject(HWND hWnd, int iObjectId, bool bHighlight)
	{
		int iParams[] = { iObjectId, bHighlight ? 1 : 0 };
		__SendIntArray(hWnd, UILITE_NAPI_HIGHLIGHT_OBJECT, iParams, 2);
	}
}

//////////////////////////////////////////////////////////////////////////
inline bool __CreateFileMapping(DWORD dwSize, HWND hWnd, HANDLE* pOutThis, HANDLE* pOutTarget);
inline bool __WriteIntArrayToFileMapping(HANDLE hFileMapping, const int *p, DWORD dwCount);
inline bool __ReadIntArrayFromFileMapping(HANDLE hFileMapping, int *p, DWORD dwCount);
inline bool __ReadStringFromFileMapping(HANDLE hFileMapping, std::wstring &s, DWORD dwMaxLength);

inline std::wstring __SendAndGetString(HWND hWnd, DWORD dwAPI, DWORD dwMaxLength)
{
	if (::GetProp(hWnd, L"_is_uilite_") != (HANDLE)VERIFY_CODE)
		return L"";

	HANDLE hFileMapping = NULL;
	HANDLE hTarget = NULL;

	std::wstring strOut;
	int iBufferSize = (dwMaxLength + 1) * sizeof(wchar_t);
	if (__CreateFileMapping(iBufferSize, hWnd, &hFileMapping, &hTarget))
	{
		if (::SendMessage(hWnd, WM_UILITE_NATIVE_API, dwAPI, (LPARAM)hTarget) == VERIFY_CODE)
			__ReadStringFromFileMapping(hFileMapping, strOut, dwMaxLength);

		::CloseHandle(hFileMapping);
	}

	return strOut;
}

inline std::wstring __SendIntArrayAndGetString(HWND hWnd, DWORD dwAPI, const int *pIn, DWORD dwInCount, DWORD dwMaxLength)
{
	if (::GetProp(hWnd, L"_is_uilite_") != (HANDLE)VERIFY_CODE)
		return L"";

	HANDLE hFileMapping = NULL;
	HANDLE hTarget = NULL;

	std::wstring strOut;
	int iBufferSize = std::max<int>((dwMaxLength + 1) * sizeof(wchar_t), std::min<int>(dwInCount, 1024));
	if (__CreateFileMapping(iBufferSize, hWnd, &hFileMapping, &hTarget))
	{
		__WriteIntArrayToFileMapping(hFileMapping, pIn, dwInCount);

		if (::SendMessage(hWnd, WM_UILITE_NATIVE_API, dwAPI, (LPARAM)hTarget) == VERIFY_CODE)
			__ReadStringFromFileMapping(hFileMapping, strOut, dwMaxLength);

		::CloseHandle(hFileMapping);
	}

	return strOut;
}

inline bool __SendAndGetIntArray(HWND hWnd, DWORD dwAPI, int *pOut, DWORD dwCount)
{
	if (::GetProp(hWnd, L"_is_uilite_") != (HANDLE)VERIFY_CODE)
		return false;

	HANDLE hFileMapping = NULL;
	HANDLE hTarget = NULL;

	bool bRet = false;
	int iBufferSize = std::min<int>(dwCount, 1024);
	if (__CreateFileMapping(iBufferSize, hWnd, &hFileMapping, &hTarget))
	{
		if (::SendMessage(hWnd, WM_UILITE_NATIVE_API, dwAPI, (LPARAM)hTarget) == VERIFY_CODE)
		{
			bRet = true;
			__ReadIntArrayFromFileMapping(hFileMapping, pOut, dwCount);
		}

		::CloseHandle(hFileMapping);
	}

	return bRet;
}

inline bool __SendIntArray(HWND hWnd, DWORD dwAPI, const int *pIn, DWORD dwInCount)
{
	if (::GetProp(hWnd, L"_is_uilite_") != (HANDLE)VERIFY_CODE)
		return false;

	HANDLE hFileMapping = NULL;
	HANDLE hTarget = NULL;

	bool bRet = false;
	int iBufferSize = std::min<int>(dwInCount, 1024);
	if (__CreateFileMapping(iBufferSize, hWnd, &hFileMapping, &hTarget))
	{
		__WriteIntArrayToFileMapping(hFileMapping, pIn, dwInCount);

		if (::SendMessage(hWnd, WM_UILITE_NATIVE_API, dwAPI, (LPARAM)hTarget) == VERIFY_CODE)
			bRet = true;

		::CloseHandle(hFileMapping);
	}

	return bRet;
}

inline bool __SendIntArrayAndGetIntArray(HWND hWnd, DWORD dwAPI, const int *pIn, DWORD dwInCount, int *pOut, DWORD dwOutCount)
{
	if (::GetProp(hWnd, L"_is_uilite_") != (HANDLE)VERIFY_CODE)
		return false;

	HANDLE hFileMapping = NULL;
	HANDLE hTarget = NULL;

	bool bRet = false;
	int iBufferSize = std::min(std::max<int>(dwInCount, dwOutCount), 1024);
	if (__CreateFileMapping(iBufferSize, hWnd, &hFileMapping, &hTarget))
	{
		__WriteIntArrayToFileMapping(hFileMapping, pIn, dwInCount);

		if (::SendMessage(hWnd, WM_UILITE_NATIVE_API, dwAPI, (LPARAM)hTarget) == VERIFY_CODE)
		{
			bRet = true;
			__ReadIntArrayFromFileMapping(hFileMapping, pOut, dwOutCount);
		}

		::CloseHandle(hFileMapping);
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////

inline bool __CreateFileMapping(DWORD dwSize, HWND hWnd, HANDLE* pOutThis, HANDLE* pOutTarget)
{
	*pOutThis = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, dwSize, NULL);
	if (*pOutThis)
	{
		DWORD dwProcessID = 0;
		::GetWindowThreadProcessId(hWnd, &dwProcessID);

		HANDLE hWndProcess = ::OpenProcess(PROCESS_DUP_HANDLE, FALSE, dwProcessID);
		if (hWndProcess)
		{
			BOOL bSucceed = ::DuplicateHandle(::GetCurrentProcess(), *pOutThis, hWndProcess, pOutTarget, 0, FALSE, DUPLICATE_SAME_ACCESS);
			::CloseHandle(hWndProcess);
			hWndProcess = NULL;

			if (bSucceed && *pOutTarget)
				return true;
		}

		::CloseHandle(*pOutThis);
		*pOutThis = NULL;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

inline bool __WriteToFileMapping(HANDLE hFileMapping, LPCBYTE lpBuffer, DWORD cbSize)
{
	if (hFileMapping == NULL || hFileMapping == INVALID_HANDLE_VALUE)
		return false;

	LPBYTE lpFileMappingBuffer = (LPBYTE)::MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, cbSize);
	if (lpFileMappingBuffer == NULL)
		return false;

	memcpy(lpFileMappingBuffer, lpBuffer, cbSize);
	::UnmapViewOfFile(lpFileMappingBuffer);
	
	return true;
}

inline bool __WriteIntArrayToFileMapping(HANDLE hFileMapping, const int *p, DWORD dwCount)
{
	return __WriteToFileMapping(hFileMapping, (LPBYTE)p, sizeof(int) * dwCount);
}

inline bool __WriteStringToFileMapping(HANDLE hFileMapping, const std::wstring &s)
{
	return __WriteToFileMapping(hFileMapping, (LPCBYTE)s.c_str(), (s.length() + 1) * sizeof(wchar_t));
}

//////////////////////////////////////////////////////////////////////////

inline bool __ReadFromFileMapping(HANDLE hFileMapping, LPBYTE lpBuffer, DWORD cbSize)
{
	if (hFileMapping == NULL || hFileMapping == INVALID_HANDLE_VALUE)
		return false;

	LPBYTE lpFileMappingBuffer = (LPBYTE)::MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, cbSize);
	if (lpFileMappingBuffer == NULL)
		return false;

	memcpy(lpBuffer, lpFileMappingBuffer, cbSize);
	::UnmapViewOfFile(lpFileMappingBuffer);

	return true;
}

inline bool __ReadIntFromFileMapping(HANDLE hFileMapping, int &i)
{
	return __ReadFromFileMapping(hFileMapping, (LPBYTE)&i, sizeof(int));
}

inline bool __ReadIntArrayFromFileMapping(HANDLE hFileMapping, int *p, DWORD dwCount)
{
	return __ReadFromFileMapping(hFileMapping, (LPBYTE)p, sizeof(int) * dwCount);
}

inline bool __ReadStringFromFileMapping(HANDLE hFileMapping, std::wstring &s, DWORD dwMaxLength)
{
	if (hFileMapping == NULL || hFileMapping == INVALID_HANDLE_VALUE)
		return false;

	LPBYTE lpFileMappingBuffer = (LPBYTE)::MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, (dwMaxLength + 1) * sizeof(wchar_t));
	if (lpFileMappingBuffer == NULL)
		return false;

	s.append((LPCWSTR)lpFileMappingBuffer, dwMaxLength);
	::UnmapViewOfFile(lpFileMappingBuffer);

	size_t iLen = wcslen(s.c_str());
	if (iLen != s.length())
		s.erase(iLen, s.length() - iLen);

	return true;
}
