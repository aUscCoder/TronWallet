// Author: ChenJianjun
#pragma once
#include <functional>
#include "../../../Universal.h"
#include "../UICoreDef.h"
#include "ActionContext.h"
#include "ActionWrapper.h"
#include "Decorator.h"


class CAction;
class CActionWndMoveTo;
namespace Action
{
	inline CActionDelay Delay(int uDuration) { return CActionDelay(uDuration); }
	inline CActionWndMoveTo WndMoveTo(int x, int y) { return CActionWndMoveTo(x, y); }
	inline CActionWndMoveTo WndMoveTo(const POINT& pt) { return CActionWndMoveTo(pt.x, pt.y); }
	inline CActionWndSizeTo WndSizeTo(int l, int t, int r, int b) { return CActionWndSizeTo(l, t, r, b); }
	inline CActionWndSizeTo WndSizeTo(const RECT& rc) { return CActionWndSizeTo(rc.left, rc.top, rc.right, rc.bottom); }
	inline CActionWndAlpha WndAlpha(BYTE cFrom, BYTE cTo) { return CActionWndAlpha(cFrom, cTo); }
	inline CActionAlpha ObjAlpha(BYTE cFrom, BYTE cTo) { return CActionAlpha(cFrom, cTo); }
	inline CActionMoveTo MoveTo(int x, int y) { return CActionMoveTo(x, y); }
	inline CActionMoveTo MoveTo(const POINT& pt) { return CActionMoveTo(pt.x, pt.y); }
	inline CActionRectTo RectTo(int l, int t, int r, int b) { return CActionRectTo(l, t, r, b); }
	inline CActionRectTo RectTo(const RECT& rc) { return CActionRectTo(rc.left, rc.top, rc.right, rc.bottom); }
	inline CActionSizeTo SizeTo(int cx, int cy) { return CActionSizeTo(cx, cy); }
	inline CActionSizeTo SizeTo(const SIZE& sz) { return CActionSizeTo(sz.cx, sz.cy); }
	inline CActionSwapBackground SwapBackground(IPainter* pBegin, IPainter* pEnd) { return CActionSwapBackground(pBegin, pEnd); }
	inline CActionPaintStatus PaintStatus(DWORD dwOldStatus, DWORD dwNewStatus) { return CActionPaintStatus(dwOldStatus, dwNewStatus); }
	inline CActionFuncCall FuncCall(CUIObject* pThis, FNActionFuncCall func, LPARAM lParam) { return CActionFuncCall(pThis, func, lParam); }
	inline CActionCallback Callback(const std::function<void(float fProgress)>& callback) { return CActionCallback(callback); }

	inline CActionSequence Sequence() { return CActionSequence(); }
	inline CActionParallel Parallel() { return CActionParallel(); }
	UNIVERSAL_LIB_API IContextCallback* OnContextDestroy(ContextCallback pCallback, LPARAM lParam);
}

using namespace Action; // NOLINT (2.1.1 为了方便使用，我主动忽略这个名字空间)
