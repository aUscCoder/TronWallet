// Author: WuJian
#pragma once

#include "UnlimitedBuffer.h"

namespace HttpClient
{
	class CHttpResponse
	{
		typedef std::map<std::string, std::string> HeaderMap;
	public:
		explicit CHttpResponse(CUnlimitedBuffer response);
		~CHttpResponse();

		HeaderMap& GetHttpHeaders();
		void TakeBody(char **ppDownloadBuffer, unsigned long *pDownloadSize);
		bool OK();

	protected:


	private:

		void ParseResponse(CUnlimitedBuffer &response);

		void ParseBody(CUnlimitedBuffer &response);
		void ParseAHeader(const std::string &strSegment);
		std::string ParseSegment(CUnlimitedBuffer &response);
		DWORD CalcSegmentLength(CUnlimitedBuffer &response, char cSeperator);

		void ParseChunkedBody(CUnlimitedBuffer &response);
		void ParseContentLengthBody(CUnlimitedBuffer &response);

		

	protected:

		union
		{
			bool m_bChunked;
			DWORD m_dwContentLength;
		};
		DWORD m_dwStatusCode;
		HeaderMap m_headerMap;
		CUnlimitedBuffer m_body;
	};
}
