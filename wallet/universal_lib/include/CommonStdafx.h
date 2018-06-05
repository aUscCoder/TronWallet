// NOLINT (all)
// Author: TuotuoXP
#pragma once


#define _SCL_SECURE_NO_DEPRECATE


// windows headers
#include <WinSock2.h>
#include <windows.h>
#include <commctrl.h>
#include <comutil.h>
#include <WinInet.h>
#include <GdiPlus.h>
#include <uxtheme.h>
#include <Psapi.h>
#include <Tlhelp32.h>
#include <WinCred.h>

// atl headers
#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>
#include <atlwinex.h>
#include <atlwinx.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <atldlgs.h>
#include <atlcoll.h>
#include <atlcrack.h>
#include <atlddx.h>
#include <atlgdix.h>
#include <atltime.h>
#include <atlctrlx.h>
#include <atlrx.h>
#define _WTL_NO_CSTRING
#include <atlmisc.h>

// undef minmax
// atl 和 GDI+ 里用到了 min/max，所以只能在它们后面再干掉

#ifdef NOMINMAX
#undef NOMINMAX
#endif
#ifdef max
#  undef max
#endif
#ifdef min
#  undef min
#endif
#define NOMINMAX


// stl headers
#include <map>
#include <vector>
#include <list>
#include <set>
#include <unordered_map>
#include <stack>
#include <algorithm>
#include <string>
#include <queue>
#include <iomanip>
#include <cmath>
#include <iterator>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <array>

#include <tuofunc.h>
#include <tuover.h>
#include <util.h>

// _DEBUG 下，禁掉几个 W4 的警告
#ifdef _DEBUG

#pragma warning(disable: 4100) // 'identifier' : unreferenced formal parameter

// 所有结构体的定义都会产生下面两个警告，先禁掉
#pragma warning(disable: 4510) // default constructor could not be generated
#pragma warning(disable: 4610) // object 'class' can never be instantiated - user - defined constructor required

//
// 先禁掉，最后再统一处理，C4238、4239
//
// v8::Handle<v8::Function> funcCallback = v8Util::CastToFunction(args[0]);
// warning C4239: nonstandard extension used : 'argument' : conversion from 'v8::Local<v8::Value>' to 'v8::Handle<v8::Value> &'
//
// sl.Select(SRANKTBL.id).Where(SRANKTBL.id == serverID).ExecSelect()
// warning C4239: nonstandard extension used : 'argument' : conversion from 'CConditionTree' to 'CConditionTree &'
//
#pragma warning(disable: 4239)
//
// warning C4238: nonstandard extension used : class rvalue used as lvalue
// funcCallback->Call(Context()->Global(), 1, &v8::Handle<v8::Value>(objRet));
//
#pragma warning(disable: 4238)
//
// warning C4838: conversion from 'int' to 'UINT' requires a narrowing conversion
// ATL library
#pragma warning(disable: 4302)
//
// warning C4302: 'type cast': truncation from 'LPCTSTR' to 'WORD'
// ATL library
#pragma warning(disable: 4838)
//
// 对于利用初始化列表展开编参模板的临时变量，都会产生 warning C4189，可以利用 _se_unused_ 宏消灭警告
// warning C4189 : local variable is initialized but not referenced
//
#define _se_unused_(x) ((void)x)

#else

#define _se_unused_(x) __noop

#endif // _DEBUG
