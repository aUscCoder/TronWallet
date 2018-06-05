// Author: Amax
#pragma once
#include <stdarg.h>
#include <string>
#include "tuoassert.h"


#define USE_SE_STRINGBUF


template<class char_type> struct Printfs	// 只允许用来做格式化操作,不允许用来拼接字符串
{
	inline Printfs(const char_type *format, ...);
	char_type szBuf[64];	// 最长只允许64字符
	int iLen;
};

Printfs<char>::Printfs(const char *format, ...)
{
	TuoAssert(strstr(format, "%s") == NULL);
	TuoAssert(strstr(format, "%S") == NULL);
	TuoAssert(strstr(format, "%ws") == NULL);
	va_list va;
	va_start(va, format);
	iLen = vsprintf_s(szBuf, format, va);
	va_end(va);
}

Printfs<wchar_t>::Printfs(const wchar_t *format, ...)
{
	TuoAssert(wcsstr(format, L"%s") == NULL);
	TuoAssert(wcsstr(format, L"%S") == NULL);
	TuoAssert(wcsstr(format, L"%ws") == NULL);
	va_list va;
	va_start(va, format);
	iLen = vswprintf_s(szBuf, format, va);
	va_end(va);
}

typedef Printfs<char> PrintfsA;
typedef Printfs<wchar_t> PrintfsW;


template<class char_type> struct StrWithLen
{
	StrWithLen(const char_type *s, int len) : pStr(s), iLen(len) {}
	const char_type *pStr;
	int iLen;
};

typedef StrWithLen<char> StrAWithLen;
typedef StrWithLen<wchar_t> StrWWithLen;


namespace se
{

template<typename char_type> inline size_t strlenTemplate(const char_type* string);
template<> inline size_t strlenTemplate<char>(const char* string) { return strlen(string); }
template<> inline size_t strlenTemplate<wchar_t>(const wchar_t* string) { return wcslen(string); }


template <typename char_type, size_t extraSize = 512>
class basic_stringBuffer
{
	typedef basic_stringBuffer<char_type, extraSize> stringBuffer_t;

public:

	basic_stringBuffer() : m_iLength(0), m_iBufSize(extraSize)
	{
		m_lpszBuffer = m_initialBuffer;
	}
	~basic_stringBuffer()
	{
		if (m_lpszBuffer != m_initialBuffer)
			free(m_lpszBuffer);
	}

	size_t length() const
	{
		return m_iLength;
	}

	void crop(size_t iSize)
	{
		if (m_iLength <= iSize)
			m_iLength = 0;
		else
			m_iLength -= iSize;
		m_lpszBuffer[m_iLength] = (char_type)0;
	}

	void clear()
	{
		m_lpszBuffer[0] = (char_type)0;
		m_iLength = 0;
	}

	friend inline stringBuffer_t& operator << (stringBuffer_t& sbLeft, const StrWithLen<char_type> &strRight)
	{
		if (strRight.iLen == 0)
			return sbLeft;
		size_t iNewLen = sbLeft.m_iLength + strRight.iLen;
		if (iNewLen >= sbLeft.m_iBufSize)
			sbLeft.expandCapacity(iNewLen);
		memcpy(sbLeft.m_lpszBuffer + sbLeft.m_iLength, strRight.pStr, strRight.iLen * sizeof(char_type));
		sbLeft.m_iLength = iNewLen;
		return sbLeft;
	}

	friend inline stringBuffer_t& operator << (stringBuffer_t& sbLeft, const std::basic_string<char_type> &strRight)
	{
		return sbLeft << StrWithLen<char_type>(strRight.c_str(), strRight.length());
	}

	friend inline stringBuffer_t& operator << (stringBuffer_t& sbLeft, stringBuffer_t& strRight)
	{
		return sbLeft << StrWithLen<char_type>(strRight.str(), strRight.length());
	}

	friend inline stringBuffer_t& operator << (stringBuffer_t& sbLeft, const char_type * lpszRight)
	{
		TuoAssert(lpszRight != NULL);
		if (lpszRight && lpszRight[0] != (char_type)0)
			sbLeft << StrWithLen<char_type>(lpszRight, strlenTemplate(lpszRight));
		return sbLeft;
	}

	friend inline stringBuffer_t& operator << (stringBuffer_t& sbLeft, const Printfs<char_type>& printfs)
	{
		return sbLeft << StrWithLen<char_type>(printfs.szBuf, printfs.iLen);
	}

