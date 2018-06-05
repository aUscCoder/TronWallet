// Author: wujian
#pragma once
#include <time.h>
#include "util.h"

namespace ATL
{
	class CTimeEx
	{
	public:

		enum TimeMask
		{
			YMD = 1,
			PSTART
		};
		
	public:

		static int ToDays(__time64_t t) { return int(t / (24 * 3600)); }

		//////////////////////////////////////////////////////////////////////////

		CTimeEx()
		{
			GetCurrentTime();
		}

		explicit CTimeEx(TimeMask dwMask)
		{
			if (dwMask == YMD)
			{
				SYSTEMTIME st;
				::GetLocalTime(&st);
				TmToTime(st.wYear, st.wMonth, st.wDay, 0, 0, 0);
			}
			else if (dwMask == PSTART)
			{
				FILETIME ftStart;
				::GetProcessTimes(::GetCurrentProcess(), &ftStart, NULL, NULL, NULL);
				FromFileTime(ftStart);
			}
		}

		CTimeEx(const CTimeEx &o)
		{
			m_time = o.m_time;
			memcpy(&m_tm, &o.m_tm, sizeof(tm));
		}

		explicit CTimeEx(__time64_t time) : m_time(time)
		{
			TimeToTm();
		}

		explicit CTimeEx(SYSTEMTIME st)
		{
			TmToTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}

		explicit CTimeEx(FILETIME filetime)
		{
			FromFileTime(filetime);
		}

		// 2011, 8, 28, 0, 0, 0
		CTimeEx(int year, int month, int mday, int hour, int minute, int second)
		{
			TmToTime(year, month, mday, hour, minute, second);
		}

		explicit CTimeEx(LPCWSTR lpszStringTime)		// 2011-07-29 17:17:13
		{
			ATLASSERT(lpszStringTime);
			int year(0), month(0), mday(0), hour(0), minute(0), second(0);
			swscanf_s(lpszStringTime, L"%d-%d-%d %d:%d:%d", &year, &month, &mday, &hour, &minute, &second);
			if (IsValidTM(year, month, mday, hour, minute, second))
				TmToTime(year, month, mday, hour, minute, second);
			else
				GetCurrentTime();
		}

		explicit CTimeEx(LPCSTR lpszStringTime)		// 2011-07-29 17:17:13
		{
			ATLASSERT(lpszStringTime);
			int year(0), month(0), mday(0), hour(0), minute(0), second(0);
			sscanf_s(lpszStringTime, "%d-%d-%d %d:%d:%d", &year, &month, &mday, &hour, &minute, &second);
			if (IsValidTM(year, month, mday, hour, minute, second))
				TmToTime(year, month, mday, hour, minute, second);
			else
				GetCurrentTime();
		}

		CTimeEx(LPCSTR lpszStringTime, LPCSTR lpszStringDefaultTime)
		{
			ATLASSERT(lpszStringTime && lpszStringDefaultTime);
			int year(0), month(0), mday(0), hour(0), minute(0), second(0);
			sscanf_s(lpszStringTime, "%d-%d-%d %d:%d:%d", &year, &month, &mday, &hour, &minute, &second);
			if (!IsValidTM(year, month, mday, hour, minute, second))
				sscanf_s(lpszStringDefaultTime, "%d-%d-%d %d:%d:%d", &year, &month, &mday, &hour, &minute, &second);
			TmToTime(year, month, mday, hour, minute, second);
		}

		CTimeEx(LPCWSTR lpszStringTime, LPCWSTR lpszStringDefaultTime)
		{
			ATLASSERT(lpszStringTime && lpszStringDefaultTime);
			int year(0), month(0), mday(0), hour(0), minute(0), second(0);
			swscanf_s(lpszStringTime, L"%d-%d-%d %d:%d:%d", &year, &month, &mday, &hour, &minute, &second);
			if (!IsValidTM(year, month, mday, hour, minute, second))
				swscanf_s(lpszStringDefaultTime, L"%d-%d-%d %d:%d:%d", &year, &month, &mday, &hour, &minute, &second);
			TmToTime(year, month, mday, hour, minute, second);
		}

		~CTimeEx()
		{

		}

		void GetCurrentTime()
		{
			// Return the time as seconds elapsed since midnight, January 1, 1970.
			m_time = ::_time64(NULL);
			TimeToTm();
		}

