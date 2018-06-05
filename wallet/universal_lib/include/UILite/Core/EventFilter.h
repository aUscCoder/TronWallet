// Author: miaojinquan
#pragma once
#include "UICoreDef.h"

struct DEvent;
class CUIObject;

class CUIEventFilter
{
public:

	CUIEventFilter() {}
	UNIVERSAL_LIB_API virtual ~CUIEventFilter();

	virtual bool OnEventFilter(CUIObject* pObj, const DEvent& event) = 0;

	UNIVERSAL_LIB_API bool IsObjectContained(CUIObject* pObj);

private:

	void AddObject(CUIObject* pObj);
	void RemoveObject(CUIObject* pObj);
	
	std::list<CUIObject*> m_listObjects;

	friend class CUIObject;
};

