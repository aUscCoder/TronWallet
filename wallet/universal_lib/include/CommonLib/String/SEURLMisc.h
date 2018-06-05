#pragma once
#include "../../Universal.h"


namespace Universal
{
	namespace SEURLMisc
	{
		UNIVERSAL_LIB_API DWORD GetDocId(const std::wstring& strUrl);
		UNIVERSAL_LIB_API DWORD GetDocId2(const std::wstring& strUrl);// 转成小写url再算
	}
}