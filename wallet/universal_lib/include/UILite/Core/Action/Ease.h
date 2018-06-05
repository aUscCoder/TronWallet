// Author: KeJinjin
#pragma once
#include "Interval.h"


template<class TTransformer, class TAlgorithm>
class CEaseAdapter : public CInterval
{
public:

	CEaseAdapter(UINT uDuration, CAction* pAction, const TAlgorithm& alg = TAlgorithm(), float fSep = 0.0f) : m_alg(alg), m_fInOutSep(fSep)
	{
		InitWithDuration(uDuration, pAction);
	}

	~CEaseAdapter() {}

	void Update(float fProgress) override
	{
		m_pAction->Update(TTransformer::Transform(fProgress, m_alg, m_fInOutSep));
	}

private:

	TAlgorithm m_alg;
	float	m_fInOutSep;
};

//////////////////////////////////////////////////////////////////////////

template<class TAlgorithm>
class CEaseAdapterNoTransformer : public CInterval
{
public:

	CEaseAdapterNoTransformer(UINT uDuration, CAction* pAction, const TAlgorithm& alg = TAlgorithm(), float fSep = 0.0f) : m_alg(alg), m_fInOutSep(fSep)
	{
		InitWithDuration(uDuration, pAction);
	}

	~CEaseAdapterNoTransformer() {}

	void Update(float fProgress) override
	{
		m_pAction->Update(m_alg.Calc(fProgress));
	}

private:

	TAlgorithm m_alg;
	float	m_fInOutSep;
};

class IAlgorithm
{
public:

	virtual float Calc(float fVal) = 0;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Algorithm

class CLinear : public IAlgorithm
{
public:

	float Calc(float fVal) override { return fVal; }
};

//////////////////////////////////////////////////////////////////////////

class CSin : public IAlgorithm
{
public:

	float Calc(float fVal) override;
};

//////////////////////////////////////////////////////////////////////////


class CPower : public IAlgorithm
{
public:

	explicit CPower(int iPow) : m_iPow(iPow), m_bFloat(false) {}
	explicit CPower(float fPow) : m_fPow(fPow), m_bFloat(true) {}
	~CPower() {}

	float Calc(float fVal) override;

private:

	int m_iPow;
	float m_fPow;
	bool m_bFloat;
};


//////////////////////////////////////////////////////////////////////////
/*
向外凸起，值会超过1.0, fOvershoot=1.70158时将会超过最大值的大约10%
*/

class COutBack : public IAlgorithm
{
public:

	explicit COutBack(float fOvershoot = 1.70158) : m_fOvershoot(fOvershoot) {}
	~COutBack() {}

	float Calc(float fVal) override;

private:

	float m_fOvershoot;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Transformer

class CIn
{
public:
	static float Transform(float fVal, IAlgorithm& alg, float fSep) { return alg.Calc(fVal); }
};

//////////////////////////////////////////////////////////////////////////

class COut
{
public:
	static float Transform(float fVal, IAlgorithm& alg, float fSep) { return 1.0f - alg.Calc(1.0f - fVal); }
};

//////////////////////////////////////////////////////////////////////////

class CInOut
{
public:

	static float Transform(float fVal, IAlgorithm& alg, float fSep)
	{
		if (fVal <= fSep)
			return alg.Calc(fVal / fSep) * fSep;
		else
			return 1.0f - alg.Calc((1.0f - fVal) / (1.0f - fSep)) * (1.0f - fSep);
	}
};

//////////////////////////////////////////////////////////////////////////

class COutIn
{
public:

	static float Transform(float fVal, IAlgorithm& alg, float fSep)
	{
		if (fVal <= fSep)
			return fSep - alg.Calc((fSep - fVal) / fSep) * fSep;
		else
			return fSep + alg.Calc((fVal - fSep) / (1.0f - fSep)) * (1.0f - fSep);
	}
};