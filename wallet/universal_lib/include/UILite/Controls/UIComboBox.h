// Author: ChenJianjun
#pragma once
#include "../../CommonLib/UI/Shadow.h"
#include "../../Universal.h"
#include "../Core/UIRoot.h"
#include "../Util/VListHelper.h"
#include "../WTLWrapper.h"
#include "UIButton.h"
#include "UIControlDef.h"
#include "UIEdit.h"


#define CB_STATUS_CHANGED_ACTION_GROUPID	-3
#define CB_DROPDOWN_ACTION_GROUPID			-4

#define COMBOBOX_DROPDOWNLIST_DEFAULT_INDENT	-1024

class CUIComboBox;
class CUIComboBoxDropDownListItem;

struct UIComboBoxSelChanging
{
	UINH hdr;
	int iNewSel;
};

typedef UIComboBoxSelChanging UIComboBoxSelEndOk;
typedef UIComboBoxSelChanging UIComboBoxSelEndCancel;

struct _UICBXItemData
{
	int iImage;
	std::wstring strText;
	int iIndent;

	CUIComboBoxDropDownListItem* pUI;
};

class CUIComboBoxDropDownList: public CUILiteWindowImpl<CUIComboBoxDropDownList>, public CShadow, public CVListHelper
{
public:

	explicit CUIComboBoxDropDownList(CUIComboBox* pComboBox);
	~CUIComboBoxDropDownList();

	int AddString(LPCWSTR lpszString, int iImage, int iIndent);
	int DeleteItem(int iItem);
	void DeleteAllItems();
	void SetImageList(HIMAGELIST hImageList) { m_imageList = hImageList; }
	HIMAGELIST GetImageList() { return m_imageList; }
	int GetDropDownHeight();
	int GetListCount() const { return m_vecItems.size(); }
	_UICBXItemData* GetItemData(int iItem);
	void SetSelEndOk(bool bSelEndOk) { m_bSelEndOk = bSelEndOk; }

	void ShowDropDownList();
	void HideDropDownList();

	BEGIN_MSG_MAP(CUIComboBoxDropDownList)
		CHAIN_MSG_MAP(CShadow)
		MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		CHAIN_MSG_MAP(CUILiteWindowImpl<CUIComboBoxDropDownList>)
	END_MSG_MAP()
	DECLARE_WND_CLASS_EX(COMBO_DROPDOWNLIST_CLASS_NAME, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_WINDOW)

	LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnHookMouseMessage(HWND hWnd, UINT uMsg, POINT pt, WORD wHitTest);

	BEGIN_UI_EVENT_MAP(CUIComboBoxDropDownList)
		CHAIN_UI_EVENT_MAP(CVListHelper)
		UI_EVENT_CREATE(OnUICreate)
		UI_EVENT_DESTROY(OnUIDestroy)
		UI_EVENT_PREPAINT(OnUIPrePaint)
		UI_EVENT_PAINTBACKGROUND(OnPaintBackground)
		UI_EVENT_PAINT(OnPaint)
		UI_EVENT_TIMER(OnTimer)
		UI_EVENT_SIZE(OnSize)
		UI_EVENT_HITTEST(OnHitTest)
		UI_EVENT_HANDLER(UICBEvent_DropListItemMouseOver, OnListItemMouseOver)
	END_UI_EVENT_MAP()

	void OnUICreate();
	void OnUIDestroy();
	void OnUIPrePaint();
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	void OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
	void OnSize();
	LRESULT OnHitTest(POINT point);
	LRESULT OnListItemMouseOver(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

private:

	// CVListScrollHelper
	int VListGetSel() override { return m_iListSelection; }
	void VListSetSel(int iItem) override;
	int VListGetCount() override { return m_vecItems.size(); }
	CRect VListGetItemRect(int iItem) override;
	bool VListIsRowHeightEqual() override { return true; }

	void UpdateLayout();
	void EnsureSelVisible();
	void OnEndSize(float fProgress, LPARAM lParam);

	void DeleteUI();

	static HHOOK sm_hMouseHook;
	static CUIComboBoxDropDownList* sm_pCurrentDropDown;
	static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

	CUIComboBox* m_pComboBox;
	CImageList m_imageList;
	std::vector<_UICBXItemData*> m_vecItems;
	int m_iListItemHeight;
	int m_iListSelection;
	bool m_bNeedUpdate;
	bool m_bSelEndOk;
	HWND m_hWndForground;

	std::list<_UICBXItemData*> m_lsDelayDelete;
};

//////////////////////////////////////////////////////////////////////////

class CUIComboBox: public CUIControl, public IHookMessage
{
public:

	UNIVERSAL_LIB_API explicit CUIComboBox(DWORD_PTR dwID = 0, DWORD dwStyle = 0);
	UNIVERSAL_LIB_API ~CUIComboBox();

