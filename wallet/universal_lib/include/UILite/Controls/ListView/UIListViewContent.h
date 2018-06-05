// Author: miaojinquan
#pragma once
#include "../../Core/UIRichObject.h"
#include "UIListViewDef.h"


class CUIListViewItem;
class CUIListView;
typedef std::vector<CUIListViewItem*> CUIListViewItemArray;
typedef bool(*ItemCompareFunc)(const LVCompareContext &lvCompareContext);
class CUIListViewContent : public CUIRichObject
{
public:

	explicit CUIListViewContent(CUIListView* pListView) : m_pListView(pListView) {}
	~CUIListViewContent() { }

	void ReLayoutIfNeed();

	int GetItemCount() const;
	void InsertItem(const int iRow, const bool bShowIcon = false, HICON hIcon = NULL, DWORD_PTR pdwData = NULL);
	void RemoveItem(const int iRow);
	void Clear();
	void InsertColumn(const int iCol);
	void RemoveColumn(const int iCol);

	CRect GetItemArea();

	void GetVisibleRange(int &iBegin, int &iEnd);
	CUIObject* GetSubItem(const int iRow, const int iCol);
	CUIListViewItem* GetItem(const int iRow) { TuoAssert(TestIndexValid(iRow)); return m_Items.at(iRow); }
	UNIVERSAL_LIB_API int GetItemIndex(CUIListViewItem* pItem);
	int GetFocusIndex() { return GetItemIndex(m_pFocusedItem); }
	bool TestIndexValid(const int iRow) { return iRow >= 0 && iRow < (int)m_Items.size(); }
	void Sort(const int iIndex, const bool bAscending, ItemCompareFunc func, DWORD_PTR pdwData);

	void SetItemBKPainter(IPainter* pPainter) { m_pItemBKPainter = pPainter; }
	IPainter* GetItemBKPainter() { return m_pItemBKPainter; }

	void OnUIPrePaint();
	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnUISize();
	void OnUIDestory();

	LRESULT OnUIHScroll(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnHeaderLayoutChanged(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnItemHeightChanged(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	bool OnUIGetKidsZOrder(std::vector<CUIObject*>* pResultVec);
	bool OnUIMeasureClient(SIZE& sz);
	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

protected:

	void	SetLayoutRequest();

	CRect	m_rcItemArea;
	bool	m_bNeedLayout		= true;
	bool	m_bVisibleChanged	= true;
	using ListItemPair = std::pair<size_t, CUIListViewItem *>;
	
	CUIListViewItem*			m_pFocusedItem = nullptr;
	CUIListView*				m_pListView = nullptr;
	IPainter*					m_pItemBKPainter = nullptr;
	std::set<CUIListViewItem*>	m_SelectedItems;
	CUIListViewItemArray		m_Items;
	friend class CUIListView;
};
