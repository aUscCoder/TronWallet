// Author: miaojinquan
#pragma once
#include "Decorator.h"


// 并行动效

class CParallelList : public CFiniteTimeDecorater
{
public:

	CParallelList();
	~CParallelList();

	bool InitWithAction(const std::vector<CFiniteTimeDecorater*>& actions);
	void Start(CUIObject* pTarget) override;
	int Step(int iDelta) override;
	void Stop() override;
	bool IsDone() override;

private:

	CUIObject* m_pTarget;
	std::vector<CFiniteTimeDecorater*> m_Actions;
	int m_iElapsed;
	int m_iCurrentAction;
};
