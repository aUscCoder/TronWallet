// Author: miaojinquan
#pragma once

#include "../../../Universal.h"
#include "../../Util/VListHelper.h"
#include "../UIButton.h"
#include "../UIControls.h"
#include "../UIEdit.h"
#include "../UIScrollArea.h"
#include "UIListViewContent.h"
#include "UIListViewHeader.h"
#include "UIListViewItem.h"


__interface IUIScrollbar; // NOLINT (4.5.2 lint bug, 识别不了 __interface)
class CUIListViewItem;
class CUIListView;


typedef DWORD_PTR HLISTVIEWCOLUMN;

class CUIListViewport : public CUIObject, public CVListHelper
{
public:

	CUIListViewport(CUIListView* pListView, CUIListViewContent* pContent) : CVListHelper(this), m_pListView(pListView), m_pContent(pContent) {}
	~CUIListViewport() {}

	LRESULT OnCalcChildrenRect(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	UNIVERSAL_LIB_API int	VListGetSel() override;
	UNIVERSAL_LIB_API void	VListSetSel(int iItem) override;
	int						VListGetCount() override { return m_pContent->GetItemCount(); }
	UNIVERSAL_LIB_API CRect	VListGetItemRect(int iItem) override;

private:

	CUIListView*		m_pListView;
	CUIListViewContent* m_pContent;

};

class CUIListViewSubItemCreator
{

public:

	CUIListViewSubItemCreator() { }
	virtual ~CUIListViewSubItemCreator() { }

	UNIVERSAL_LIB_API virtual CUIObject* OnCreateObject(CUIListView* pView, HLISTVIEWITEM hItem, const int iCol);
};

class CUIListView : public CUIScrollArea
{

public:

	UNIVERSAL_LIB_API explicit CUIListView(IUIScrollbar* pHScrollBar = nullptr, IUIScrollbar* pVScrollBar = nullptr);
	~CUIListView() { SAFE_DELETE_PTR(m_pCreator); SAFE_DELETE_PTR(m_pHeaderSectionPainter); }

	void SetHeaderHeight(const int iHeight) { m_iHeaderHeight = iHeight; Layout(); }
	int	 GetHeaderHeight() const { return m_iHeaderHeight; }

	// 用在不想让列表第一列紧挨着Header的情况
	void SetContentPadding(const int iPadding) { m_iContentPadding = iPadding; SetContentMargin(0, m_iHeaderHeight + iPadding, 0, 0); }

	void SetHeaderSectionPainter(IPainter* pPainter) { TuoAssert(!UIIsCreated() && L"创建之前调用！"); m_pHeaderSectionPainter = pPainter; }
	IPainter* GetHeaderSectionPainter() { return m_pHeaderSectionPainter; }

	void SetHeaderBKPainter(IPainter* pBKPainter, bool bAutoDelete) { m_Header.UISetBkPainter(pBKPainter, bAutoDelete); }
	void SetContentBKPainter(IPainter* pBKPainter, bool bAutoDelete) { m_Content.UISetBkPainter(pBKPainter, bAutoDelete); }
	void SetItemBKPainter(IPainter*	pBKPainter) { m_Content.SetItemBKPainter(pBKPainter); }

	int GetHeaderValidWidth() { return m_Header.GetValidClientWidth(); }
	int GetFocusItem() { return m_Content.GetFocusIndex(); }

	HLISTVIEWITEM GetItem(const int iRow) { return (HLISTVIEWITEM)m_Content.GetItem(iRow); }
	UIListViewColumn* GetColumnData(const int iIndex) { return m_Header.GetSection(iIndex)->GetInfo(); }
	UIListViewColumn* GetColumnData(HLISTVIEWCOLUMN hCol) { return ((CUIListViewHeaderSection*)hCol)->GetInfo(); }

	// 在堆上创建，这里会自动delete
	void SetSubItemCreator(CUIListViewSubItemCreator* pSubItemCreator)
	{
		TuoAssert(!UIIsCreated() && L"创建之前调用啊亲！");
		if (m_pCreator)
			delete m_pCreator;

		m_pCreator = pSubItemCreator;
	}

	CUIListViewSubItemCreator* GetSubItemCreator() { return m_pCreator; }

	UNIVERSAL_LIB_API void SetFocusItem(const int iIndex);
	UNIVERSAL_LIB_API void InsertColumn(const int iIndex, const UIListViewColumn &lvc);
	UNIVERSAL_LIB_API void InsertColumn(const int iIndex, const LPCWSTR lpszText,
		const int iWidth = 10, UIListViewItemTextAlign eTextpos = UILVIAlign_Left);
	UNIVERSAL_LIB_API void RemoveColumn(const int iIndex);
	UNIVERSAL_LIB_API void SetColumn(const int iIndex, const UIListViewColumn &lvc);
	UNIVERSAL_LIB_API void SetItemDisabled(const int iIndex, bool bDisabled = true);

	UNIVERSAL_LIB_API void InsertItem(const int iRow, bool bShowIcon = false, HICON hIcon = NULL, DWORD_PTR pdwData = NULL);
	UNIVERSAL_LIB_API void RemoveItem(const int iRow);
	UNIVERSAL_LIB_API void RemoveItem(HLISTVIEWITEM hListViewItem);
	UNIVERSAL_LIB_API void Clear();

