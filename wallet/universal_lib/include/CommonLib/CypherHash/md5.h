// Author: wujian
#pragma once
#include "../../Universal.h"

typedef unsigned long int UINT4;

struct MD5_CTX
{
	UINT4 state[4];				/* state (ABCD) */
	UINT4 count[2];				/* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];	/* input buffer */
};


namespace Universal
{
	namespace Misc
	{
		UNIVERSAL_LIB_API void MD5Hash(const void *pBufIn, int iBufLen, void *pBufOut);		// sizeof(pBufOut)至少为16
		UNIVERSAL_LIB_API void MD5HashX(const void *pBufIn, int iBufLen, void *pBufOut, int iOutBufLen);		// sizeof(pBufOut)为4/8/16

		UNIVERSAL_LIB_API void MD5HashToString(const void *pBufIn, int iBufLen, wchar_t *szBufOut);	// _countof(szBufOut)至少为32 + 1
		UNIVERSAL_LIB_API void MD5HashToStringA(const void *pBufIn, int iBufLen, char *szBufOut);		// _countof(szBufOut)至少为32 + 1

		UNIVERSAL_LIB_API bool MD5HashFileToString(LPCWSTR szFile, wchar_t *szBufOut);		// _countof(szBufOut)至少为32 + 1
		UNIVERSAL_LIB_API bool MD5HashFileToStringA(LPCWSTR szFile, char *szBufOut);		// _countof(szBufOut)至少为32 + 1

		UNIVERSAL_LIB_API std::wstring MD5HashToString(const void *pBufIn, int iBufLen);
		UNIVERSAL_LIB_API std::string MD5HashToStringA(const void *pBufIn, int iBufLen);
	}
}

//////////////////////////////////////////////////////////////////////////

UNIVERSAL_LIB_API void MD5Init(MD5_CTX *context);
UNIVERSAL_LIB_API void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen);
UNIVERSAL_LIB_API void MD5Final(unsigned char digest[], MD5_CTX *context, int iDigestLen);
