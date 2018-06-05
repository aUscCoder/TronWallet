// NOLINT (all)
// Author: TuotuoXP
#pragma once
#include "../../Universal.h"
#include "DataSerializer.h"


/*

[TuotuoXP] 这个文件是用来帮助大家跨进程调用一个内嵌函数的，使用起来很方便，具体方法如下：


1. 定义&序列化函数

// 这个结构体是目标进程在调用这个函数时传入的额外参数（即来自于目标进程的数据）
struct TestData
{
	int aaa;
	int bbb;
};

void Test()
{
	// 函数的定义可以写在函数体里面，方便阅读，当然写成全局的也可以
	struct A
	{
		TestData test;		// 在目标进程中的数据
		void func(int iFromPID, LPCWSTR lpszInfo, LPCWSTR lpszExtra)		// 这个函数会运行在目标进程里面
		{
			// 注意，这个函数的参数都只能写简单类型，不能写std::string之类的
			// 这里可以访问到test
		}
	};

	// 序列化函数的第一个参数是内嵌类的名字，第二个参数是内嵌类里成员函数的名字，后面的参数依次为成员函数的参数
	UINT uID = SerializeFunc(A, func, GetCurrentProcessId(), L"test abc", std::wstring(L"abc"));

	// 得到id之后可以将其发送到其他进程了
	::PostMessage(wnd, msg, uID, 0);
}


2. 在目标进程中将序列化之后的函数运行

void RunFunc(UINT uFuncID)
{
	// 在目标进程中可以添加额外数据
	TestData test = { 100, 200 };
	// 直接运行即可，非常方便
	UnserializeFunc(wParam, &test);
}


3. Done！跨进程调用函数就是这么cool！今天你调用了没？


*/


//////////////////////////////////////////////////////////////////////////


#define SerializeFuncEx(c, cf, ...)			SerializeFuncV<c>(sizeof(c), &cf, __VA_ARGS__)
#define SerializeFunc(c, f, ...)			SerializeFuncV<c>(sizeof(c), &c::f, __VA_ARGS__)
#define UnserializeFunc(id, add)			Universal::CrossProcessFunc::UnserializeFunction(id, add, sizeof(*add))
#define UnserializeFuncN(id)				Universal::CrossProcessFunc::UnserializeFunction(id, NULL, 0)


