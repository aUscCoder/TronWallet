// Author: ChenJianjun
#pragma once
#include "../../CommonLib/UI/Shadow.h"
#include "../Util/VListHelper.h"
#include "../WTLWrapper.h"
#include "UIControls.h"


class CUIDropdownList;
class CUIDropdownListItem;

struct DropdownListItem
{
	std::wstring strText;
	int iImage;
	int iIndent;
	void* pData;
	CUIDropdownListItem* pUI;
};

struct DropdownListSelChanging
{
	UINH hdr;
	int iNewSel;
};

typedef DropdownListSelChanging DropdownListClose;
typedef DropdownListSelChanging DropdownListClick;

//////////////////////////////////////////////////////////////////////////

class CUIDropdownList: public CUILiteWindowImpl<CUIDropdownList>, public CShadow,
	public CVListHelper
{
public:

	explicit CUIDropdownList(CUIObject* pOwner);
	~CUIDropdownList();

	int AddItem(LPCWSTR lpszString, int iImage, int iIndent, void* pData = NULL);
	void DeleteItem(int iItem);
	void DeleteAllItems();
	int  GetDropdownHeight();
	CImageList& GetImageList() { return m_imageList; }
	void* GetItemData(int iItem) const;
	int  GetItemImage(int iItem) const;
	const std::wstring& GetItemText(int iItem) const;
	int  GetItemIndent(int iItem) const;
	int  GetListCount() const { return m_vecItems.size(); }
	int  GetMargin() const { return m_iMargin; }
	int  GetMaxHeight() const { return m_iMaxDropdownHeight; }
	int  GetSelection() const { return m_iSelection; }
	int  GetWidth() const { return m_iDropdownWidth; }
	void SetAutoDelete(bool bAutoDelete = true) { m_bAutoDelete = bAutoDelete; }
	void SetMargin(int iMargin) { m_iMargin = iMargin; }
	void SetImageList(HIMAGELIST hImageList) { m_imageList = hImageList; }
	void SetItemHeight(int iHeight) { m_iItemHeight = iHeight; }
	void SetItemBkPainter(IPainter* pPainter, bool bManaged = true);
	void SetWidth(int iWidth) { m_iDropdownWidth = iWidth; }
	void SetMaxHeight(int iHeight) { m_iMaxDropdownHeight = iHeight; }
	void SetInitialSelection(int iSelection) { SetNewSel(iSelection); }

	bool ShowDropdownList(const RECT& rcPos);
	void HideDropdownList();

	bool OnMessageHook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

	BEGIN_MSG_MAP(CUIDropdownList)
		CHAIN_MSG_MAP(CShadow)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		CHAIN_MSG_MAP(CUILiteWindowImpl<CUIDropdownList>)
	END_MSG_MAP()
	DECLARE_WND_CLASS_EX(DROPDOWNLIST_CLASS_NAME, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_WINDOW)

	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHideDropdownList(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnHookMouseMessage(HWND hWnd, UINT uMsg, POINT pt, WORD wHitTest);
	void OnFinalMessage(HWND);

	BEGIN_UI_EVENT_MAP(CUIDropdownList)
		CHAIN_UI_EVENT_MAP(CVListHelper)
		UI_EVENT_CREATE(OnUICreate)
		UI_EVENT_DESTROY(OnUIDestroy)
		UI_EVENT_PREPAINT(OnUIPrePaint)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
		UI_EVENT_PAINT(OnUIPaint)
		UI_EVENT_SIZE(OnUISize)
		UI_EVENT_HITTEST(OnUIHitTest)
	END_UI_EVENT_MAP()

	void OnUICreate();
	void OnUIDestroy();
	void OnUIPrePaint();
	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnUIPaint(CDCHandle dc, DWORD dwStatus);
	void OnUISize();
	LRESULT OnUIHitTest(POINT point);

private:

	// CVListScrollHelper
	int VListGetSel() override { return m_iSelection; }
	void VListSetSel(int iItem) override { SetNewSel(iItem); }
	int VListGetCount() override { return m_vecItems.size(); }
	CRect VListGetItemRect(int iItem) override;
	bool VListIsRowHeightEqual() override { return true; }

	friend class CUIDropdownListItem;

	static HHOOK sm_hMouseHook;
	static CUIDropdownList* sm_pCurrentDropdown;
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	void DrawItemBackground(CDCHandle dc, DWORD dwStatus, CUIDropdownListItem* pItem);
	void DrawItem(CDCHandle dc, DWORD dwStatus, CUIDropdownListItem* pItem);

	void UpdateLayout();
	void OnEndSize(float fProgress, LPARAM lParam);
	LRESULT FireCommand(CUIObject* p, UINH* pHDR);
	void EnsureSelVisible();
	void ClickItem(int iItem);
	void SetNewSel(int iNewSel);

	CUIObject* m_pOwner;
	CImageList m_imageList;

	int m_iDropdownWidth;
	int m_iMaxDropdownHeight;
	int m_iMargin;
	int m_iItemHeight;

	IPainter* m_pItemBkPainter;
	bool m_bItemBkPainterManaged;

	HWND m_hWndForground;
	int m_iSelection;
	bool m_bAutoDelete;
	std::vector<DropdownListItem*> m_vecItems;
};

//////////////////////////////////////////////////////////////////////////

class CUIDropdownListItem : public CUIObject
{
public:

	explicit CUIDropdownListItem(CUIDropdownList* pList);
	~CUIDropdownListItem() {}

	void SetItem(int iItem) { m_iItem = iItem; }
	int GetItem() { return m_iItem; }

	BEGIN_UI_EVENT_MAP(CUIDropdownListItem)
		UI_EVENT_GETTEXT(OnUIGetText)
		UI_EVENT_SETTEXT(OnUISetText)
		UI_EVENT_GETTEXTLENGTH(OnUIGetTextLength)
		UI_EVENT_PAINTBACKGROUND(OnPaintBackground)
		UI_EVENT_PAINT(OnPaint)
		UI_EVENT_LBUTTONUP(OnLButtonUp)
		UI_EVENT_MOUSEENTER(OnMouseEnter)
		UI_EVENT_MOUSELEAVE(OnMouseLeave)
	END_UI_EVENT_MAP()

	int OnUIGetTextLength() { return (UINT)m_pDropdownList->GetItemText(m_iItem).length(); }
	void OnUIGetText(std::wstring& strTextOut) { strTextOut = m_pDropdownList->GetItemText(m_iItem); }
	void OnUISetText(const std::wstring& strText) { UIASSERT(false); }
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	void OnLButtonUp(UINT nFlags, POINT point);
	void OnMouseEnter();
	void OnMouseLeave();

private:

	CUIDropdownList* m_pDropdownList;
	int m_iItem;
};

//////////////////////////////////////////////////////////////////////////

inline void CUIDropdownList::SetItemBkPainter(IPainter* pPainter, bool bManaged)
{
	if (m_pItemBkPainter && m_bItemBkPainterManaged)
		delete m_pItemBkPainter;

	m_pItemBkPainter = pPainter;
	m_bItemBkPainterManaged = bManaged;
}

