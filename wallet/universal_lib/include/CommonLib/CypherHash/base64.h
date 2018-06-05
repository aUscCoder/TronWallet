// Author: wujian
#pragma once
#include "../../Universal.h"
#include "../String/Convert.h"


UNIVERSAL_LIB_API int base64_encode(const void *in_str, int length, char *out_str, int *ret_length);
UNIVERSAL_LIB_API int base64_decode(const char *in_str, int length, void *out_str, int *ret_length);

UNIVERSAL_LIB_API std::string Base64Encode(const void *pData, int iLen);
UNIVERSAL_LIB_API char* Base64Decode(const std::string &strBase64, int &iRetLen);

UNIVERSAL_LIB_API std::wstring base64_encode_str(LPCWSTR lpszSrc, CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API std::wstring base64_decode_str(LPCWSTR lpszBase64Code, CodePage dwCodePage = CP_Utf8);
UNIVERSAL_LIB_API std::string base64_encode_str(const std::string& str);
UNIVERSAL_LIB_API std::string base64_decode_str(const std::string& str);
