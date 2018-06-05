// Author: WuJian
#pragma once
#include "../../Universal.h"

UNIVERSAL_LIB_API BYTE* SEBlowFish_Encrypt(const BYTE *lpContent, DWORD dwLength, DWORD *pOutLength, const BYTE *pKey = NULL, DWORD dwKeyLen = 0);
UNIVERSAL_LIB_API BYTE* SEBlowFish_Decrypt(const BYTE *lpBuffer, DWORD dwLength, DWORD *pOutLength, const BYTE *pKey = NULL, DWORD dwKeyLen = 0);
