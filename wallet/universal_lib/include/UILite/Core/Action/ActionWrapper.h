// Author: ChenJianjun
#pragma once
#include "../UICoreDef.h"
#include "Ease.h"
#include "MovePath.h"


class CFiniteTimeDecorater;
class CAction;
class IPainter;
class CUIObject;

//////////////////////////////////////////////////////////////////////////
// 这个operator会触发一个编译器错误
// CActionSequence operator << (const CFiniteTimeDecorater* pLeft, const CFiniteTimeDecorater* pRight)
//		error C2803: 'operator <<' must have at least one formal parameter of class type
// 所以我需要把CFiniteTimeDecorater*包装成CFiniteTimeDecoraterPtr，然后实现
//	CActionSequence operator << (const CFiniteTimeDecoraterPtr pLeft, const CFiniteTimeDecoraterPtr pRight)

class CFiniteTimeDecoraterPtr
{
public:
	CFiniteTimeDecoraterPtr(const CFiniteTimeDecorater* ptr) : m_Ptr(ptr) {} // NOLINT (3.1.3 这里需要隐式的类型转换)
	~CFiniteTimeDecoraterPtr() {}

	operator CFiniteTimeDecorater*() { return (CFiniteTimeDecorater*)m_Ptr; } // NOLINT (3.3.2 需要把它当做一个指针使用)
	operator CFiniteTimeDecorater*() const { return (CFiniteTimeDecorater*)m_Ptr; } // NOLINT (3.3.2 需要把它当做一个指针使用)
	CFiniteTimeDecorater* operator->() { return (CFiniteTimeDecorater*)m_Ptr; }
	CFiniteTimeDecorater* operator->() const { return (CFiniteTimeDecorater*)m_Ptr; }

private:

	const CFiniteTimeDecorater* m_Ptr;
};

////////////////////////////////////////////////////////////////////////
class CActionDecorator
{
public:

	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr Once();
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr Linear(UINT uDuration);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr SinIn(UINT uDuration);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr SinOut(UINT uDuration);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr SinOutIn(UINT uDuration, float fSep = 0.5f);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr SinInOut(UINT uDuration, float fSep = 0.5f);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr PowerIn(UINT uDuration, int exp = 2);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr PowerOut(UINT uDuration, int exp = 2);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr PowerInOut(UINT uDuration, int exp = 2, float fSep = 0.5f);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr PowerOutIn(UINT uDuration, int exp = 2, float fSep = 0.5f);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr PowerIn(UINT uDuration, float exp);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr PowerOut(UINT uDuration, float exp);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr PowerInOut(UINT uDuration, float exp, float fSep = 0.5f);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr PowerOutIn(UINT uDuration, float exp, float fSep = 0.5f);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr OutBack(UINT uDuration, float fOvershoot = 1.70158f);
	UNIVERSAL_LIB_API CFiniteTimeDecoraterPtr Custom(UINT uDuration, IAlgorithm* pAlg, bool bAutoDelete = true);

private:

	virtual CAction* CreateAction() = 0;
};

//////////////////////////////////////////////////////////////////////////

__declspec(selectany) CLinearPath g_linear;

template <class T>
class CActionPath
{
public:

	CActionPath() : m_pPath(NULL) {}
	~CActionPath() {}

	CActionDecorator& XSinIn() { return MakePath(new CSinPath<CIn>(), true, &g_linear, false); }
	CActionDecorator& XSinOut()	{ return MakePath(new CSinPath<COut>, true, &g_linear, false); }
	CActionDecorator& XSinInOut(float fSep = 0.5f) { return MakePath(new CSinPath<CInOut>(fSep), true, &g_linear, false); }
	CActionDecorator& XSinOutIn(float fSep = 0.5f) { return MakePath(new CSinPath<COutIn>(fSep), true, &g_linear, false); }
	CActionDecorator& XPowerIn(int iPow = 2) { return MakePath(new CPowerPath<CIn>(iPow), true, &g_linear, false); }
	CActionDecorator& XPowerIn(float fPow) { return MakePath(new CPowerPath<CIn>(fPow), true, &g_linear, false); }
	CActionDecorator& XPowerOut(int iPow = 2) { return MakePath(new CPowerPath<COut>(iPow), true, &g_linear, false); }
	CActionDecorator& XPowerOut(float fPow) { return MakePath(new CPowerPath<COut>(fPow), true, &g_linear, false); }
	CActionDecorator& XPowerInOut(int iPow = 2, float fSep = 0.5f) { return MakePath(new CPowerPath<CInOut>(iPow, fSep), true, &g_linear, false); }
	CActionDecorator& XPowerInOut(float fPow, float fSep = 0.5f) { return MakePath(new CPowerPath<CInOut>(fPow, fSep), true, &g_linear, false); }
	CActionDecorator& XPowerOutIn(int iPow = 2, float fSep = 0.5f) { return MakePath(new CPowerPath<COutIn>(iPow, fSep), true, &g_linear, false); }
	CActionDecorator& XPowerOutIn(float fPow, float fSep = 0.5f) { return MakePath(new CPowerPath<COutIn>(fPow, fSep), true, &g_linear, false); }

