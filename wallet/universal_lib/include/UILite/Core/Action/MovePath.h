// Author: ChenJianjun
#pragma once
#include "../../Util/ManagedPtr.h"
#include "Ease.h"


struct IMovePath
{
	virtual ~IMovePath() {}
	virtual POINT GoThrough(const POINT& ptStart, const POINT& ptEnd, float fProgress) = 0;
};

struct ITransformXY
{
	virtual ~ITransformXY() {}
	virtual float Transform(float fProgress) = 0;
};

class CMovePath : public IMovePath
{
public:

	CMovePath(ITransformXY* x, bool xManaged, ITransformXY* y, bool yManaged) : m_x(x, xManaged), m_y(y, yManaged) {}
	~CMovePath() {}

	POINT GoThrough(const POINT& ptStart, const POINT& ptEnd, float fProgress) override
	{
		POINT ptNow = {
			ptStart.x + (int)((ptEnd.x - ptStart.x) * m_x->Transform(fProgress)),
			ptStart.y + (int)((ptEnd.y - ptStart.y) * m_y->Transform(fProgress))
		};

		return ptNow;
	}

private:

	se::managed_ptr<ITransformXY> m_x;
	se::managed_ptr<ITransformXY> m_y;
};

class CMovePathFunc : public IMovePath
{
public:

	CMovePathFunc(FNPathControl p, LPARAM lParam) : m_pf(p), m_lParam(lParam) {}
	~CMovePathFunc() {}

	POINT GoThrough(const POINT& ptStart, const POINT& ptEnd, float fProgress) override
	{
		return m_pf(ptStart, ptEnd, fProgress, m_lParam);
	}

private:

	FNPathControl m_pf;
	LPARAM m_lParam;
};

//////////////////////////////////////////////////////////////////////////

class CLinearPath : public ITransformXY,
	public CLinear
{
public:
	float Transform(float fProgress) override { return fProgress; }
};

template <class TTransformer>
class CSinPath : public ITransformXY,
	public CSin
{
public:
	
	explicit CSinPath(float fSep = 0.5f) : m_fSep(fSep) {}
	~CSinPath() {}

	float Transform(float fProgress) override { return TTransformer::Transform(fProgress, *this, m_fSep); }

private:
	float m_fSep;
};

template <class TTransformer>
class CPowerPath : public ITransformXY,
	public CPower
{
public:

	explicit CPowerPath(int iPow = 2, float fSep = 0.5f) : CPower(iPow), m_fSep(fSep) {}
	explicit CPowerPath(float fPow, float fSep = 0.5f) : CPower(fPow), m_fSep(fSep) {}
	~CPowerPath() {}

	float Transform(float fProgress) override { return TTransformer::Transform(fProgress, *this, m_fSep); }

private:

	float m_fSep;
};
