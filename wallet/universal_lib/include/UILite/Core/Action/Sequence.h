// Author: ChenJianjun
#pragma once
#include "Interval.h"


//////////////////////////////////////////////////////////////////////////

class CSequence: public CFiniteTimeDecorater
{
public:

	CSequence();
	~CSequence();

	bool InitWithAction(CFiniteTimeDecorater* pAction1, CFiniteTimeDecorater* pAction2);
	void Start(CUIObject* pTarget) override;
	int Step(int iDelta) override;
	void Stop() override;
	bool IsDone() override;

private:

	CUIObject* m_pTarget;
	CFiniteTimeDecorater* m_pAction[2];
	int m_iElapsed;
	int m_iCurrentAction;
};

//////////////////////////////////////////////////////////////////////////
class CSequenceList: public CFiniteTimeDecorater
{
public:
	CSequenceList();
	~CSequenceList();

	bool InitWithAction(const std::vector<CFiniteTimeDecorater*>& actions);
	void Start(CUIObject* pTarget) override;
	int Step(int iDelta) override;
	void SetDirection(bool bForword) override;
	void Stop() override;
	bool IsDone() override;
	int GetElapsed() const { return m_iElapsed; }

private:

	CUIObject* m_pTarget;
	std::vector<CFiniteTimeDecorater*> m_Actions;
	int m_iElapsed;
	int m_iCurrentAction;
};

//////////////////////////////////////////////////////////////////////////

class CRepeat: public CFiniteTimeDecorater
{
public:

	CRepeat() : m_dwRepeatTimes(0), m_dwCurrent(0), m_pTarget(NULL) {}
	~CRepeat() {}

	bool InitWithAction(CFiniteTimeDecorater* pAction, DWORD iTimes);

	void Start(CUIObject* pTarget) override;
	int Step(int iDelta) override;
	bool IsDone() override;
	void Stop() override;

protected:

	CUIObject* m_pTarget;
	DWORD m_dwRepeatTimes;
	DWORD m_dwCurrent;
};
