// Author: WuJian
#pragma once

#include "HttpClient.h"

namespace HttpClient
{
	class CSocketHttpClient : public IHttpClient
	{
	public:

		CSocketHttpClient();
		virtual ~CSocketHttpClient();

		int Get(const char *lpszUrl, char **ppDownloadBuffer, unsigned long *pDownloadSize) override;
		int Post(const char *lpszUrl, BYTE *lpBody, unsigned long uBodySize, char **ppDownloadBuffer, unsigned long *pDownloadSize) override;
		void SetUserAgent(const char *lpszUA) override;

	protected:
		std::string m_strPort;
		std::string m_strUA;
	};
}