	CActionDecorator& YSinIn() { return MakePath(&g_linear, false, new CSinPath<CIn>(), true); }
	CActionDecorator& YSinOut()	{ return MakePath(&g_linear, false, new CSinPath<COut>, true); }
	CActionDecorator& YSinInOut(float fSep = 0.5f) { return MakePath(&g_linear, false, new CSinPath<CInOut>(fSep), true); }
	CActionDecorator& YSinOutIn(float fSep = 0.5f) { return MakePath(&g_linear, false, new CSinPath<COutIn>(fSep), true); }
	CActionDecorator& YPowerIn(int iPow = 2) { return MakePath(&g_linear, false, new CPowerPath<CIn>(iPow), true); }
	CActionDecorator& YPowerIn(float fPow) { return MakePath(&g_linear, false, new CPowerPath<CIn>(fPow), true); }
	CActionDecorator& YPowerOut(int iPow = 2) { return MakePath(&g_linear, false, new CPowerPath<COut>(iPow), true); }
	CActionDecorator& YPowerOut(float fPow) { return MakePath(&g_linear, false, new CPowerPath<COut>(fPow), true); }
	CActionDecorator& YPowerInOut(int iPow = 2, float fSep = 0.5f) { return MakePath(&g_linear, false, new CPowerPath<CInOut>(iPow, fSep), true); }
	CActionDecorator& YPowerInOut(float fPow, float fSep = 0.5f) { return MakePath(&g_linear, false, new CPowerPath<CInOut>(fPow, fSep), true); }
	CActionDecorator& YPowerOutIn(int iPow = 2, float fSep = 0.5f) { return MakePath(&g_linear, false, new CPowerPath<COutIn>(iPow, fSep), true); }
	CActionDecorator& YPowerOutIn(float fPow, float fSep = 0.5f) { return MakePath(&g_linear, false, new CPowerPath<COutIn>(fPow, fSep), true); }

	CActionDecorator& Bezier();

	CActionDecorator& Path(FNPathControl pFNPathFunc, LPARAM lParam)
	{
		UIASSERT(m_pPath.get() == NULL);
		m_pPath.reset(new CMovePathFunc(pFNPathFunc, lParam), true);
		return *(T*)this;
	}

	CActionDecorator& Path(ITransformXY* pX, bool xManaged, ITransformXY* pY, bool yManaged)
	{
		return MakePath(pX, xManaged, pY, yManaged);
	}

	CActionDecorator& Path(IMovePath* pPath, bool bManaged)
	{
		m_pPath.reset(pPath, bManaged);
		return *(T*)this;
	}

protected:

	se::managed_ptr<IMovePath> GetTransform() const
	{
		if (m_pPath.get() == NULL)
		{
			static CMovePath linear(&g_linear, false, &g_linear, false);
			return se::managed_ptr<IMovePath>(&linear, false);
		}

		return m_pPath;
	}

private:

	template<class TX, class TY>
	CActionDecorator& MakePath(TX* x, bool xManaged, TY* y, bool yManaged)
	{
		UIASSERT(m_pPath.get() == NULL);
		m_pPath.reset(new CMovePath(x, xManaged, y, yManaged), true);
		return *(T*)this;
	}

	se::managed_ptr<IMovePath> m_pPath;
};

//////////////////////////////////////////////////////////////////////////
class CActionDelay
{
public:

	explicit CActionDelay(int iDelay) : m_iDelay(iDelay) {}
	~CActionDelay() {}

	UNIVERSAL_LIB_API operator CFiniteTimeDecoraterPtr();

private:

	int m_iDelay;
};

//////////////////////////////////////////////////////////////////////////
class CActionWndMoveTo : public POINT,
	public CActionDecorator,
	public CActionPath<CActionWndMoveTo>
{
public:
	CActionWndMoveTo(int _x, int _y) { x = _x; y = _y; }
	~CActionWndMoveTo() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;
};

//////////////////////////////////////////////////////////////////////////
class CActionWndSizeTo : public RECT,
	public CActionDecorator
{
public:
	CActionWndSizeTo(int _left, int _top, int _right, int _bottom) { left = _left; top = _top, right = _right, bottom = _bottom; }
	~CActionWndSizeTo() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;
};

//////////////////////////////////////////////////////////////////////////
class CActionWndAlpha : public CActionDecorator
{
public:
	CActionWndAlpha(BYTE cFrom, BYTE cTo) { m_cFrom = cFrom; m_cTo = cTo; }
	~CActionWndAlpha() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;

	BYTE m_cFrom;
	BYTE m_cTo;
};

//////////////////////////////////////////////////////////////////////////
class CActionAlpha : public CActionDecorator
{
public:
	CActionAlpha(BYTE cFrom, BYTE cTo) : m_cFrom(cFrom), m_cTo(cTo) { }
	~CActionAlpha() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;

	BYTE m_cFrom;
	BYTE m_cTo;
};

//////////////////////////////////////////////////////////////////////////
class CActionMoveTo : public POINT,
	public CActionDecorator,
	public CActionPath<CActionMoveTo>
{
public:
	CActionMoveTo(int _x, int _y) { x = _x; y = _y; }
	~CActionMoveTo() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;
};

