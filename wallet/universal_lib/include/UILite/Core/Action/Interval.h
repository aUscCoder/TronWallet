// Author: ChenJianjun
#pragma once
#include "Decorator.h"


//////////////////////////////////////////////////////////////////////////

class CInterval: public CFiniteTimeDecorater
{
public:

	CInterval() : m_uElapsed(0) {}
	~CInterval() {}

	bool InitWithDuration(UINT uDuration, CAction* pAction);
	UINT GetElapsed() const { return m_uElapsed; }
	void Resume(CUIObject* pTarget) override;
	void Start(CUIObject *pTarget) override;
	int Step(int iDelta) override;
	virtual void Update(float fProgress) = 0;
	bool IsDone(void) override;

protected:

	UINT m_uElapsed;
};

//////////////////////////////////////////////////////////////////////////

class CDelay: public CInterval
{
public:

	CDelay() {}
	~CDelay() {}

	void Start(CUIObject *pTarget) override;
	void Update(float fProgress) override {  }
	bool IsDone() override;
	void Stop() override { m_bStop = true; }

};

//////////////////////////////////////////////////////////////////////////

class COnce: public CInterval
{
public:

	COnce() {}
	~COnce() {}

	int Step(int iDelta) override;
	void Update(float fProgress) override;
	bool IsDone() { return true; }
};

class IAlgorithm;
class CEaseAdapterCustom : public CInterval
{
public:

	CEaseAdapterCustom(UINT uDuration, CAction* pAction, IAlgorithm* alg, bool bAutoDelete = true);

	~CEaseAdapterCustom();

	void Update(float fProgress) override;
	void SetCurTime(UINT uCurTime);

private:

	IAlgorithm* m_pAlgorithm;
	bool		m_bAutoDelete;
};