	friend inline stringBuffer_t& operator << (stringBuffer_t& sbLeft, const char_type cRight)
	{
		size_t iNewLen = sbLeft.m_iLength + 1;
		if (iNewLen >= sbLeft.m_iBufSize)
			sbLeft.expandCapacity(iNewLen);
		sbLeft.m_lpszBuffer[sbLeft.m_iLength] = cRight;
		sbLeft.m_iLength = iNewLen;
		return sbLeft;
	}

	const char_type* str() const
	{
		m_lpszBuffer[m_iLength] = (char_type)0;
		return m_lpszBuffer;
	}


private:

	void expandCapacity(size_t minCapacity = 0)
	{
		size_t iNewCapacity = (m_iBufSize + 1) * 2;
// 		if (iNewCapacity < m_iBufSize) // exceed max int, wont happen on 32 bit machine, if move to 64 bit, should check
// 		{
// 			iNewCapacity = 0xFFFFFFFF;
// 		}
		if (minCapacity >= iNewCapacity)
		{
			iNewCapacity = minCapacity + 1; // make sure there's one more space for '\0'
		}
		m_iBufSize = iNewCapacity;
		if (m_lpszBuffer == m_initialBuffer)
		{
			m_lpszBuffer = (char_type*)malloc(m_iBufSize * sizeof(char_type));
			memcpy(m_lpszBuffer, m_initialBuffer, extraSize * sizeof(char_type));
		}
		else
		{
			m_lpszBuffer = (char_type*)realloc(m_lpszBuffer, m_iBufSize * sizeof(char_type));
		}
	}

	size_t m_iLength;
	size_t m_iBufSize;
	char_type * m_lpszBuffer;
	char_type m_initialBuffer[extraSize];
	basic_stringBuffer(const basic_stringBuffer<char_type, extraSize>& sbRight);
	basic_stringBuffer& operator= (const basic_stringBuffer& sbRight);

};

typedef basic_stringBuffer<char> stringbuf;
typedef basic_stringBuffer<wchar_t> wstringbuf;


// 模板类操作符特化

inline wstringbuf& operator << (wstringbuf& sbLeft, const char cRight)
{
	return sbLeft << (wchar_t)cRight;
}

inline stringbuf& operator << (stringbuf& sbLeft, const long lRight)
{
	char lpszRight[32];
	_ltoa_s(lRight, lpszRight, 32, 10);
	return sbLeft << lpszRight;
}
inline wstringbuf& operator << (wstringbuf& sbLeft, const long lRight)
{
	wchar_t lpszRight[32];
	_ltow_s(lRight, lpszRight, 32, 10);
	return sbLeft << lpszRight;
}

inline stringbuf& operator << (stringbuf& sbLeft, const unsigned long dwRight)
{
	char lpszRight[32];
	_ultoa_s(dwRight, lpszRight, 32, 10);
	return sbLeft << lpszRight;
}
inline wstringbuf& operator << (wstringbuf& sbLeft, const unsigned long dwRight)
{
	wchar_t lpszRight[32];
	_ultow_s(dwRight, lpszRight, 32, 10);
	return sbLeft << lpszRight;
}

inline stringbuf& operator << (stringbuf& sbLeft, const int lRight)
{
	char lpszRight[32];
	_itoa_s(lRight, lpszRight, 32, 10);
	return sbLeft << lpszRight;
}
inline wstringbuf& operator << (wstringbuf& sbLeft, const int lRight)
{
	wchar_t lpszRight[32];
	_itow_s(lRight, lpszRight, 32, 10);
	return sbLeft << lpszRight;
}

inline stringbuf& operator << (stringbuf& sbLeft, const unsigned int iRight)
{
	char lpszRight[32];
	_ultoa_s(iRight, lpszRight, 32, 10);
	return sbLeft << lpszRight;
}
inline wstringbuf& operator << (wstringbuf& sbLeft, const unsigned int iRight)
{
	wchar_t lpszRight[32];
	_ultow_s(iRight, lpszRight, 32, 10);
	return sbLeft << lpszRight;
}

inline stringbuf& operator << (stringbuf& sbLeft, const unsigned __int64 &intVal)
{
	return sbLeft << PrintfsA("%I64u", intVal);
}
inline wstringbuf& operator << (wstringbuf& sbLeft, const unsigned __int64 &intVal)
{
	return sbLeft << PrintfsW(L"%I64u", intVal);
}
inline stringbuf& operator << (stringbuf& sbLeft, const __int64 &intVal)
{
	return sbLeft << PrintfsA("%I64d", intVal);
}
inline wstringbuf& operator << (wstringbuf& sbLeft, const __int64 &intVal)
{
	return sbLeft << PrintfsW(L"%I64d", intVal);
}


}
