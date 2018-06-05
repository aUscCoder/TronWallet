// Author: miaojinquan
#pragma once
#include "../../Core/UIObject.h"
#include "../../Core/UIRichObject.h"
#include "../../UICrack.h"
#include "../UIControlDef.h"
#include "../UIControlEvent.h"
#include "UIListViewDef.h"


class CUICheckBox;
class CUIListView;
class CUIListViewItem;
class CUIListViewCheckSubItem : public CUIRichObject
{

public:

	CUIListViewCheckSubItem(CUIListView* pView, HLISTVIEWITEM hItem);
	~CUIListViewCheckSubItem();
	BEGIN_UI_EVENT_MAP(CUIListViewCheckSubItem)
		UI_REDRAW_ON_OWN_MOUSE_OR_FOCUS()
		UI_EVENT_CREATE(OnUICreate)
		UI_EVENT_SIZE(OnUISize)
		UI_EVENT_PAINT(OnUIPaint)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
		UI_EVENT_LBUTTONDOWN(OnUILButtonDown)
		UI_EVENT_LBUTTONUP(OnUILButtonUp)
		UI_EVENT_LBUTTONDBLCLK(OnUILButtonDblClk)
		UI_EVENT_RBUTTONDOWN(OnUIRButtonDown)
		UI_EVENT_RBUTTONUP(OnUIRButtonUp)
		UI_EVENT_RBUTTONDBLCLK(OnUIRButtonDblClk)
		UI_EVENT_KEYDOWN(OnUIKeyDown)
		UI_EVENT_HANDLER(UILVEvent_ItemStatusChanged, OnItemStatusChanged)
		UI_COMMAND_CODE(UIBN_CHECKCHANGE, OnCheckChanged)
	END_UI_EVENT_MAP()

