// Author: chenjianjun
#pragma once
#include "../../Universal.h"


namespace Universal
{
	namespace Assoc
	{
		UNIVERSAL_LIB_API std::wstring GetFileAssoc(LPCWSTR lpExt);
		UNIVERSAL_LIB_API std::wstring GetProtocolAssoc(LPCWSTR lpProt);
	}
}
