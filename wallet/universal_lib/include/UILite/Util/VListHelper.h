// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "../Core/UICoreDef.h"
#include "../UICrack.h"


typedef std::vector<int> RowHeightVec;

#define INVALID_SEL_INDEX	-1

class CVListHelper
{
public:

	explicit CVListHelper(CUIObject* pScrollHost) : m_pScrollHost(pScrollHost) {}
	virtual ~CVListHelper() {}

	UNIVERSAL_LIB_API void VListEnsureVisible(int iItem);

	//////////////////////////////////////////////////////////////////////////

	virtual int VListGetSel() = 0;
	virtual void VListSetSel(int iItem) = 0;
	virtual int VListGetCount() = 0;
	virtual CRect VListGetItemRect(int iItem) = 0;
	virtual bool VListIsRowHeightEqual() { return false; }	// 行高是否相等
	virtual bool VListCanRowSelection() { return true; }	// 是否可以选中行

	BEGIN_UI_EVENT_MAP(CVListHelper)
		UI_EVENT_VSCROLL(OnUIVScroll)
		UI_EVENT_KEYDOWN(OnUIKeyDown)
		UI_EVENT_HANDLER(UIEvent_CalcChildrenRect, OnUICalcChildrenRect)
	END_UI_EVENT_MAP()

	UNIVERSAL_LIB_API void OnUIVScroll(UIVScrollEvent eEvent, int iDelta);
	UNIVERSAL_LIB_API void OnUIKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	UNIVERSAL_LIB_API LRESULT OnUICalcChildrenRect(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:

	int GetFirstVisibleLine();
	int GetLastVisibleLine(int iFirstVisible);
	int GetLineHeight(int iLine);
	int NormalizedScrollPos(int& iScrollPos);
	void GetScrollVisible(bool& bHScrollVisible, bool& bVScrollVisible);

	CUIObject* m_pScrollHost;
};

//////////////////////////////////////////////////////////////////////////
inline int CVListHelper::GetLineHeight(int iLine)
{
	if (iLine < 0 || iLine >= (int)VListGetCount())
		return -1;

	return VListGetItemRect(VListIsRowHeightEqual() ? 0 : iLine).Height();
}
