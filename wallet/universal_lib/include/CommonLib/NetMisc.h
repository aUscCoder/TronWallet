// Author:	Yanghao
#pragma once
#include "../Universal.h"
#include "FakeWinInetApi.h"

namespace Universal
{
	namespace NetMisc
	{

		WININET_API(HINTERNET, InternetOpenA,
					(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxyName, LPCSTR lpszProxyBypass, DWORD dwFlags),
					(lpszAgent, dwAccessType, lpszProxyName, lpszProxyBypass, dwFlags));
		WININET_API(HINTERNET, InternetOpenW,
					(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxyName, LPCWSTR lpszProxyBypass, DWORD dwFlags),
					(lpszAgent, dwAccessType, lpszProxyName, lpszProxyBypass, dwFlags));

		WININET_API(HINTERNET, InternetOpenUrlA,
					(HINTERNET hInternet, LPCSTR lpszUrl, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwFlags, DWORD_PTR dwContext),
					(hInternet, lpszUrl, lpszHeaders, dwHeadersLength, dwFlags, dwContext));
		WININET_API(HINTERNET, InternetOpenUrlW,
					(HINTERNET hInternet, LPCWSTR lpszUrl, LPCWSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwFlags, DWORD_PTR dwContext),
					(hInternet, lpszUrl, lpszHeaders, dwHeadersLength, dwFlags, dwContext));


		WININET_API(HINTERNET, InternetConnectA,
					(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort,
					LPCSTR lpszUsername, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext),
					(hInternet, lpszServerName, nServerPort, lpszUsername, lpszPassword, dwService, dwFlags, dwContext));
		WININET_API(HINTERNET, InternetConnectW,
					(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort,
					LPCWSTR lpszUsername, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext),
					(hInternet, lpszServerName, nServerPort, lpszUsername, lpszPassword, dwService, dwFlags, dwContext));

		WININET_API(HINTERNET, HttpOpenRequestA,
					(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer,
					LPCSTR FAR * lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext),
					(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer,
					lplpszAcceptTypes, dwFlags, dwContext));
		WININET_API(HINTERNET, HttpOpenRequestW,
					(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer,
					LPCWSTR FAR * lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext),
					(hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer,
					lplpszAcceptTypes, dwFlags, dwContext));


		WININET_API(BOOL, HttpSendRequestExA,
					(HINTERNET hRequest, LPINTERNET_BUFFERSA lpBuffersIn, LPINTERNET_BUFFERSA lpBuffersOut, DWORD dwFlags, DWORD_PTR dwContext),
					(hRequest, lpBuffersIn, lpBuffersOut, dwFlags, dwContext));
		WININET_API(BOOL, HttpSendRequestExW,
					(HINTERNET hRequest, LPINTERNET_BUFFERSW lpBuffersIn, LPINTERNET_BUFFERSW lpBuffersOut, DWORD dwFlags, DWORD_PTR dwContext),
					(hRequest, lpBuffersIn, lpBuffersOut, dwFlags, dwContext));

		WININET_API(BOOL, InternetCrackUrlA,
			(LPCSTR lpszUrl, DWORD dwUrlLength, DWORD dwFlags, LPURL_COMPONENTSA lpUrlComponents),
			(lpszUrl, dwUrlLength, dwFlags, lpUrlComponents));


		WININET_API(BOOL, HttpAddRequestHeadersA,
			(HINTERNET hRequest, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwModifiers),
			(hRequest, lpszHeaders, dwHeadersLength, dwModifiers));


		WININET_API(BOOL, HttpEndRequestA,
			(HINTERNET hRequest, LPINTERNET_BUFFERSA lpBuffersOut, DWORD dwFlags, DWORD_PTR dwContext),
			(hRequest, lpBuffersOut, dwFlags, dwContext));


		WININET_API(BOOL, HttpQueryInfoA,
			(HINTERNET hRequest, DWORD dwInfoLevel, LPVOID lpBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex),
			(hRequest, dwInfoLevel, lpBuffer, lpdwBufferLength, lpdwIndex));

		WININET_API(BOOL, InternetReadFile,
			(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead),
			(hFile, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead));


		WININET_API(BOOL, InternetWriteFile,
			(HINTERNET hFile, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten),
			(hFile, lpBuffer, dwNumberOfBytesToWrite, lpdwNumberOfBytesWritten));

		WININET_API(BOOL, InternetCloseHandle, (HINTERNET hInternet), (hInternet));

		//
		UNIVERSAL_LIB_API void PostPingbackWithWinINet(LPCSTR lpszUrl, LPCSTR lpszParam);
		//void PostByWinINet(LPCSTR lpszUrl, LPCSTR lpszContent, int iLen, LPCSTR lpszAddHeader);
	}
}
