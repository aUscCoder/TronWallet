// Author: wujian
#pragma once
#include "../../Universal.h"

UNIVERSAL_LIB_API BYTE* RSA_Encrypt(const BYTE *lpContent, DWORD dwLength, DWORD *pOutLength, const BYTE *pKey = NULL, DWORD dwKeyLen = 0);

