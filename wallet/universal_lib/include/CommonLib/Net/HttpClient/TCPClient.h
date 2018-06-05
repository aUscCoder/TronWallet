// Author: WuJian
#pragma once

#include "../HttpClient/UnlimitedBuffer.h"
#include "SocketClient.h"

namespace HttpClient
{
	class CTCPClient
	{
	public:
		int SendAndRecv(const char *lpszHostName, const char *lpszPortNumber, BYTE* lpData, int iLength, HttpClient::CUnlimitedBuffer &serverResponse);

	private:
		int Send(CSocketClient &sock, BYTE* lpData, int iLength);
		int Recv(CSocketClient &sock, HttpClient::CUnlimitedBuffer &serverResponse);
	};
}
