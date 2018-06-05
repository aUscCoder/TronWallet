// Author: chenjianjun
#pragma once
#include <util.h>
#include "../../Universal.h"


struct CodePage
{
	CodePage() : dwCodePage(CP_UTF8) {}
	CodePage(DWORD c, int) : dwCodePage(c) {}
	~CodePage() {}

	bool operator == (const CodePage &cp) const
	{
		return dwCodePage == cp.dwCodePage;
	}

	bool operator != (const CodePage &cp) const
	{
		return dwCodePage != cp.dwCodePage;
	}

	DWORD dwCodePage;
};

const int g_iAcp_DONOT_USE_THIS = CP_ACP;
const int g_iUtf8_DONOT_USE_THIS = CP_UTF8;
const int g_iUtf7_DONOT_USE_THIS = CP_UTF7;

#undef CP_ACP
#undef CP_OEMCP
#undef CP_MACCP
#undef CP_THREAD_ACP
#undef CP_SYMBOL
#undef CP_UTF7
#undef CP_UTF8


#define CP_Utf8		CodePage(g_iUtf8_DONOT_USE_THIS, 0)
#define CP_Utf7		CodePage(g_iUtf7_DONOT_USE_THIS, 0)
#define CP_Ansi		CodePage(g_iAcp_DONOT_USE_THIS, 0)
#define CP_GBK		CodePage(936, 0)


UNIVERSAL_LIB_API std::wstring AToW(__in LPCSTR lpszSrc, __in int iSrcLen = -1, __in CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API std::wstring AToW(__in const std::string& str, __in CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API std::wstring AToW(__in const se::stringbuf& sb, __in CodePage codePage = CP_Utf8);
LPCWSTR AToW(__in LPCSTR lpszSrc, __out LPWSTR lpszDest, __in const int iDestLen, __in CodePage codePage = CP_Utf8);
LPCWSTR AToW(__in LPCSTR lpszSrc, __in const int iSrcLen, __out LPWSTR lpszDest, __in const int iDestLen, __in CodePage codePage = CP_Utf8);
LPCWSTR AToW(__in LPCSTR lpszSrc, __out std::wstring& strDest, __in CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API LPCWSTR AToW(__in LPCSTR lpszSrc, __in const int iSrcLen, __out std::wstring& strDest, __in CodePage codePage = CP_Utf8);

UNIVERSAL_LIB_API std::string WToA(__in LPCWSTR lpszSrc, __in const int iSrcLen = -1, __in CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API std::string WToA(__in const std::wstring& str, __in CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API std::string WToA(__in const se::wstringbuf& sb, __in CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API LPCSTR WToA(__in LPCWSTR lpszSrc, __out LPSTR lpszDest, __in const int iDestLen, __in CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API LPCSTR WToA(__in LPCWSTR lpszSrc, __in const int iSrcLen, __out LPSTR lpszDest, __in const int iDestLen,
	__in CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API LPCSTR WToA(__in LPCWSTR lpszSrc, __out std::string& strDest, __in CodePage codePage = CP_Utf8);
UNIVERSAL_LIB_API LPCSTR WToA(__in LPCWSTR lpszSrc, __in const int iSrcLen, __out std::string& strDest, __in CodePage codePage = CP_Utf8);
