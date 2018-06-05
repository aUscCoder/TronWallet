// Author: wujian
#pragma once
#include "../../Universal.h"


UNIVERSAL_LIB_API BYTE* AES_Encrypt(const BYTE *lpContent, DWORD dwLength, DWORD *pOutLength,
									const BYTE *pKey = NULL, DWORD dwKeyLen = 0, const BYTE *iv = NULL, DWORD dwIVLen = 0);

UNIVERSAL_LIB_API BYTE* AES_Decrypt(const BYTE *lpBuffer, DWORD dwLength, DWORD *pOutLength,
									const BYTE *pKey = NULL, DWORD dwKeyLen = 0, const BYTE *iv = NULL, DWORD dwIVLen = 0);

UNIVERSAL_LIB_API BYTE* AES_HashString(const BYTE* lpBuffer, DWORD dwLength, DWORD *pOutLength);