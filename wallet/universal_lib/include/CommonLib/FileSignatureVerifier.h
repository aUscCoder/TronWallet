// Author: WangZhan
#pragma once
#include "../Universal.h"

namespace Universal
{
	namespace SigVerifier
	{
		UNIVERSAL_LIB_API bool VerifyFileSignature(LPCWSTR lpszFilePath, LPCWSTR lpszSubjectName);
	}
}

