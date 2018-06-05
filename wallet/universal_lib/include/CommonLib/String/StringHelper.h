// Author: Chenjianjun & WuJian
#pragma once
#include "../../Universal.h"


#define BLANKS						" \r\n\t"
#define WBLANKS						L" \r\n\t"
#define BLANKS_WITH_QUOTATION		L" \r\n\t\"\'"
#define BLANKS_SLASH				L"\\"

#define  LPSZ(str) ((str).c_str())

#ifndef StrVec
typedef std::vector<std::wstring> StrVec;
#endif

#ifndef StrVecA
typedef std::vector<std::string> StrVecA;
#endif

#ifndef StrList
typedef std::list<std::wstring> StrList;
typedef std::list<std::string> StrListA;
#endif

template<typename t_char>
struct STRINGT
{
	const t_char* lpsz;
	int len;
};

typedef STRINGT<char> StringA;
typedef STRINGT<wchar_t> StringW;

UNIVERSAL_LIB_API char *stristr(const char *s1, const char *s2);
UNIVERSAL_LIB_API wchar_t* wcsistr(const wchar_t* s1, const wchar_t* s2);

UNIVERSAL_LIB_API int SplitStringA(LPCSTR lpszStr, std::vector<StringA>& v, char chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringA(LPCSTR lpszStr, StrVecA& v, char chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringA(LPCSTR lpszStr, StrListA& v, char chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringA(LPCSTR lpszStr, int iStrLen/* lpszStr的长度, -1表示一直到\0结束 */, std::vector<StringA>& v, char chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringA(LPCSTR lpszStr, int iStrLen/* lpszStr的长度, -1表示一直到\0结束 */, StrVecA& v, char chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringA(LPCSTR lpszStr, int iStrLen/* lpszStr的长度, -1表示一直到\0结束 */, StrListA& v, char chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringA(LPCSTR s, StrVecA& v, const std::string& strSeparator);
UNIVERSAL_LIB_API int SplitStringA(LPCSTR s, std::vector<StringA>& v, const std::string& strSeparator);

UNIVERSAL_LIB_API int SplitStringW(LPCWSTR lpszStr, std::vector<StringW>& v, wchar_t chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringW(LPCWSTR lpszStr, StrList& v, wchar_t chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringW(LPCWSTR lpszStr, StrVec& v, wchar_t chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringW(LPCWSTR lpszStr, int iStrLen/* lpszStr的长度, -1表示一直到\0结束 */, std::vector<StringW>& v, wchar_t chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringW(LPCWSTR lpszStr, int iStrLen/* lpszStr的长度, -1表示一直到\0结束 */, StrList& v, wchar_t chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringW(LPCWSTR lpszStr, int iStrLen/* lpszStr的长度, -1表示一直到\0结束 */, StrVec& v, wchar_t chSeparator = '\n', size_t iMaxCount = 0x7fffffff);
UNIVERSAL_LIB_API int SplitStringW(LPCWSTR s, StrVec& v, const std::wstring& strSeparator);
UNIVERSAL_LIB_API int SplitStringW(LPCWSTR s, std::vector<StringW>& v, const std::wstring& strSeparator);

UNIVERSAL_LIB_API void ReplaceAllDistinctW(std::wstring& str, const std::wstring& strOld, const std::wstring& strNew);
UNIVERSAL_LIB_API void ReplaceAllDistinctA(std::string& str, const std::string& strOld, const std::string& strNew);
inline void ReplaceString(std::wstring& str, const std::wstring& strOld, const std::wstring& strNew) { ReplaceAllDistinctW(str, strOld, strNew); }
inline void ReplaceString(std::string& str, const std::string& strOld, const std::string& strNew) { ReplaceAllDistinctA(str, strOld, strNew); }

UNIVERSAL_LIB_API void TrimString(std::wstring& str, LPCWSTR lpszBlankStr = WBLANKS);
UNIVERSAL_LIB_API void TrimString(std::string& str, LPCSTR lpszBlankStr = BLANKS);
UNIVERSAL_LIB_API std::string TrimA(const std::string& s, const std::string & t = BLANKS);
UNIVERSAL_LIB_API std::wstring TrimW(const std::wstring& s, const std::wstring & t = WBLANKS);
UNIVERSAL_LIB_API std::string RTrimA(const std::string& s, const std::string& t = BLANKS);
UNIVERSAL_LIB_API std::wstring RTrimW(const std::wstring& s, const std::wstring& t = WBLANKS);

UNIVERSAL_LIB_API std::string FormatString(LPCSTR lpszFormat, ...);
UNIVERSAL_LIB_API std::wstring FormatString(LPCWSTR lpszFormat, ...);


inline int ToLowerASCII(int c) { return (c >= 'A' && c <= 'Z') ? (c + ('a' - 'A')) : c; }
inline int ToUpperASCII(int c) { return (c >= 'a' && c <= 'z') ? (c - ('a' - 'A')) : c; }

inline std::string ToLower(const std::string& s)
{
	std::string d(s);
	std::transform(d.begin(), d.end(), d.begin(), ToLowerASCII);
	return d;
}

inline std::wstring ToLower(const std::wstring& s)
{
	std::wstring d(s);
	std::transform(d.begin(), d.end(), d.begin(), ToLowerASCII);
	return d;
}

inline std::string ToUpper(const std::string& s)
{
	std::string d(s);
	std::transform(d.begin(), d.end(), d.begin(), ToUpperASCII);
	return d;
}

inline std::wstring ToUpper(const std::wstring& s)
{
	std::wstring d(s);
	std::transform(d.begin(), d.end(), d.begin(), ToUpperASCII);
	return d;
}

inline void ToLowerSrc(std::string& d) { std::transform(d.begin(), d.end(), d.begin(), ToLowerASCII); }
inline void ToLowerSrc(std::wstring& d) { std::transform(d.begin(), d.end(), d.begin(), ToLowerASCII); }
inline void ToUpperSrc(std::string& d) { std::transform(d.begin(), d.end(), d.begin(), ToUpperASCII); }
inline void ToUpperSrc(std::wstring & d) { std::transform(d.begin(), d.end(), d.begin(), ToUpperASCII); }

inline std::wstring itos(long l)
{
	wchar_t s[32];
	_itow_s(l, s, 30, 10);
	return s;
}

inline std::string itosA(long l)
{
	char s[32];
	_itoa_s(l, s, 30, 10);
	return s;
}

//
// WuJian 写一个函数，封装string.join这件事
// 老代码
// se::wstringbuf sb;
// sb << L"part1" << L"part2" << L"part3"
// std::wstring str = sb.str()
//
// 新代码
// std::wstring str = StringJoin(L"part1", L"part2", L"part3");
//

template<typename STREAM, typename PART>
void StringJoiner(STREAM &s, const PART &t)
{
	s << t;
}

template<typename STREAM, typename PART, typename ... Args>
void StringJoiner(STREAM &s, const PART &first, const Args& ... args)
{
	s << first;
	StringJoiner(s, args...);
}

template<typename char_type, typename ... Args>
std::basic_string<char_type, std::char_traits<char_type>, std::allocator<char_type>> JoinString(Args ...args)
{
	se::basic_stringBuffer<char_type> sb;
	StringJoiner(sb, args...);
	//
	// [WuJian]
	// std::wstring str = JoinStringW(L"", L"");
	// 这里是一个move constrcuctor，不会发生二次copy
	//
	return sb.str();
}

template<typename ... Args>
std::wstring JoinStringW(const Args& ... args)
{
	return JoinString<wchar_t>(std::forward<const Args&>(args)...);
}

template<typename ... Args>
std::string JoinStringA(const Args& ...args)
{
	return JoinString<char>(std::forward<const Args&>(args)...);
}

template<typename STREAM, typename CONNECTOR, typename PART, typename ... Args>
void PathJoiner(STREAM &s, const CONNECTOR &c, const PART &t)
{
	s << c << t;
}

template<typename STREAM, typename CONNECTOR, typename PART, typename ... Args>
void PathJoiner(STREAM &s, const CONNECTOR &c, const PART &FIRST, const Args& ... args)
{
	s << c << FIRST;
	PathJoiner(s, c, args...);
}

template<typename STREAM, typename CONNECTOR, typename PART, typename ... Args>
void PathJoinStep1(STREAM &s, const CONNECTOR &c, const PART &FIRST, const Args& ... args)
{
	s << FIRST;
	PathJoiner(s, c, args...);
}


template<typename char_type, typename CONNECTOR, typename ... Args>
std::basic_string<char_type, std::char_traits<char_type>, std::allocator<char_type>> JoinPath(const CONNECTOR &c, const Args& ... args)
{
	se::basic_stringBuffer<char_type> sb;
	PathJoinStep1(sb, c, args...);
	return sb.str();
}

template<typename CONNECTOR, typename ... Args>
std::wstring JoinPathW(const CONNECTOR &c, const Args& ... args)
{
	return JoinPath<wchar_t>(c, std::forward<const Args&>(args)...);
}

template<typename CONNECTOR, typename ... Args>
std::string JoinPathA(const CONNECTOR &c, const Args& ... args)
{
	return JoinPath<char>(c, std::forward<const Args&>(args)...);
}


//////////////////////////////////////////////////////////////////////////
namespace StringHelper
{
	template <typename char_type>
	class CStartsWithT
	{
	public:
		explicit CStartsWithT(const char_type* pStr, const char_type* pStart) : m_pStr(pStr), m_pStart(pStart)
		{
		}

		~CStartsWithT()
		{
		}

		bool Compare(bool bIgnoreCase, size_t nLength) const
		{
			TuoAssert(m_pStr && m_pStart);
			TuoAssert(nLength == size_t(-1) || nLength <= strlenT(m_pStart));

			const size_t len = nLength == size_t(-1) ? strlenT(m_pStart) : nLength;
			if (len > strlenT(m_pStr))
				return false;

			const int nCmp = bIgnoreCase ? strnicmpT(m_pStr, m_pStart, len) : strncmpT(m_pStr, m_pStart, len);
			return nCmp == 0;
		}

	private:
		size_t strlenT(const char* pStr) const
		{
			return strlen(pStr);
		}
		size_t strlenT(const wchar_t* pStr) const
		{
			return wcslen(pStr);
		}

		int strncmpT(const char* first, const char* last, size_t count) const
		{
			return strncmp(first, last, count);
		}
		int strncmpT(const wchar_t* first, const wchar_t* last, size_t count) const
		{
			return wcsncmp(first, last, count);
		}

		int strnicmpT(const char* first, const char* last, size_t count) const
		{
			return _strnicmp(first, last, count);
		}
		int strnicmpT(const wchar_t* first, const wchar_t* last, size_t count) const
		{
			return _wcsnicmp(first, last, count);
		}

	private:
		const char_type* m_pStr = nullptr;
		const char_type* m_pStart = nullptr;
	};

	template<typename char_type>
	bool StartsWithImpl(const char_type* pStr, const char_type* pStart, size_t nLength, bool bIgnoreCase)
	{
		CStartsWithT<char_type> startsWith(pStr, pStart);
		return startsWith.Compare(bIgnoreCase, nLength);
	}
} // namespace StringHelper

//
// 判断 pStr 是否以 pStart 开头，区分大小写
// nLength 表示要比较的字符个数， -1 表示比较整个 pStart
//
template<typename char_type>
bool StartsWith(const char_type* pStr, const char_type* pStart, size_t nLength)
{
	return StringHelper::StartsWithImpl(pStr, pStart, nLength, false);
}

//
// 判断 pStr 是否以 pStart 开头，忽略大小写
// nLength 表示要比较的字符个数， -1 表示比较整个 pStart
//
template<typename char_type>
bool StartsWithI(const char_type* pStr, const char_type* pStart, size_t nLength)
{
	return StringHelper::StartsWithImpl(pStr, pStart, nLength, true);
}

//
// 判断 str 是否以 start 开头，区分大小写
//
template < typename char_type>
bool StartsWith(const std::basic_string<char_type, std::char_traits<char_type>, std::allocator<char_type>> & str,
	const std::basic_string<char_type, std::char_traits<char_type>, std::allocator<char_type>> & start)
{
	return StartsWith(str.c_str(), start.c_str(), start.length());
}

//
// 判断 str 是否以 start 开头，忽略大小写
//
template < typename char_type>
bool StartsWithI(const std::basic_string<char_type, std::char_traits<char_type>, std::allocator<char_type>> & str,
	const std::basic_string<char_type, std::char_traits<char_type>, std::allocator<char_type>> & start)
{
	return StartsWithI(str.c_str(), start.c_str(), start.length());
}
