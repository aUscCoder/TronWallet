// Author: WuJian

#pragma once

namespace HttpClient
{
	class CUnlimitedBuffer
	{
	public:
		CUnlimitedBuffer();
		explicit CUnlimitedBuffer(DWORD dwInitBufferLength);
		CUnlimitedBuffer(const CUnlimitedBuffer &r);
		~CUnlimitedBuffer();

		void Free();
		void Move(BYTE **ppOut, DWORD *pLength);
		DWORD Append(const BYTE *pNewData, DWORD dwNewDataLength);
		void MoveForward(DWORD dwStep);
		void SkipAnyOf(const std::string &strSkip);

		char* StrBuffer() const { return (char*)m_pBuffer; }
		BYTE* Buffer() const { return m_pBuffer; }
		DWORD BufferLength() const { return m_dwBufferUsedLength; }

	private:
		void TryGrow(DWORD dwRequiredLength);

	private:
		BYTE *m_pBuffer;
		DWORD m_dwBufferFullLength;
		DWORD m_dwBufferUsedLength;
		bool m_bMutable;
	};
}