	void OnUICreate();
	void OnUISize();
	void OnUIPaint(CDCHandle dc, DWORD dwStatus);
	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);

	void OnUILButtonDown(UINT nFlags, POINT point);
	void OnUILButtonUp(UINT nFlags, POINT point);
	void OnUILButtonDblClk(UINT nFlags, POINT point);
	void OnUIRButtonDown(UINT nFlags, POINT point);
	void OnUIRButtonUp(UINT nFlags, POINT point);
	void OnUIRButtonDblClk(UINT nFlags, POINT point);
	void OnUIKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	LRESULT OnItemStatusChanged(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

	LRESULT OnCheckChanged(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

	void OnFinalEvent() override { delete this; }

private:

	std::unique_ptr<CUICheckBox>		m_pCheckBox;
	HLISTVIEWITEM						m_hItem;
	CUIListView*						m_pView;
};

class CUIListViewSubItem : public CUIObject
{
public:

	explicit CUIListViewSubItem(CUIListView* pListView, CUIListViewItem* pRow);
	~CUIListViewSubItem() { }

	CUIListViewItem* GetItem() { return m_pRow; }
	void SetDisplayText(LPCWSTR lpszText);

	//////////////////////////////////////////////////////////////////////////
	// 消息响应函数
	BEGIN_UI_EVENT_MAP(CUIListViewSubItem)
		UI_EVENT_GETFONT(OnUIGetFont)
		UI_EVENT_GETTEXT(OnUIGetText)
		UI_EVENT_SETTEXT(OnUISetText)
		UI_EVENT_GETTEXTLENGTH(OnUIGetTextLength)
		UI_EVENT_PAINT(OnUIPaint)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
		UI_EVENT_HANDLER(UIEvent_LeftButtonUp, OnMouseHandler)
		UI_EVENT_HANDLER(UIEvent_LeftButtonDown, OnMouseHandler)
		UI_EVENT_HANDLER(UIEvent_LeftButtonDblClk, OnMouseHandler)
	END_UI_EVENT_MAP()

	HFONT OnUIGetFont();
	void OnUIGetText(std::wstring& strTextOut);
	void OnUISetText(const std::wstring& strText);

	int OnUIGetTextLength() { UIASSERT(FALSE && L"shit!!!"); return 0; }

	void OnUIPaint(CDCHandle dc, DWORD dwStatus);

	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);

	LRESULT OnMouseHandler(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

	virtual void OnFinalEvent() override { delete this; }

private:

	LPCWSTR				m_lpszText = nullptr;

	CUIListViewItem*	m_pRow;
	CUIListView*		m_pListView;
};

class CUIListViewItem : public CUIObject
{

public:

	explicit CUIListViewItem(CUIListView* pListView, CUIObject* pObject = nullptr);
	~CUIListViewItem() { }

	int GetHeight() const { return m_iHeight; }
	void SetHeight(const int iHeight);
	void SetSelected(const bool bSelected);
	bool IsSelected() const { return UITestStatus(UILVStatus_ItemSelected); }

	void InsertSubItem(const int iIndex);
	void RemoveSubItem(const int iIndex);

	void SetInfo(const UIListViewItem &lvItem);
	void SetData(DWORD_PTR pdwData) { m_lvItem.pdwData = pdwData; NotifyItemStatusChanged(); }
	UIListViewItem* GetInfo() { return &m_lvItem; }
	CUIObject* GetSubItem(const int iIndex);
	int GetIndex(CUIObject* pSubItem);

	bool IsChecked() { return m_lvItem.m_bChecked; }
	void SetChecked(bool bChecked);

	CRect GetItemPartRect(UIListViewItemPart ePart);

	CUIObject* AllocSubItem(const int iIndex);

	BEGIN_UI_EVENT_MAP(CUIListViewItem)
		UI_REDRAW_ON_OWN_MOUSE_OR_FOCUS()
		UI_EVENT_PREPAINT(OnUIPrePaint)
		UI_EVENT_CREATE(OnUICreate)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
		UI_EVENT_GETKIDSZORDER(OnUIGetKidsZOrder)
		UI_EVENT_LBUTTONDOWN(OnUILButtonDown)
		UI_EVENT_LBUTTONUP(OnUILButtonUp)
		UI_EVENT_LBUTTONDBLCLK(OnUILButtonDblClk)
		UI_EVENT_RBUTTONDOWN(OnUIRButtonDown)
		UI_EVENT_RBUTTONUP(OnUIRButtonUp)
		UI_EVENT_RBUTTONDBLCLK(OnUIRButtonDblClk)
		UI_EVENT_KEYDOWN(OnUIKeyDown)
		UI_EVENT_MOUSEENTER(OnUIMouseEnter)
		UI_EVENT_MOUSELEAVE(OnUIMouseLeave)
	END_UI_EVENT_MAP()

	void OnUIPrePaint();
	void OnUICreate();
	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	bool OnUIGetKidsZOrder(std::vector<CUIObject*>* pResultVec);
	void OnUILButtonDown(UINT nFlags, POINT point);
	void OnUILButtonUp(UINT nFlags, POINT point);
	void OnUILButtonDblClk(UINT nFlags, POINT point);
	void OnUIRButtonDown(UINT nFlags, POINT point);
	void OnUIRButtonUp(UINT nFlags, POINT point);
	void OnUIRButtonDblClk(UINT nFlags, POINT point);
	void OnUIKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnUIMouseEnter();
	void OnUIMouseLeave();

	virtual void OnFinalEvent() override { delete this; }
	void ReLayout();

protected:

	void EnsureSubItemAlloc();
	void EnsureSubItemCreated(const int iBegin, const int iEnd);

	void NotifyItemStatusChanged() // 通知每一个SubItem
	{
		if (UIIsCreated())
		{
			for (size_t i = 0; i < m_SubItems.size(); i++)
				m_SubItems.at(i)->UIFireEvent(UILVEvent_ItemStatusChanged, 0, 0);
		}
	}

	UIListViewItem	m_lvItem;
	bool	m_bNeedLayout;
	bool	m_bSubItemCreated;
	bool	m_bNewSelected;		// 是否是新选中的，用于处理什么时候出现编辑框
	int		m_iHeight;

	std::vector<CUIObject*> m_SubItems;
	CUIListView*	m_pListView;
	friend class CUIListView;
};
