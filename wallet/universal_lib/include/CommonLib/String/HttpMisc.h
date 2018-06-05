// Author: liujing
#pragma once
#include "../../Universal.h"

struct HttpRequestHeader
{
	std::string strVerb;
	std::string strPath;
	std::string strHttpVersion;
	std::map<std::string, std::string> headers;
};

struct HttpResponseHeader
{
	std::string strHttpVersion;
	int iStatusCode;
	std::string strStatusInfo;
	std::map<std::string, std::string> headers;
};

namespace Universal
{
	namespace HttpMisc
	{
		UNIVERSAL_LIB_API void ParseRequestHeader(const std::string& strHeader, HttpRequestHeader& request);
		UNIVERSAL_LIB_API void ParseResponseHeader(const std::string& strHeader, HttpResponseHeader& response);
		UNIVERSAL_LIB_API std::wstring GetFileExtFromContentType(LPCSTR lpszContentType);
		UNIVERSAL_LIB_API std::wstring ExtractFileNameFromHeader(const std::wstring& strURL,
			const std::string& strContentDisposition, const std::string& strHttpRequestHeader, const std::string& strContentType);
	}
}