	UNIVERSAL_LIB_API bool OnHookMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) override;
	void SetText(const std::wstring& strText) { m_Edit.UISetText(strText); }
	
	UNIVERSAL_LIB_API int AddString(LPCWSTR lpszString, int iImage = -1, int iIndent = COMBOBOX_DROPDOWNLIST_DEFAULT_INDENT);
	UNIVERSAL_LIB_API int AddString(CUITextLayout* pLayout, bool bManaged, int iImage = -1, int iIndent = COMBOBOX_DROPDOWNLIST_DEFAULT_INDENT);
	UNIVERSAL_LIB_API int DeleteItem(int iItem);
	UNIVERSAL_LIB_API void DeleteAllItems();
	UNIVERSAL_LIB_API LPCWSTR GetString(int iItem);
	UNIVERSAL_LIB_API void RemoveTextLayout(bool bKeepText = true) override;
	int GetCount() const { return m_DropDownList.GetListCount(); }
	bool SetEditSel(int iStart, int iEnd);
	UNIVERSAL_LIB_API int FindString(LPCWSTR lpszString, bool bMatchCase = false);
	bool IsDropDownVisible() { return m_DropDownList.UIGetMgr()->IsUIVisible(); }
	bool IsReadOnly() { return UITestStyle(UICBStyle_ReadOnly); }
	bool IsSimple() { return !UITestStyle(UICBStyle_DropDownList | UICBStyle_DropDown); }
	bool IsDropDown() { return UITestStyle(UICBStyle_DropDown); }
	bool IsDropDownList() { return UITestStyle(UICBStyle_DropDownList); }
	bool IsSort() { return UITestStyle(UICBStyle_Sort); }
	bool HasImage() { return UITestStyle(UICBStyle_Image); }
	bool HasExtension() { return UITestStyle(UICBStyle_Extension); }
	void SetLimitText(DWORD dwLimitText);
	DWORD GetLimitText() { return m_dwLimitText; }
	void SetDropDownHeight(DWORD dwHeight) { m_dwDropDownHeight = dwHeight; }
	DWORD GetDropDownHeight() { return m_dwDropDownHeight; }
	void SetDefaultImage(int iImage) { m_iDefaultImage = iImage; }
	void SetDefaultDropdownListItemIndent(DWORD dwIndent) { m_dwDefaultIndent = dwIndent; }
	void SetDropdownListTopMargin(DWORD dwMargin) { m_dwDropdownListTopMargin = dwMargin; }
	void SetImageList(HIMAGELIST hImageList) { m_DropDownList.SetImageList(hImageList); }
	HIMAGELIST GetImageList() { return m_DropDownList.GetImageList(); }
	UNIVERSAL_LIB_API void SetCurSel(int iSelection, bool bNotify = true);
	int GetCurSel() { return m_iCurrentSelection; }
	int GetNextItem(int iCur);
	int GetPrevItem(int iCur);
	UNIVERSAL_LIB_API bool ShowDropdownList();

	void SetBorder(const RECT& rcBorder);
	void SetButtonWidth(DWORD dwWidth);
	void SetButtonPainter(IPainter* pButtonPainter, bool bManaged = false);
	void SetEditBkPainter(IPainter* pPainter, bool bManaged = false);
	void SetDropdownItemPainter(IPainter* pPainter, bool bManaged = true);
	void UISetScrollbar(IUIScrollbar* pScroll, bool bManaged) override;
	void SetDropdownShadow(HBITMAP hbmShadow);
	UNIVERSAL_LIB_API void SetExtBkPainter(IPainter* pPainter, int iWidth, bool bAutoDelete = false);
	UNIVERSAL_LIB_API void ModifyStyle(DWORD dwAdd, DWORD dwRemove);

	//////////////////////////////////////////////////////////////////////////
	// DropDownList Callback
	void OnBeginDropDown();
	void OnCloseUp();

