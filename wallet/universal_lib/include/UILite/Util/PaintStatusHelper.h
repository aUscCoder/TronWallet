// Author: ChenJianjun
#pragma once
#include "../Core/UICoreDef.h"
#include "../Core/UICoreEvent.h"
#include "../TextLayout.h"
#include "../UICrack.h"


#define PAINT_STATUS_DURATION	300

class CUIObject;
class CPaintStatusHelper
{
public:

	explicit CPaintStatusHelper(CUIObject* pOwner,
		DWORD dwStatusMask = UIStatus_Hover | UIStatus_Pressed | UIStatus_Focus | UIStatus_Disabled | UIStatus_HoverChild,
		DWORD dwDuration = PAINT_STATUS_DURATION)
		: m_pOwner(pOwner), m_dwDuration(dwDuration), m_dwStatusMask(dwStatusMask)
	{
	
	}

	~CPaintStatusHelper()
	{
	
	}

	BEGIN_UI_EVENT_MAP(CPaintStatusHelper)
		UI_EVENT_HANDLER(UIEvent_CombinedStatusChanged, OnUIStatusChanged)
	END_UI_EVENT_MAP()

	UNIVERSAL_LIB_API LRESULT OnUIStatusChanged(int, WPARAM wParam, LPARAM lParam, bool&);
	void SetStatusChangeDuration(DWORD dwDuration) { m_dwDuration = dwDuration; }

private:

	CUIObject* m_pOwner;
	DWORD m_dwStatusMask;
	DWORD m_dwDuration;
};
