// Author: TuotuoXP
#pragma once
#include "../Universal.h"
#include "String/Convert.h"


enum
{
	WINDOWS_OLDEST	= -1000,

	WINDOWS_2000	= 1,
	WINDOWS_XP_NO_SP,
	WINDOWS_XP_SP_1,
	WINDOWS_XP_SP_2,
	WINDOWS_XP_SP_3,
	WINDOWS_2003,
	WINDOWS_VISTA_NO_SP,
	WINDOWS_VISTA_SP_1,
	WINDOWS_VISTA_SP_2,
	WINDOWS_7,
	WINDOWS_8,
	WINDOWS_8_1,
	WINDOWS_10,

	WINDOWS_LATEST	= 1000,
};


namespace Universal
{
	namespace Misc
	{
		UNIVERSAL_LIB_API long GetAccurateOSVersion();
		UNIVERSAL_LIB_API long GetIEVersion();
		UNIVERSAL_LIB_API bool IsAdmin();

		// 探测char* 字符串的编码，当前只能分辨是不是UTF-8编码的字符串
		UNIVERSAL_LIB_API CodePage DetectEncoding(const char* psz);

		UNIVERSAL_LIB_API DWORD GetParentProcessIDByPID(DWORD dwPID);
		UNIVERSAL_LIB_API std::wstring GetProcessNameByPID(DWORD dwPID);
		inline std::wstring GetParentProcessName() { return GetProcessNameByPID(GetParentProcessIDByPID(GetCurrentProcessId())); }

		UNIVERSAL_LIB_API DWORD GetSystemIdleTime();

		UNIVERSAL_LIB_API SECURITY_ATTRIBUTES* GetLowSecurityDescriptor();

		UNIVERSAL_LIB_API long GetUniqueID();

		UNIVERSAL_LIB_API std::string GenGUID();

		UNIVERSAL_LIB_API void ShowItemInNewFolder(const std::wstring& strFile, bool bIsDesktop);

		UNIVERSAL_LIB_API std::wstring GetProcessCommandLine(HANDLE hTargetProcess);
	}
}


//
// IS_KEY_DOWN
// GetKeyState(), If the high - order bit is 1, the key is down; otherwise, it is up.
//
#ifndef IS_KEY_DOWN
# define IS_KEY_DOWN(vkValue) ((::GetKeyState(vkValue)&0x8000) == 0x8000)
#endif

//
// IS_KEY_TOGGLED
// GetKeyState(), If the low - order bit is 1, the key is toggled.A key, such as the CAPS LOCK key, is toggled if it is turned on.
//
#ifndef IS_KEY_TOGGLED
# define IS_KEY_TOGGLED(vkValue) ((::GetKeyState(vkValue)&0x1) == 0x1)
#endif

// 不允许使用拷贝构造和赋值
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)
#endif