	UNIVERSAL_LIB_API void SetItemText(const int iRow, const int iCol, LPCWSTR lpszText);
	UNIVERSAL_LIB_API void SetItem(const int iRow, const UIListViewItem &lvItem);
	UNIVERSAL_LIB_API void SetItemChecked(const int iRow, bool bChecked);
	UNIVERSAL_LIB_API bool IsItemChecked(const int iRow);

	void SetItemData(const int iRow, DWORD_PTR dwData);
	void SetItemData(HLISTVIEWITEM hItem, DWORD_PTR dwData);
	DWORD_PTR GetItemData(const int iRow) { TuoAssert(iRow >= 0 && iRow < GetItemCount()); return m_Content.m_Items[iRow]->m_lvItem.pdwData; }
	DWORD_PTR GetItemData(HLISTVIEWITEM hItem) { TuoAssert(hItem); return ((CUIListViewItem*)hItem)->m_lvItem.pdwData; }

	UIListViewItem* GetItemInfo(const int iRow) { TuoAssert(iRow >= 0 && iRow < GetItemCount()); return &m_Content.m_Items[iRow]->m_lvItem; }
	UIListViewItem* GetItemInfo(HLISTVIEWITEM hItem) { return ((CUIListViewItem*)hItem)->GetInfo(); }

	UNIVERSAL_LIB_API int GetItemIndex(HLISTVIEWITEM hItem);
	UNIVERSAL_LIB_API std::wstring GetItemText(const int iRow, const int iCol);
	UNIVERSAL_LIB_API std::wstring GetItemText(HLISTVIEWITEM hItem, const int iCol);

	bool IsFocused() { return UIIsFocused() || UIIsFocusedChild(); }

	UNIVERSAL_LIB_API bool SetItemSelected(const int iRow, bool bSelect);
	UNIVERSAL_LIB_API bool IsSelected(const int iRow);
	UNIVERSAL_LIB_API bool IsSelected(HLISTVIEWITEM hItem);
	UNIVERSAL_LIB_API std::vector<int> GetSelectedItems();
	UNIVERSAL_LIB_API void ClearSelection();

	UNIVERSAL_LIB_API void GetVisibleRange(int &iBegin, int &iEnd);
	UNIVERSAL_LIB_API void GetVisibleColumn(int &iBegin, int &iEnd);
	UNIVERSAL_LIB_API int GetItemCount() const;
	UNIVERSAL_LIB_API int GetColumnCount() const;

	UNIVERSAL_LIB_API CRect GetItemRect(const int iRow);
	UNIVERSAL_LIB_API CRect GetItemRect(HLISTVIEWITEM hItem);
	UNIVERSAL_LIB_API CRect GetSubItemRect(const int iRow, const int iCol);

	UNIVERSAL_LIB_API bool EditItem(const int iRow);
	UNIVERSAL_LIB_API bool EndEdit(bool bCancel);
	UNIVERSAL_LIB_API void EnsureVisible(const int iRow);

	UNIVERSAL_LIB_API void Sort(const int iSubIndex, const bool bAscending = true, ItemCompareFunc func = NULL, DWORD_PTR pdwData = NULL);
	UNIVERSAL_LIB_API void SetItemHeight(const int iRow, const int iHeight);
	UNIVERSAL_LIB_API CUIObject* GetSubItem(const int iRow, const int iCol);

	void OnUICreate();
	void OnUISize();
	void OnUIKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnUITimer(UINT_PTR nIDEvent, LPARAM lParam);
	void OnUISetFont();
	void OnUIPrePaint();
	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnUIStyleChanged(DWORD dwOldStyle);
	bool OnUIMeasureChild(CUIObject* pChild, SIZE& sz);
	LRESULT OnItemLButtonDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnItemLButtonUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnItemLButonDblClk(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnItemRButtonDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnItemRButtonUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnHeadLayoutChanged(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnEditBoxKeyDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnEditBoxKillFocus(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnEditBoxTextChange(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnSelectionChanged(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

protected:
	
	UNIVERSAL_LIB_API void Layout() override;
	UNIVERSAL_LIB_API void OnViewportScrollChanged(const DEvent& event) override;

	bool SetItemSelected(CUIListViewItem* pItem, bool bSelect, bool bFireEvent = true);
	void SelectItem(CUIListViewItem* pItem, bool bClear);
	int  GetItemIndex(CUIListViewItem* pItem);
	CUIListViewHeaderSection* GetColumn(const int iIndex);
	UNIVERSAL_LIB_API bool TestIndexValid(const int iRow, const int iCol);
	void OnVisibleColumnChanged(const int iBegin, const int iEnd);
	
	void ChangeToSimpleList();

	CUIListViewHeader	m_Header;
	CUIListViewContent	m_Content;
	CUIEditBox*			m_pEdit;
	bool				m_bItemLDBClick;
	CUIListViewSubItemCreator*	m_pCreator;
	IPainter*			m_pHeaderSectionPainter;

	bool				m_bNeedLayout;
	int					m_iHeaderHeight;
	int					m_iContentPadding;	// Content 中第一行可以有空白
	friend class CUIListViewItem;
	friend class CUIListViewSubItem;
	friend class CUIListViewContent;
	friend class CUIListViewHeader;
};

inline void CUIListView::SetItemData(const int iRow, DWORD_PTR dwData)
{
	TuoAssert(iRow >= 0 && iRow < GetItemCount());
	m_Content.m_Items[iRow]->SetData(dwData);
}

inline void CUIListView::SetItemData(HLISTVIEWITEM hItem, DWORD_PTR dwData)
{
	CUIListViewItem *pItem = (CUIListViewItem*)hItem;
	pItem->SetData(dwData);
}
