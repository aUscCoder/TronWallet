// Author: ChenJianjun
#pragma once
#include "ActionContext.h"


class CDecorator;
class CUIObject;
class CUIManager;
class CActionManager
{
public:

	explicit CActionManager(CUIManager* pUIMgr);
	~CActionManager();

	void RunAction(CUIObject* pTarget, CDecorator* pAction, IContextCallback* pContextDestroyCallback, int nGroup, bool bRestart = true);
	void StopAction(CUIObject* pTarget, int nGroup);
	void StopAction(CUIObject* pTarget);
	bool IsEmpty() { return m_ActionTargetMap.empty(); }

	bool Update(int iDelta);

	void OnTimer();
	void StartActionTimer();
	void StopActionTimer();

private:

	typedef std::set<CDecorator*> ActionSet;
	typedef std::list<std::pair<CDecorator*, IContextCallback*>> ActionList;
	typedef std::map<CUIObject*, ActionList> ActionListMap;
	typedef std::map<CUIObject*, ActionSet> ActionSetMap;

	ActionSetMap m_StoppedSet;
	ActionListMap m_ActionTargetMap;

	__int64 m_i64LastActionTime;

	CUIManager* m_pUIMgr;
	CUIObject* m_pCurrentTarget;
	CDecorator* m_pCurrentAction;
	bool m_bBusy;
};

