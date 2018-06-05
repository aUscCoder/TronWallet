// Author: WuJian
#pragma once
#include <windows.h>

BYTE* BlowFish_Encrypt(const BYTE *lpContent, DWORD dwLength, DWORD *pOutLength, const BYTE *pKey = NULL, DWORD dwKeyLen = 0);
BYTE* BlowFish_Decrypt(const BYTE *lpBuffer, DWORD dwLength, DWORD *pOutLength, const BYTE *pKey = NULL, DWORD dwKeyLen = 0);