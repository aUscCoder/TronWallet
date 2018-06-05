// Author: liujing
#pragma once

namespace HttpClient
{
	class CSocketClient
	{
	public:
		CSocketClient();
		~CSocketClient();

		int SCreate(const char *lpszHost, const char *lpszPort);
		int Close();

		int SSend(const char *lpszBuffer, int iLength, int iFlags);
		int SRecv(char *lpszBuffer, int iLength, int iFlags);

	private:

		SOCKET m_socket;
	};
}
