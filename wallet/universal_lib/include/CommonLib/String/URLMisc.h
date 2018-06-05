// Author: TuotuoXP
#pragma once
#include "../../Universal.h"
#include "Convert.h"


namespace Universal
{
	namespace URLMisc
	{
		enum UrlFlag
		{
			Url_Scheme = 1 << 0,
			Url_Host = 1 << 1,
			Url_UserName = 1 << 2,
			Url_Password = 1 << 3,
			Url_Path = 1 << 4,
			Url_Port = 1 << 5,
			Url_FileName = 1 << 6,
			Url_TopDomain = 1 << 7,
			Url_Query = 1 << 8,

			Url_All = Url_Scheme | Url_Host | Url_UserName | Url_Password | Url_Path | Url_Port | Url_FileName | Url_TopDomain | Url_Query,
		};

		struct URLInfo
		{
			std::string strScheme;
			std::string strHost;
			std::string strUserName;
			std::string strPassword;
			std::string strPath;
			std::string strPort;
			std::string strFileName;
			std::string strTopDomain;
			std::string strQuery;
		};

		// 修崩溃 27229 6.0.5.17681 20150812 DoLowerCaseEqualsASCII
		// 初始化全局变量，避免多线程问题
		UNIVERSAL_LIB_API void Initialize();

		UNIVERSAL_LIB_API void NormalizeURL(std::wstring &inputAndOutputURL);
		/**
		* \fn	void ConvertToDisplayURL(std::wstring &inputAndOutputURL);
		*
		* \brief	将一个URL转换成供UI展现的格式,该函数会Normalize
		*
		* \author	miaojinquan
		* \date	2016-03-11
		*
		* \param[in,out]	inputAndOutputURL	源字符串
		*
		*/
		UNIVERSAL_LIB_API void ConvertToDisplayURL(std::wstring &inputAndOutputURL);
		UNIVERSAL_LIB_API std::wstring FileUrlToFilePath(LPCWSTR lpszUrl);
		UNIVERSAL_LIB_API bool IsURLLeafPage(LPCSTR lpszURL);	// 判断这个URL是不是一个最终页

		UNIVERSAL_LIB_API std::wstring ExtractDomain(LPCWSTR lpszUrl);
		UNIVERSAL_LIB_API std::string ExtractDomain(LPCSTR lpszUrl);
		UNIVERSAL_LIB_API std::wstring ExtractPath(LPCWSTR lpszUrl);
		UNIVERSAL_LIB_API std::string ExtractPath(LPCSTR lpszUrl);
		UNIVERSAL_LIB_API std::wstring ExtractTopDomain(LPCWSTR lpszUrl);
		UNIVERSAL_LIB_API std::string ExtractScheme(LPCWSTR lpszUrl);
		UNIVERSAL_LIB_API std::string ExtractScheme(LPCSTR lpszUrl);
		UNIVERSAL_LIB_API bool CrackURL(LPCSTR lpszUrl, DWORD dwFlag, URLInfo& info);

		UNIVERSAL_LIB_API std::string UrlEncode(const std::string& strUrl);
		UNIVERSAL_LIB_API std::string UrlDecode(const std::string& strUrl);

		inline std::wstring UrlEncodeW(LPCWSTR lpszURL) { return AToW(UrlEncode(WToA(lpszURL))); }

		UNIVERSAL_LIB_API std::wstring HtmlEscape(const std::wstring &szText);
		UNIVERSAL_LIB_API std::wstring HtmlUnescape(const std::wstring &szText);

		/**
		* \fn	UNIVERSAL_LIB_API std::string UnescapeURLComponent(const std::string& strUrl);
		*
		* \brief	Unescape url component.在字符串中查找"%XX",（X是一个16进制的值），然后转换成数字
		* 			比如“%E4%BD%A0” 会转换为e4bda0 这个函数不会将“+”号变为“ ”；并且会对一些不会
		* 			在url中出现的特殊字符不做处理,并DecodeUrl
		*
		* \author	miaojinquan
		* \date	2016-03-11
		*
		* \param	strUrl	源字符串
		*
		* \return	A std::string.
		*/

		UNIVERSAL_LIB_API std::string UnescapeURLComponent(const std::string& strUrl);

		// 转换 "\'" 为 "\'\'"
		UNIVERSAL_LIB_API std::string TranslateSingleQuotes(const std::string &str);
		UNIVERSAL_LIB_API std::string TranslateSingleQuotes(LPCSTR lpsz, int nCount);

		UNIVERSAL_LIB_API std::wstring CombineUrl(const wchar_t* url, const wchar_t* href_value); // 效果等价于InternetCombineUrl

		UNIVERSAL_LIB_API bool IsUrlHostNameValid(LPCWSTR szUrl);
		UNIVERSAL_LIB_API bool IsValidUrl(LPCWSTR szUrl);

		// 取#号前面的url数据
		UNIVERSAL_LIB_API std::string ExtractRegularUrl(const std::string &strUrl);
	}
}