//////////////////////////////////////////////////////////////////////////
class CActionRectTo : public RECT,
	public CActionDecorator
{
public:
	CActionRectTo(int _left, int _top, int _right, int _bottom) { left = _left; top = _top, right = _right, bottom = _bottom; }
	~CActionRectTo() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;
};

//////////////////////////////////////////////////////////////////////////
class CActionSizeTo : public SIZE,
	public CActionDecorator
{
public:
	CActionSizeTo(int _cx, int _cy) { cx = _cx; cy = _cy; }
	~CActionSizeTo() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;
};

//////////////////////////////////////////////////////////////////////////
class CActionSwapBackground : public CActionDecorator
{
public:
	CActionSwapBackground(IPainter* pBegin, IPainter* pEnd) : m_pBegin(pBegin), m_pEnd(pEnd) {}
	~CActionSwapBackground() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;

	IPainter* m_pBegin;
	IPainter* m_pEnd;
};

//////////////////////////////////////////////////////////////////////////
class CActionPaintStatus : public CActionDecorator
{
public:
	CActionPaintStatus(DWORD dwOldStatus, DWORD dwNewStatus) : m_dwOldStatus(dwOldStatus), m_dwNewStatus(dwNewStatus) {}
	~CActionPaintStatus() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;

	DWORD m_dwOldStatus;
	DWORD m_dwNewStatus;
};

//////////////////////////////////////////////////////////////////////////
class CActionFuncCall : public CActionDecorator
{
public:
	CActionFuncCall(CUIObject* pThis, FNActionFuncCall func, LPARAM lParam) : m_pThis(pThis), m_func(func), m_lParam(lParam) {}
	~CActionFuncCall() {}

private:
	UNIVERSAL_LIB_API CAction* CreateAction() override;

	CUIObject* m_pThis;
	FNActionFuncCall m_func;
	LPARAM m_lParam;
};

//////////////////////////////////////////////////////////////////////////
class CActionCallback : public CActionDecorator
{
public:
	explicit CActionCallback(const std::function<void(float fProgress)>& callback) : m_callback(callback) {}
	~CActionCallback() {}

private:

	UNIVERSAL_LIB_API CAction* CreateAction() override;

	const std::function<void(float fProgress)>& m_callback;
};

//////////////////////////////////////////////////////////////////////////
class CActionSequence
{
public:

	CActionSequence() : m_dwRepeatTimes(1) {}
	CActionSequence(const CActionSequence& that) : m_dwRepeatTimes(1)
	{
		if (this == &that)
			return;

		CActionSequence* pThat = (CActionSequence*)&that;
		m_vec = pThat->m_vec;
		m_dwRepeatTimes = pThat->m_dwRepeatTimes;
		pThat->m_vec.clear();
	};

	~CActionSequence() {}

	CActionSequence& operator << (CFiniteTimeDecorater* p)
	{
		m_vec.push_back(p);
		return *this;
	}

	CActionSequence& operator << (CFiniteTimeDecoraterPtr p)
	{
		m_vec.push_back(p);
		return *this;
	}

	CActionSequence& operator << (CActionSequence& seq)
	{
		return operator << ((CFiniteTimeDecorater*)seq);
	}

	UNIVERSAL_LIB_API operator CFiniteTimeDecorater*(); // NOLINT (3.3.2 把它传给UIRunAction，然后自动创建出CFiniteTimeDecorater*)

private:

	friend CActionSequence& operator * (CActionSequence& seq, DWORD dwTimes);
	friend CActionSequence& operator * (DWORD dwTimes, CActionSequence& seq);
	std::vector<CFiniteTimeDecorater*> m_vec;
	DWORD m_dwRepeatTimes;
};

inline CActionSequence operator << (const CFiniteTimeDecoraterPtr pLeft, const CFiniteTimeDecoraterPtr pRight)
{
	return CActionSequence() << (CFiniteTimeDecorater*)pLeft << (CFiniteTimeDecorater*)pRight;
}

inline CActionSequence& operator * (CActionSequence& seq, DWORD dwTimes) { seq.m_dwRepeatTimes *= dwTimes; return seq; }
inline CActionSequence& operator * (DWORD dwTimes, CActionSequence& seq) { seq.m_dwRepeatTimes *= dwTimes; return seq; }

class CActionParallel
{
public:

	CActionParallel& operator << (CFiniteTimeDecorater* p)
	{
		m_vec.push_back(p);
		return *this;
	}

	CActionParallel& operator << (CFiniteTimeDecoraterPtr p)
	{
		m_vec.push_back(p);
		return *this;
	}

	CActionParallel& operator << (CActionSequence& seq)
	{
		return operator << ((CFiniteTimeDecorater*)seq);
	}

	UNIVERSAL_LIB_API operator CFiniteTimeDecorater*(); // NOLINT (3.3.2 把它传给UIRunAction，然后自动创建出CFiniteTimeDecorater*)

private:

	std::vector<CFiniteTimeDecorater*> m_vec;
};