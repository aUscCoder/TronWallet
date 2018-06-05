// Author: WuJian
#pragma once

namespace HttpClient
{
	class IHttpClient
	{
	public:

		IHttpClient() {}
		virtual ~IHttpClient() {}

		virtual int Get(const char *lpszUrl, char **ppDownloadBuffer, unsigned long *pDownloadSize) = 0;
		virtual int Post(const char *lpszUrl, BYTE *lpBody, unsigned long uBodySize, char **ppDownloadBuffer, unsigned long *pDownloadSize) = 0;
		virtual void SetUserAgent(const char *lpszUA) = 0;
	};
}