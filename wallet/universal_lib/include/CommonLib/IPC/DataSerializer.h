// Author: TuotuoXP
#pragma once
#include "../../Universal.h"

#if defined(_WIN32) && defined(GetMessage)
// Allow GetMessage to be used as a valid method name in classes.
// windows.h defines GetMessage() as a macro.  Let's re-define it as an inline
// function.  The inline function should be equivalent for C++ users.
inline BOOL GetMessage_Win32(LPMSG lpMsg, HWND hWnd,
							 UINT wMsgFilterMin, UINT wMsgFilterMax) {
	return GetMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
}
#undef GetMessage
inline BOOL GetMessage(LPMSG lpMsg, HWND hWnd,
					   UINT wMsgFilterMin, UINT wMsgFilterMax) {
	return GetMessage_Win32(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
}
#endif

class CDataCollector;
class CDataArgs;

// [TuotuoXP] for check mapping leak
typedef void (*DataSerializerDebugCallback)(UINT uMappingID);

namespace Universal
{
	namespace DataSerializer
	{
		UNIVERSAL_LIB_API HANDLE SerializeData(const CDataCollector *pCollector, UINT uMsg);
		UNIVERSAL_LIB_API HANDLE SerializeDataToBackupProcess(const CDataCollector *pCollector, UINT uMsg);
		UNIVERSAL_LIB_API HANDLE SerializeDataToAnotherProcess(const CDataCollector *pCollector, UINT uMsg, HANDLE hTargetProcess);

		UNIVERSAL_LIB_API void UnserializeData(HANDLE hMapping, CDataArgs *pDataArgs, bool bDoNotReleaseMapping = false);

		UNIVERSAL_LIB_API void ReleaseDataArgs(CDataArgs *pDataArgs);


		UNIVERSAL_LIB_API WPARAM MoveMappingToAnotherProcess(WPARAM wParam, HANDLE hNewProcess, bool bCloseSrc = true);
		UNIVERSAL_LIB_API WPARAM MoveMappingToAnotherProcess(WPARAM wParam, HWND hNewWnd, bool bCloseSrc = true);
	}

	namespace DataSerializerInit
	{
		UNIVERSAL_LIB_API void SetBackupProcess(HANDLE hBackupProcess);
		UNIVERSAL_LIB_API void SetBackupProcessForce(HANDLE hBackupProcess);
		UNIVERSAL_LIB_API HANDLE GetBackupProcess();
	}

	namespace DataSerializerDebug
	{
		UNIVERSAL_LIB_API void RegisterDebugCallback(DataSerializerDebugCallback dbg);
	}
}

//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define TEST_INIT() \
	do \
	{ \
		m_pTestData = NULL; \
		if (m_eArgType == ArgType_String || m_eArgType == ArgType_StringA || m_eArgType == ArgType_Binary) \
		{ \
			m_pTestData = new char[m_iLen]; \
			memcpy(m_pTestData, m_pData, m_iLen); \
		} \
		else \
			m_pTestData = NULL; \
	} while (0)

#define TEST_VERIFY() \
	do \
	{ \
		if (m_pTestData != NULL && (m_eArgType == ArgType_String || m_eArgType == ArgType_StringA || m_eArgType == ArgType_Binary)) \
		{ \
			TuoAssert(memcmp(m_pTestData, m_pData, m_iLen) == 0 && "如果出现这个assert，请使用_SD(str)"); \
		} \
	} while (0)

#else
#define TEST_INIT()
#define TEST_VERIFY()
#endif


// BEGIN_NOLINT (3.1.3 基础库函数)
// BEGIN_NOLINT (4.5.39 基础库函数)

class _D
{

public:

	_D(LPCWSTR lpszString) : m_eArgType(ArgType_String), m_pData(lpszString), m_bNeedDelete(false)
	{
		TuoAssert(lpszString);
		m_iLen = (wcslen(lpszString) + 1) * sizeof(wchar_t);
		TEST_INIT();
	}
	_D(const std::wstring &str) : m_eArgType(ArgType_String), m_bNeedDelete(false)
	{
		m_pData = str.c_str();
		m_iLen = (str.length() + 1) * sizeof(wchar_t);
		TEST_INIT();
	}
	_D(const se::wstringbuf &str) : m_eArgType(ArgType_String), m_bNeedDelete(false)
	{
		m_pData = str.str();
		m_iLen = (str.length() + 1) * sizeof(wchar_t);
		TEST_INIT();
	}
	
	_D(LPCSTR lpszString) : m_eArgType(ArgType_StringA), m_pData(lpszString), m_bNeedDelete(false)
	{
		TuoAssert(lpszString);
		m_iLen = strlen(lpszString) + 1;
		TEST_INIT();
	}
	_D(const std::string &str) : m_eArgType(ArgType_StringA), m_bNeedDelete(false)
	{
		m_pData = str.c_str();
		m_iLen = str.length() + 1;
		TEST_INIT();
	}
	_D(const se::stringbuf &str) : m_eArgType(ArgType_StringA), m_bNeedDelete(false)
	{
		m_pData = str.str();
		m_iLen = str.length() + 1;
		TEST_INIT();
	}
	_D(const void *pData, int iLen) : m_eArgType(ArgType_Binary), m_pData(pData), m_iLen(iLen), m_bNeedDelete(false)
	{
		TuoAssert(pData);
		TEST_INIT();
	}

	_D(int iIntVal) : m_eArgType(ArgType_Int), m_pData(NULL), m_iLen(iIntVal), m_bNeedDelete(false) { TEST_INIT(); }
	_D(long iIntVal) : m_eArgType(ArgType_Int), m_pData(NULL), m_iLen(iIntVal), m_bNeedDelete(false) { TEST_INIT(); }
	_D(unsigned int iIntVal) : m_eArgType(ArgType_UInt), m_pData(NULL), m_iLen(iIntVal), m_bNeedDelete(false) { TEST_INIT(); }
	_D(unsigned long iIntVal) : m_eArgType(ArgType_UInt), m_pData(NULL), m_iLen(iIntVal), m_bNeedDelete(false) { TEST_INIT(); }
	_D(HANDLE hVal) : m_eArgType(ArgType_UInt), m_pData(NULL), m_iLen((int)hVal), m_bNeedDelete(false) { TEST_INIT(); }
	_D(HWND hWndVal) : m_eArgType(ArgType_UInt), m_pData(NULL), m_iLen((int)hWndVal), m_bNeedDelete(false) { TEST_INIT(); }
	_D(__int64 iIntVal) : m_eArgType(ArgType_Int64), m_pData((void*)(iIntVal & 0xffffffff)),
		m_iLen((int)(iIntVal >> 32)), m_bNeedDelete(false) { TEST_INIT(); }
	_D(unsigned __int64 iIntVal) : m_eArgType(ArgType_UInt64), m_pData((void*)(iIntVal & 0xffffffff)),
		m_iLen((int)(iIntVal >> 32)), m_bNeedDelete(false) { TEST_INIT(); }
	~_D() {}

	void Free()
	{
		if (m_bNeedDelete)
			free((void*)m_pData);
#ifdef _DEBUG
		if (m_pTestData)
		{
			delete m_pTestData;
			m_pTestData = NULL;
		}
#endif
	}


	LPCWSTR GetStrArg() const
	{
		TuoAssert(m_eArgType == ArgType_String);
		TuoAssert(m_pData);
		TEST_VERIFY();
		return (LPCWSTR)m_pData;
	}
	int GetStrLength() const
	{
		TuoAssert(m_eArgType == ArgType_String);
		TuoAssert(m_pData);
		TEST_VERIFY();
		return m_iLen;
	}
	LPCSTR GetStrAArg() const
	{
		TuoAssert(m_eArgType == ArgType_StringA);
		TuoAssert(m_pData);
		TEST_VERIFY();
		return (LPCSTR)m_pData;
	}
	int GetStrALength() const
	{
		TuoAssert(m_eArgType == ArgType_StringA);
		TuoAssert(m_pData);
		TEST_VERIFY();
		return m_iLen;
	}

	const void* GetBinaryArg() const
	{
		TuoAssert(m_eArgType == ArgType_Binary);
		TuoAssert(m_pData);
		TEST_VERIFY();
		return m_pData;
	}
	int GetBinaryArgLength() const
	{
		TuoAssert(m_eArgType == ArgType_Binary);
		TuoAssert(m_pData);
		TEST_VERIFY();
		return m_iLen;
	}

	int GetIntArg() const
	{
		TuoAssert(m_eArgType == ArgType_Int || m_eArgType == ArgType_UInt);
		TuoAssert(m_pData == NULL);
		return m_iLen;
	}

	__int64 GetInt64Arg() const
	{
		TuoAssert(m_eArgType == ArgType_Int64 || m_eArgType == ArgType_UInt64);
		return ((__int64)m_iLen << 32) | ((__int64)(unsigned int)m_pData);
	}

public:

	enum ArgType
	{
		ArgType_String = 100,
		ArgType_StringA,
		ArgType_Binary,
		ArgType_Int,
		ArgType_UInt,
		ArgType_Int64,
		ArgType_UInt64,
	};

	ArgType GetArgType() const { return m_eArgType; }

protected:

	ArgType m_eArgType;

	const void *m_pData;
	int m_iLen;
	bool m_bNeedDelete;

#ifdef _DEBUG
	void *m_pTestData;
#endif


	friend class CDataCollectorInternal;
	friend class CDataArgsInternal;
};


class _SD : public _D
{

public:

	_SD(LPCWSTR lpszStr) : _D(lpszStr) { Copy(); }
	_SD(const std::wstring &str) : _D(str) { Copy(); }
	_SD(const se::wstringbuf &str) : _D(str) { Copy(); }
	_SD(LPCSTR lpszStr) : _D(lpszStr) { Copy(); }
	_SD(const std::string &str) : _D(str) { Copy(); }
	_SD(const se::stringbuf &str) : _D(str) { Copy(); }

	_SD(void *pData, int iLen, bool bRecopy) : _D(pData, iLen)
	{
		m_bNeedDelete = true;
		if (bRecopy)
			Copy();
	}

	~_SD() {}

private:

	void Copy()
	{
		m_bNeedDelete = true;
		const void *pOldData = m_pData;
		m_pData = malloc(m_iLen);
		memcpy((void*)m_pData, pOldData, m_iLen);
	}

};

// END_NOLINT
// END_NOLINT

//////////////////////////////////////////////////////////////////////////

class CDataCollector
{

public:

	CDataCollector() {}
	~CDataCollector()
	{
		for (size_t i = 0; i < m_Data.size(); i++)
			m_Data[i].Free();
	}

	CDataCollector& operator << (const _D &data) { m_Data.push_back(data); return *this; }

	bool Empty() const { return m_Data.empty(); }

protected:

	std::vector<_D> m_Data;

};

//////////////////////////////////////////////////////////////////////////

struct BinaryArg
{
	const void *pData;
	int iLen;
};

class CDataArgs
{

public:

	CDataArgs() : m_pArgs(NULL), m_iArgCount(0), m_uMsg(0), m_iCurrentWalkingArg(0), m_pCurrentMappingData(NULL) {}

	~CDataArgs() { Release(); }

	void Release() { Universal::DataSerializer::ReleaseDataArgs(this); }

	bool Valid() const { return m_pArgs != NULL; }


	const _D& operator [] (int iIndex) const
	{
		TuoAssert(m_pArgs);
		TuoAssert(iIndex >= 0 && iIndex < m_iArgCount);
		return m_pArgs[iIndex];
	}

	CDataArgs& operator >> (bool &bVal)
	{
		int intVal = m_pArgs[m_iCurrentWalkingArg].GetIntArg();
		m_iCurrentWalkingArg++;
		TuoAssert(intVal == 0 || intVal == 1);
		bVal = !!intVal;
		return *this;
	}

	CDataArgs& operator >> (int &intVal)
	{
		intVal = m_pArgs[m_iCurrentWalkingArg].GetIntArg();
		m_iCurrentWalkingArg++;
		return *this;
	}
	CDataArgs& operator >> (DWORD &uintVal)
	{
		uintVal = m_pArgs[m_iCurrentWalkingArg].GetIntArg();
		m_iCurrentWalkingArg++;
		return *this;
	}
	CDataArgs& operator >> (__int64 &intVal)
	{
		intVal = m_pArgs[m_iCurrentWalkingArg].GetInt64Arg();
		m_iCurrentWalkingArg++;
		return *this;
	}

	CDataArgs& operator >> (HANDLE &hHandleVal)
	{
		hHandleVal = (HANDLE)m_pArgs[m_iCurrentWalkingArg].GetIntArg();
		m_iCurrentWalkingArg++;
		return *this;
	}
	CDataArgs& operator >> (HWND &hWndVal)
	{
		hWndVal = (HWND)m_pArgs[m_iCurrentWalkingArg].GetIntArg();
		m_iCurrentWalkingArg++;
		return *this;
	}
	CDataArgs& operator >> (HKL &hklVal)
	{
		hklVal = (HKL)m_pArgs[m_iCurrentWalkingArg].GetIntArg();
		m_iCurrentWalkingArg++;
		return *this;
	}

	CDataArgs& operator >> (LPCWSTR &lpszVal)
	{
		lpszVal = m_pArgs[m_iCurrentWalkingArg].GetStrArg();
		m_iCurrentWalkingArg++;
		return *this;
	}
	CDataArgs& operator >> (LPCSTR &lpszVal)
	{
		lpszVal = m_pArgs[m_iCurrentWalkingArg].GetStrAArg();
		m_iCurrentWalkingArg++;
		return *this;
	}
	CDataArgs& operator >> (BinaryArg &binaryArg)
	{
		binaryArg.pData = m_pArgs[m_iCurrentWalkingArg].GetBinaryArg();
		binaryArg.iLen = m_pArgs[m_iCurrentWalkingArg].GetBinaryArgLength();
		m_iCurrentWalkingArg++;
		return *this;
	}


	UINT GetMessage() const { return m_uMsg; }
	int GetArgCount() const { return m_iArgCount; }

protected:

	_D *m_pArgs;
	int m_iArgCount;
	UINT m_uMsg;

	const BYTE *m_pCurrentMappingData;
	int m_iCurrentWalkingArg;

	friend class CDataCollectorInternal;
};
