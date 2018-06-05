// NOLINT (all)
// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "../String/StringHelper.h"
#include "../CommonLibMsg.h"


#define PostTuoTask(wnd, func, ...)			PostTuoTaskImpl(wnd, this, &func, __VA_ARGS__)
#define SendNotifyTuoTask(wnd, func, ...)	SendNotifyTuoTaskImpl(wnd, this, &func, __VA_ARGS__)
#define SendTuoTask(wnd, func, ...)			SendTuoTaskImpl(wnd, this, &func, __VA_ARGS__)
#define SendTuoTaskNonBlock(wnd, func, ...) SendTuoTaskNonBlockImpl(wnd, this, &func, __VA_ARGS__)
#define SendTuoTaskNonBlockAndReturn(wnd, func, ...) return SendTuoTaskNonBlockAndReturnImpl<decltype(func(__VA_ARGS__))>(wnd, this, &func, __VA_ARGS__)
#define SendTuoTaskAndReturn(wnd, func, ...) return SendTuoTaskAndReturnImpl<decltype(func(__VA_ARGS__))>(wnd, this, &func, __VA_ARGS__)

#define PostStaticTuoTask(wnd, func, ...) PostStaticTuoTaskImpl(wnd, func, __VA_ARGS__)
#define SendStaticTuoTask(wnd, func, ...) SendStaticTuoTaskImpl(wnd, func, __VA_ARGS__)
#define SendNotifyStaticTuoTask(wnd, func, ...) SendNotifyStaticTuoTaskImpl(wnd, func, __VA_ARGS__)
#define SendStaticTuoTaskAndReturn(wnd, func, ...) return SendStaticTuoTaskAndReturnImpl<decltype(func(__VA_ARGS__))>(wnd, func, __VA_ARGS__)
#define SendStaticTuoTaskNonBlockAndReturn(wnd, func, ...) \
	return SendStaticTuoTaskNonBlockAndReturnImpl<decltype(func(__VA_ARGS__))>(wnd, func, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

template<typename ThisType, typename FuncType, typename... types>
BOOL PostTuoTaskImpl(HWND hWnd, ThisType pThis, FuncType pFunc, types... args)
{
	std::function<void()>* p = new std::function<void()>(std::bind(pFunc, pThis, args...));
	if (::PostMessage(hWnd, WM_UTIL_POST_TUO_TASK, TRUE, (LPARAM)p))
		return TRUE;

	delete p;
	return FALSE;
}

template<typename ThisType, typename FuncType, typename... types>
LRESULT SendTuoTaskImpl(HWND hWnd, ThisType pThis, FuncType pFunc, types... args)
{
	std::function<void()> f = std::function<void()>(std::bind(pFunc, pThis, args...));
	DWORD dwRes = 0;
	return ::SendMessageTimeout(hWnd, WM_UTIL_POST_TUO_TASK, 0, (LPARAM)&f, SMTO_BLOCK | SMTO_NOTIMEOUTIFNOTHUNG, 0, &dwRes);
}

template<typename ThisType, typename FuncType, typename... types>
LRESULT SendTuoTaskNonBlockImpl(HWND hWnd, ThisType pThis, FuncType pFunc, types... args)
{
	std::function<void()> f = std::function<void()>(std::bind(pFunc, pThis, args...));
	DWORD dwRes = 0;
	return ::SendMessageTimeout(hWnd, WM_UTIL_POST_TUO_TASK, 0, (LPARAM)&f, SMTO_NOTIMEOUTIFNOTHUNG, 0, &dwRes);
}

template<typename ThisType, typename FuncType, typename... types>
BOOL SendNotifyTuoTaskImpl(HWND hWnd, ThisType pThis, FuncType pFunc, types... args)
{
	std::function<void()>* p = new std::function<void()>(std::bind(pFunc, pThis, args...));
	if (::SendNotifyMessage(hWnd, WM_UTIL_POST_TUO_TASK, TRUE, (LPARAM)p))
		return TRUE;

	delete p;
	return FALSE;
}

template<typename ReturnType, typename ThisType, typename FuncType, typename... types>
ReturnType SendTuoTaskAndReturnImpl(HWND hWnd, ThisType pThis, FuncType pFunc, types... args)
{
	decltype((pThis->*pFunc)(args...)) ret;
	std::function<void()> f = std::function<void()>([&ret, pThis, pFunc, args...]()
	{
		ret = (pThis->*pFunc)(args...);
	});

	DWORD dwRes = 0;
	::SendMessageTimeout(hWnd, WM_UTIL_POST_TUO_TASK, 0, (LPARAM)&f, SMTO_BLOCK | SMTO_NOTIMEOUTIFNOTHUNG, 0, &dwRes);

	return ret;
}

template<typename ReturnType, typename ThisType, typename FuncType, typename... types>
ReturnType SendTuoTaskNonBlockAndReturnImpl(HWND hWnd, ThisType pThis, FuncType pFunc, types... args)
{
	decltype((pThis->*pFunc)(args...)) ret;
	std::function<void()> f = std::function<void()>([&ret, pThis, pFunc, args...]()
	{
		ret = (pThis->*pFunc)(args...);
	});

	DWORD dwRes = 0;
	::SendMessageTimeout(hWnd, WM_UTIL_POST_TUO_TASK, 0, (LPARAM)&f, SMTO_NOTIMEOUTIFNOTHUNG, 0, &dwRes);

	return ret;
}

template<typename FuncType, typename... types>
BOOL PostStaticTuoTaskImpl(HWND hWnd, FuncType pFunc, types... args)
{
	std::function<void()>* p = new std::function<void()>(std::bind(pFunc, args...));
	if (::PostMessage(hWnd, WM_UTIL_POST_TUO_TASK, TRUE, (LPARAM)p))
		return TRUE;

	delete p;
	return FALSE;
}

template<typename FuncType, typename... types>
LRESULT SendStaticTuoTaskImpl(HWND hWnd, FuncType pFunc, types... args)
{
	std::function<void()> f = std::function<void()>(std::bind(pFunc, args...));
	DWORD dwRes = 0;
	return ::SendMessageTimeout(hWnd, WM_UTIL_POST_TUO_TASK, 0, (LPARAM)&f, SMTO_BLOCK | SMTO_NOTIMEOUTIFNOTHUNG, 0, &dwRes);
}

template<typename FuncType, typename... types>
BOOL SendNotifyStaticTuoTaskImpl(HWND hWnd, FuncType pFunc, types... args)
{
	std::function<void()>* p = new std::function<void()>(std::bind(pFunc, args...));
	if (::SendNotifyMessage(hWnd, WM_UTIL_POST_TUO_TASK, TRUE, (LPARAM)p))
		return TRUE;

	delete p;
	return FALSE;
}

template<typename ReturnType, typename FuncType, typename... types>
ReturnType SendStaticTuoTaskAndReturnImpl(HWND hWnd, FuncType pFunc, types... args)
{
	ReturnType ret;
	std::function<void()> f = std::function<void()>([&ret, pFunc, args...]()
	{
		ret = pFunc(args...);
	});

	DWORD dwRes = 0;
	::SendMessageTimeout(hWnd, WM_UTIL_POST_TUO_TASK, 0, (LPARAM)&f, SMTO_BLOCK | SMTO_NOTIMEOUTIFNOTHUNG, 0, &dwRes);

	return ret;
}

template<typename ReturnType, typename FuncType, typename... types>
ReturnType SendStaticTuoTaskNonBlockAndReturnImpl(HWND hWnd, FuncType pFunc, types... args)
{
	ReturnType ret;
	std::function<void()> f = std::function<void()>([&ret, pFunc, args...]()
	{
		ret = pFunc(args...);
	});

	DWORD dwRes = 0;
	::SendMessageTimeout(hWnd, WM_UTIL_POST_TUO_TASK, 0, (LPARAM)&f, SMTO_NOTIMEOUTIFNOTHUNG, 0, &dwRes);

	return ret;
}

//////////////////////////////////////////////////////////////////////////

#define ON_TUO_TASK_MSG() \
	if (uMsg == WM_UTIL_POST_TUO_TASK) \
	{ \
		std::function<void()>* pFunc = (std::function<void()>*)lParam; \
		(*pFunc)(); \
		if (wParam) \
			delete pFunc; \
		return TRUE; \
	}