	//////////////////////////////////////////////////////////////////////////
	void OnCreate();
	void OnDestroy();
	int OnUIGetTextLength();
	void OnUIGetText(std::wstring& strTextOut);
	void OnUISetText(const std::wstring& strText);
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	void OnSize();
	void OnUIFocusEnter();
	void OnUIFocusLeave();
	void OnSetFocus();
	void OnUILButtonDown(UINT nFlags, POINT point);
	void OnUIStyleChanged(DWORD dwOldStyle);
	LRESULT OnUIStatusChanged(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIWantEnter(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIWantEscape(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnEditSetFocus(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnEditKillFocus(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnAsyncSelAll(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnDefaultFontChange(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnDefaultColorChange(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnShowDropdownList(CUIObject* pSender, UINH* pHDR);
	LRESULT OnEditCommand(CUIObject* pSender, DWORD dwCode, UINH* pHDR);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	void ChildLayout();

private:

	friend CUIComboBoxDropDownListItem;
	friend CUIComboBoxDropDownList;

	CUIEdit m_Edit;
	CUIButton m_Btn;
	CUIComboBoxDropDownList m_DropDownList;
	int m_iCurrentSelection;
	int m_iDefaultImage;
	bool m_bSelChanging;

	IPainter* m_pExtPainter;
	IPainter* m_pDropdownItemPainter;
	bool m_bExtPainterManaged;
	bool m_bDropdownItemPainterManaged;

	DWORD m_dwLimitText;
	DWORD m_dwDropDownHeight;
	DWORD m_dwButtonWidth;
	DWORD m_dwExtWidth;
	DWORD m_dwDefaultIndent;
	DWORD m_dwDropdownListTopMargin;
	CRect m_rcBorder;

	CRect m_rcEdit;
};

class CUIComboBoxDropDownListItem: public CUIObject, public CUITextLayoutContainter
{
public:

	CUIComboBoxDropDownListItem(CUIComboBox* pComboBox, _UICBXItemData* pData, int* pListSeletionItem);
	~CUIComboBoxDropDownListItem();

	void SetItem(int iItem) { m_iItem = iItem; }
	int GetItem() { return m_iItem; }
	_UICBXItemData* GetData() { return m_pData; }

	BEGIN_UI_EVENT_MAP(CUIComboBoxDropDownListItem)
		UI_EVENT_GETTEXT(OnUIGetText)
		UI_EVENT_SETTEXT(OnUISetText)
		UI_EVENT_GETTEXTLENGTH(OnUIGetTextLength)
		UI_EVENT_PAINTBACKGROUND(OnPaintBackground)
		UI_EVENT_PAINT(OnPaint)
		UI_EVENT_LBUTTONUP(OnLButtonUp)
		UI_EVENT_MOUSEENTER(OnMouseEnter)
	END_UI_EVENT_MAP()

	int OnUIGetTextLength() { return (UINT)m_pData->strText.length(); }
	void OnUIGetText(std::wstring& strTextOut) { strTextOut = m_pData->strText; }
	void OnUISetText(const std::wstring& strText) { m_pData->strText = strText; UIRedraw(); }
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	void OnLButtonUp(UINT nFlags, POINT point);
	void OnMouseEnter();

private:

	CUIComboBox*	m_pComboBox;
	_UICBXItemData* m_pData;
	int*			m_pListSeletionItem;
	int				m_iItem;
};

//////////////////////////////////////////////////////////////////////////
// 下面这些函数很小, 导出会比较亏, 直接inline吧

inline void CUIComboBox::SetLimitText(DWORD dwLimitText)
{
	m_dwLimitText = dwLimitText;
	m_Edit.UISetLimitText(dwLimitText);
}

inline void CUIComboBox::SetBorder(const RECT& rcBorder)
{
	m_rcBorder = rcBorder;
	if (UIIsCreated())
		UIFireEvent(UIEvent_Size);
}

inline void CUIComboBox::SetButtonWidth(DWORD dwWidth)
{
	m_dwButtonWidth = dwWidth;
	if (UIIsCreated())
		UIFireEvent(UIEvent_Size);
}

inline void CUIComboBox::SetButtonPainter(IPainter* pButtonPainter, bool bAutoDelete /* = false */)
{
	m_Btn.UISetPainter(pButtonPainter, bAutoDelete);
}

inline void CUIComboBox::SetEditBkPainter(IPainter* pPainter, bool bAutoDelete /* = false */)
{
	m_Edit.UISetBkPainter(pPainter, bAutoDelete);
}

inline void CUIComboBox::SetDropdownItemPainter(IPainter* pPainter, bool bManaged /* = true */)
{
	if (m_pDropdownItemPainter && m_bDropdownItemPainterManaged)
		delete m_pDropdownItemPainter;

	m_pDropdownItemPainter = pPainter;
	m_bDropdownItemPainterManaged = bManaged;
}

inline void CUIComboBox::SetDropdownShadow(HBITMAP hbmShadow)
{
	m_DropDownList.SetShadow(hbmShadow);
}

inline bool CUIComboBox::SetEditSel(int iStart, int iEnd)
{
	return m_Edit.UISetSel(iStart, iEnd) != 0;
}

inline void CUIComboBox::UISetScrollbar(IUIScrollbar* pScroll, bool bManaged)
{
	m_DropDownList.UISetScrollbar(pScroll, bManaged);
}

inline int CUIComboBox::OnUIGetTextLength()
{
	return m_Edit.UIGetTextLength();
}

inline void CUIComboBox::OnUIGetText(std::wstring& strTextOut)
{
	strTextOut = m_Edit.UIGetText();
}

inline void CUIComboBox::OnUISetText(const std::wstring& strText)
{
	SetText(strText);
}
