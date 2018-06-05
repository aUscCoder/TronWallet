// Author: ChenJianjun
#pragma once
#include "../UICoreDef.h"
#include "../UIObject.h"
#include "MovePath.h"


inline bool fequal(float f1, float f2) { return fabs(f1 - f2) < 0.000001; }

class CAction;
class CAction
{
public:

	CAction();
	virtual ~CAction();

	CUIObject* GetTarget() { return m_pTarget; }

	virtual void Resume(CUIObject* pTarget) { m_pTarget = pTarget; }
	virtual void StartWithTarget(CUIObject* pTarget) { m_pTarget = pTarget; }
	virtual void Update(float fProgress/* 0 - 1.0 */) = 0;
	virtual void Stop() {}
	virtual bool DoPaint(CDCHandle dc) { return false; }

protected:

	CUIObject* m_pTarget;
};

class CMoveTo : public CAction
{
public:

	explicit CMoveTo(const se::managed_ptr<IMovePath>& path) : m_Path(path) {}
	~CMoveTo() {}

	void Init(const CPoint& ptEnd);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;

protected:

	CPoint m_ptEnd;
	CPoint m_ptStart;

	se::managed_ptr<IMovePath> m_Path;
};

class CBezierTo : public CAction
{
public:

	CBezierTo() {}
	~CBezierTo() {}

	void Init(const CPoint& ptEnd);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;

protected:

	CPoint m_ptEnd;
	CPoint m_ptStart;
};

//////////////////////////////////////////////////////////////////////////

class CWndMoveTo : public CMoveTo
{
public:

	explicit CWndMoveTo(const se::managed_ptr<IMovePath>& path) : CMoveTo(path) {}
	~CWndMoveTo() {}

	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;
};

//////////////////////////////////////////////////////////////////////////

class CRectTo : public CAction
{
public:

	CRectTo() {}
	~CRectTo() {}

	void Init(const CRect& rcEnd);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;

protected:

	CRect m_rcEnd;
	CRect m_rcStart;
};

//////////////////////////////////////////////////////////////////////////

class CSizeTo : public CAction
{
public:

	CSizeTo() {}
	~CSizeTo() {}

	void Init(const CSize& sizeEnd);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;

protected:

	CSize m_sizeEnd;
	CSize m_sizeStart;
};

//////////////////////////////////////////////////////////////////////////

class CWndSizeTo: public CRectTo
{
public:

	CWndSizeTo() {}
	~CWndSizeTo() {}

	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;
};

//////////////////////////////////////////////////////////////////////////
class CWndAlpha : public CAction
{
public:

	CWndAlpha() {}
	~CWndAlpha() {}

	void Init(BYTE cFrom, BYTE cTo);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;

protected:

	BYTE m_cFrom;
	BYTE m_cTo;
};

//////////////////////////////////////////////////////////////////////////
class CObjAlpha : public CAction
{
public:

	CObjAlpha() {}
	~CObjAlpha() {}

	void Init(BYTE cFrom, BYTE cTo);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;

protected:

	BYTE m_cFrom;
	BYTE m_cTo;
};

//////////////////////////////////////////////////////////////////////////

class CSwapBackground: public CAction
{
public:

	CSwapBackground() : m_pBegin(NULL), m_pEnd(NULL), m_fProgress(0.0f) {}
	~CSwapBackground();

	void Init(IPainter* pBegin, IPainter* pEnd);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;
	void Stop() override;
	bool DoPaint(CDCHandle dc) override;

protected:

	float m_fProgress;
	IPainter* m_pBegin;
	IPainter* m_pEnd;
};

template<typename T>
class CValueAction : public CAction
{
public:

	void Init(T tStart, T tEnd)
	{
		m_tStart	= tStart;
		m_tEnd		= tEnd;
	}

	void Update(float fProgress/* 0 - 1.0 */)
	{
		m_tValue = m_tStart + (int)(fProgress * (m_tEnd - m_tStart));
	}

	T GetValue() const { return m_tValue; }

	void Stop() {}

protected:

	T m_tValue;
	T m_tStart;
	T m_tEnd;
};

//////////////////////////////////////////////////////////////////////////

class CPaintStatus: public CAction
{
public:

	CPaintStatus() : m_dwOldStatus(0), m_dwNewStatus(0), m_fProgress(0) {}
	~CPaintStatus() {}

	void Init(DWORD dwOldStatus, DWORD dwNewStatus);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;
	void Stop() override;
	bool DoPaint(CDCHandle dc) override;

protected:

	DWORD m_dwOldStatus;
	DWORD m_dwNewStatus;
	float m_fProgress;
};


//////////////////////////////////////////////////////////////////////////

class CFuncCall: public CAction
{
public:

	CFuncCall() : m_pFuncCallTarget(NULL), m_pFunc(NULL), m_pParam(NULL) {}
	~CFuncCall() {}

	void Init(CUIObject* pFuncCallTarget, FNActionFuncCall func, LPARAM lParam);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;

private:

	CUIObject* m_pFuncCallTarget;
	FNActionFuncCall m_pFunc;
	LPARAM m_pParam;
};

//////////////////////////////////////////////////////////////////////////

class CCallback : public CAction
{
public:

	void Init(const std::function<void(float)>& func);
	void StartWithTarget(CUIObject* pTarget) override;
	void Update(float fProgress) override;

private:

	std::function<void(float)> m_func; // NOLINT (4.5.63 Lint的bug)
};
