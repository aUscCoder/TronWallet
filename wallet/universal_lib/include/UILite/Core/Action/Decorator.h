// Author: ChenJianjun
#pragma once
#include "../UICoreDef.h"


#define LONG_CRITICAL_DURATION	3000

class CAction;
class CUIObject;
class CFiniteTimeDecorater;

class CDecorator
{
public:

	CDecorator();
	virtual ~CDecorator();

	bool InitWithAction(CAction* pAction);
	bool InitWithAction(CDecorator* pAction);
	void SetAutoDelete(bool bAutoDelete = true) { m_bAutoDelete = bAutoDelete; }
	bool IsAutoDelete() const { return m_bAutoDelete; }
	virtual void SetDirection(bool bForword) { m_bDirectForword = bForword; }
	bool IsDirForword() { return m_bDirectForword; }

	void SetGroupID(int nID) { m_nGroupID = nID; }
	int GetGroupID() const { return m_nGroupID; }

	virtual void Start(CUIObject* pTarget);
	virtual void Resume(CUIObject* pTarget);
	virtual int Step(int iDelta) = 0;
	virtual bool IsDone() { return !m_pAction || m_bStop; }
	virtual void Stop();

protected:

	virtual bool IsLongDuration() { return false; }

	union
	{
		CAction*	m_pAction;
		CDecorator* m_pDecorator;
	};

	int  m_nGroupID;
	bool m_bDecorator;
	bool m_bStop;
	bool m_bDirectForword;
	bool m_bAutoDelete;
};

//////////////////////////////////////////////////////////////////////////

class CFiniteTimeDecorater : public CDecorator
{
public:

	CFiniteTimeDecorater() : m_uDuration(0) {}
	virtual ~CFiniteTimeDecorater() {}

	inline UINT GetDuration(void) { return m_uDuration; }
	inline void SetDuration(UINT uDuration) { m_uDuration = std::max<UINT>(uDuration, 0); }

protected:
	virtual bool IsLongDuration() { return m_uDuration > LONG_CRITICAL_DURATION; }

	UINT m_uDuration; // ms
};