namespace Universal
{
	namespace CrossProcessFunc
	{
		UNIVERSAL_LIB_API UINT SerializeFunction(void *ppFunctionCall, CDataCollector &col, int iAdditionalDataSize);
		UNIVERSAL_LIB_API void UnserializeFunction(UINT uFunctionID, void *pAdditionalData, int iAdditionalDataSize);
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


#define SerializeFuncToWnd(wnd, msg, c, f, ...) \
	do \
	{ \
		int iFuncID = SerializeFuncV<c>(sizeof(c), &c::f, __VA_ARGS__); \
		::PostMessage(wnd, msg, iFuncID, 0); \
	} while (0)

#define SerializeFuncToWndEx(wnd, msg, c, cf, ...) \
	do \
	{ \
		int iFuncID = SerializeFuncV<c>(sizeof(c), &cf, __VA_ARGS__); \
		::PostMessage(wnd, msg, iFuncID, 0); \
	} while (0)


#define ON_TUO_CROSS_PROCESS_FUNC_MSG(msg) \
	if (uMsg == msg) \
	{ \
		UnserializeFunc(wParam, this); \
		return TRUE; \
	}


//////////////////////////////////////////////////////////////////////////

inline int __Checker(int) { return 100; }
inline unsigned int __Checker(unsigned int) { return 100; }
inline long __Checker(long) { return 100; }
inline unsigned long __Checker(unsigned long) { return 100; }
inline HWND __Checker(HWND) { return NULL; }
inline HANDLE __Checker(HANDLE) { return NULL; }
inline LPCWSTR __Checker(LPCWSTR) { return L""; }
inline LPCWSTR __Checker(const std::wstring &) { return L""; }
inline LPCWSTR __Checker(const se::wstringbuf &) { return L""; }
inline LPCSTR __Checker(LPCSTR) { return ""; }
inline LPCSTR __Checker(const std::string &) { return ""; }
inline LPCSTR __Checker(const se::stringbuf &) { return ""; }
inline const _D& __Checker(const _D &d) { return d; }


template<typename C, typename Functor>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p)
{
	if (0) { C *pC = NULL; (pC->*p)(); }
	CrossProcessDataCollector col;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0)); }
	CrossProcessDataCollector col;
	col << a0;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1)); }
	CrossProcessDataCollector col;
	col << a0 << a1;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1, typename Type2>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1, Type2 a2)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1), __Checker(a2)); }
	CrossProcessDataCollector col;
	col << a0 << a1 << a2;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1, typename Type2, typename Type3>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1, Type2 a2, Type3 a3)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1), __Checker(a2), __Checker(a3)); }
	CrossProcessDataCollector col;
	col << a0 << a1 << a2 << a3;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1, typename Type2, typename Type3, typename Type4>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1, Type2 a2, Type3 a3, Type4 a4)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1), __Checker(a2), __Checker(a3), __Checker(a4)); }
	CrossProcessDataCollector col;
	col << a0 << a1 << a2 << a3 << a4;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1, typename Type2, typename Type3, typename Type4, typename Type5>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1, Type2 a2, Type3 a3, Type4 a4, Type5 a5)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1), __Checker(a2), __Checker(a3), __Checker(a4), __Checker(a5)); }
	CrossProcessDataCollector col;
	col << a0 << a1 << a2 << a3 << a4 << a5;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1, Type2 a2, Type3 a3, Type4 a4, Type5 a5, Type5 a6)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1), __Checker(a2), __Checker(a3), __Checker(a4), __Checker(a5), __Checker(a6)); }
	CrossProcessDataCollector col;
	col << a0 << a1 << a2 << a3 << a4 << a5 << a6;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6, typename Type7>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1, Type2 a2, Type3 a3, Type4 a4, Type5 a5, Type6 a6, Type7 a7)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1), __Checker(a2), __Checker(a3), __Checker(a4), __Checker(a5), __Checker(a6), __Checker(a7)); }
	CrossProcessDataCollector col;
	col << a0 << a1 << a2 << a3 << a4 << a5 << a6 << a7;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6, typename Type7, typename Type8>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1, Type2 a2, Type3 a3, Type4 a4, Type5 a5, Type6 a6, Type7 a7, Type8 a8)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1), __Checker(a2), __Checker(a3), __Checker(a4), __Checker(a5), __Checker(a6), __Checker(a7), __Checker(a8)); }
	CrossProcessDataCollector col;
	col << a0 << a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6, typename Type7, typename Type8, typename Type9>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1, Type2 a2, Type3 a3, Type4 a4, Type5 a5, Type6 a6, Type7 a7, Type8 a8, Type9 a9)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1), __Checker(a2), __Checker(a3), __Checker(a4), __Checker(a5), __Checker(a6), __Checker(a7), __Checker(a8), __Checker(a9)); }
	CrossProcessDataCollector col;
	col << a0 << a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8 << a9;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}

template<typename C, typename Functor, typename Type0, typename Type1, typename Type2, typename Type3, typename Type4, typename Type5, typename Type6, typename Type7, typename Type8, typename Type9, typename Type10>
inline UINT SerializeFuncV(int iAddtionalDataSize, Functor p, Type0 a0, Type1 a1, Type2 a2, Type3 a3, Type4 a4, Type5 a5, Type6 a6, Type7 a7, Type8 a8, Type9 a9, Type10 a10)
{
	if (0) { C *pC = NULL; (pC->*p)(__Checker(a0), __Checker(a1), __Checker(a2), __Checker(a3), __Checker(a4), __Checker(a5), __Checker(a6), __Checker(a7), __Checker(a8), __Checker(a9), __Checker(a10)); }
	CrossProcessDataCollector col;
	col << a0 << a1 << a2 << a3 << a4 << a5 << a6 << a7 << a8 << a9 << a10;
	return Universal::CrossProcessFunc::SerializeFunction(&p, col, iAddtionalDataSize);
}