		void FromFileTime(FILETIME filetime)
		{
			FILETIME localtime;
			if (::FileTimeToLocalFileTime(&filetime, &localtime))
			{
				SYSTEMTIME st;
				if (::FileTimeToSystemTime(&localtime, &st) && st.wYear > 1900)
				{
					TmToTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
					return;
				}
			}

			ATLASSERT(0);
			GetCurrentTime();
		}

		void TmToTime(int year, int month, int mday, int hour, int minute, int second)
		{
			if (IsValidTM(year, month, mday, hour, minute, second))
			{
				memset(&m_tm, 0, sizeof(m_tm));
				m_tm.tm_year = year - 1900, m_tm.tm_mon = month - 1, m_tm.tm_mday = mday, m_tm.tm_hour = hour, m_tm.tm_min = minute, m_tm.tm_sec = second;
				m_time = ::_mktime64(&m_tm);
			}
			else
			{
				ATLASSERT(0);
				GetCurrentTime();
			}
			// 再调用一次TimeToTm是为了填充tm里的tm_wday等部分
			TimeToTm();
		}

		void TimeToTm()
		{
			errno_t t = ::_localtime64_s(&m_tm, &m_time);
			if (t)
			{
				ATLASSERT(0);
				m_time = ::_time64(NULL);
				::_localtime64_s(&m_tm, &m_time);
			}
		}

		//
		// https://msdn.microsoft.com/en-us/library/d1y53h2a.aspx
		// _mktime64 will return –1 cast to type __time64_t if timeptr references a date after 23:59:59, December 31, 3000, UTC.
		//
		bool IsValidTM(int year, int month, int mday, int hour, int minute, int second)
		{
			ATLASSERT(year < 3000);
			return (year > 1900 && year < 3000)
				&& (month >= 1 && month <= 12)
				&& (mday >= 1 && mday <= 31)
				&& (hour >= 0 && hour <= 23)
				&& (minute >= 0 && minute <= 59)
				&& (second >= 0 && second <=59);
		}

		int GetYear() const { return m_tm.tm_year + 1900; }	/* years since 1900 */
		int GetMonth() const { return m_tm.tm_mon + 1; }		/* months since January - [0,11] */
		int GetDay() const { return m_tm.tm_mday; }			/* day of the month - [1,31] */
		int GetHour() const { return m_tm.tm_hour; }		/* hours since midnight - [0,23] */
		int GetMin() const { return m_tm.tm_min; }			/* minutes after the hour - [0,59] */
		int GetSec() const { return m_tm.tm_sec; }			/* seconds after the minute - [0,59] */
		int GetDayOfWeek() const { return m_tm.tm_wday; }	/* days since Sunday - [0,6] */
		int GetDayOfYear() const { return m_tm.tm_yday; }	/* days since January 1 - [0,365] */

		LPCSTR GetMonthNameA() const
		{
			ATLASSERT(m_tm.tm_mon >= 0 && m_tm.tm_mon <= 11);
			const char * lpMonths[] = { "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };
			return lpMonths[m_tm.tm_mon];
		}

		LPCWSTR GetMonthName() const
		{
			ATLASSERT(m_tm.tm_mon >= 0 && m_tm.tm_mon <= 11);
			const wchar_t * lpMonths[] = { L"jan", L"feb", L"mar", L"apr", L"may", L"jun", L"jul", L"aug", L"sep", L"oct", L"nov", L"dec" };
			return lpMonths[m_tm.tm_mon];
		}

		LPCSTR GetDayName() const
		{
			ATLASSERT(m_tm.tm_wday >= 0 && m_tm.tm_wday <= 6);
			const char * lpWeekdayName[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
			return lpWeekdayName[m_tm.tm_wday];
		}

		__time64_t GetTime() const { return m_time; }
		void GetAsSystemTime(SYSTEMTIME &timeDest)
		{
			timeDest.wYear = (WORD)(1900 + m_tm.tm_year);
			timeDest.wMonth = (WORD)(1 + m_tm.tm_mon);
			timeDest.wDayOfWeek = (WORD)m_tm.tm_wday;
			timeDest.wDay = (WORD)m_tm.tm_mday;
			timeDest.wHour = (WORD)m_tm.tm_hour;
			timeDest.wMinute = (WORD)m_tm.tm_min;
			timeDest.wSecond = (WORD)m_tm.tm_sec;
			timeDest.wMilliseconds = 0;
		}

		std::wstring GetTimeStr() const
		{
			WCHAR szBuffer[128] = { 0 };
			swprintf_s(szBuffer, L"%04d-%02d-%02d %02d:%02d:%02d",
				m_tm.tm_year + 1900, m_tm.tm_mon + 1, m_tm.tm_mday, m_tm.tm_hour, m_tm.tm_min, m_tm.tm_sec);
			return szBuffer;
		}
		std::string GetTimeStrA() const
		{
			CHAR szBuffer[128] = { 0 };
			sprintf_s(szBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
				m_tm.tm_year + 1900, m_tm.tm_mon + 1, m_tm.tm_mday, m_tm.tm_hour, m_tm.tm_min, m_tm.tm_sec);
			return szBuffer;
		}

		std::string GetGMTStrA() const
		{
			CHAR szBuffer[128] = { 0 };
			sprintf_s(szBuffer, "%02d-%s-%02d %02d:%02d:%02d GMT",
				m_tm.tm_mday, GetMonthNameA(), m_tm.tm_year + 1900, m_tm.tm_hour, m_tm.tm_min, m_tm.tm_sec);
			return szBuffer;
		}
		std::wstring GetGMTStr() const
		{
			WCHAR szBuffer[128] = { 0 };
			swprintf_s(szBuffer, L"%02d-%s-%02d %02d:%02d:%02d GMT",
				m_tm.tm_mday, GetMonthName(), m_tm.tm_year + 1900, m_tm.tm_hour, m_tm.tm_min, m_tm.tm_sec);
			return szBuffer;
		}

		std::wstring Format(LPCWSTR lpszFormat) const
		{
			WCHAR szBuffer[128] = { 0 };
			wcsftime(szBuffer, 128, lpszFormat, &m_tm);
			return szBuffer;
		}
		std::string FormatA(LPCSTR lpszFormat) const
		{
			CHAR szBuffer[128] = { 0 };
			strftime(szBuffer, 128, lpszFormat, &m_tm);
			return szBuffer;
		}

		std::wstring GetTimeStampStr() const
		{
			se::wstringbuf sb;
			sb << m_time;
			return sb.str();
		}
		std::string GetTimeStampStrA() const
		{
			se::stringbuf sb;
			sb << m_time;
			return sb.str();
		}

		bool operator < (__int64 r) const { return m_time < r; }
		bool operator <= (__int64 r) const { return m_time <= r; }
		bool operator > (__int64 r) const { return m_time > r; }
		bool operator >= (__int64 r) const { return m_time >= r; }
		bool operator == (__int64 r) const { return m_time == r; } // NOLINT (3.3.2 老代码, yangqi, 20160511)

		bool operator < (const CTimeEx &right) const { return m_time < right.m_time; }
		bool operator <= (const CTimeEx &right) const { return m_time <= right.m_time; }
		bool operator > (const CTimeEx &right) const { return m_time > right.m_time; }
		bool operator >= (const CTimeEx &right) const { return m_time >= right.m_time; }
		bool operator == (const CTimeEx &right) const { return m_time == right.m_time; }	// NOLINT (3.3.2 真的需要……)
		friend __time64_t operator - (const CTimeEx &left, const CTimeEx &right) { return left.m_time - right.m_time; }

		inline __time64_t ToTimeSpan(int day, int hour, int minute, int second) { return second + 60 * (minute + 60 * (hour + __int64(24) * day)); }

		/* 修改当前时间 */
		void Minus(int day, int hour, int minute, int second) { m_time -= ToTimeSpan(day, hour, minute, second); TimeToTm(); }
		void Plus(int day, int hour, int minute, int second) { m_time += ToTimeSpan(day, hour, minute, second); TimeToTm(); }

		/* 获取一个多长时间之前(之后)的时间戳 */
		CTimeEx TheTimeBefore(int day, int hour, int minute, int second) { return CTimeEx(m_time - ToTimeSpan(day, hour, minute, second)); }
		CTimeEx TheTimeAfter(int day, int hour, int minute, int second) { return CTimeEx(m_time + ToTimeSpan(day, hour, minute, second)); }

		/* 是否在某个时间戳之前(之后) */
		bool Before(int year, int month, int mday, int hour, int minute, int second) { return *this < CTimeEx(year, month, mday, hour, minute, second); }
		bool After(int year, int month, int mday, int hour, int minute, int second) { return *this > CTimeEx(year, month, mday, hour, minute, second); }

		bool IsTheDay(int year, int month, int mday) { return m_tm.tm_year + 1900 == year && m_tm.tm_mon + 1 == month && m_tm.tm_mday == mday; }

	private:

		tm m_tm;
		__time64_t m_time;
	};
}