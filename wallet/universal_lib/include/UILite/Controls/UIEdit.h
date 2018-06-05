// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "TxTextHost.h"
#include "UIControls.h"
#include "UIWinControls.h"


struct UIEditKeyNotify
{
	UINH hdr;
	UINT nChar;
	UINT nRepCnt;
	UINT nFlags;
};

struct UIEditContextMenu
{
	UINH hdr;
	POINT pt;
};

class CUIHScrollbar;
class CUIVScrollbar;
__interface IUIScrollbar; // NOLINT (4.5.2 lint bug, 识别不了 __interface)

class CUIEdit: public CUIControl, public IEditNotify
{
public:

	UNIVERSAL_LIB_API explicit CUIEdit(DWORD_PTR dwID = 0, LPCWSTR lpszText = NULL, DWORD dwEditStyle = 0);
	~CUIEdit() {}

	UNIVERSAL_LIB_API HRESULT OnEditNotify(DWORD iNotify, void *pv) override;

	DWORD UIGetLimitText() const { return m_TxEdit.GetLimitText(); }
	void UISetLimitText(DWORD dwLimitText) { m_TxEdit.SetLimitText(dwLimitText); }
	void UISetAutoHScroll(bool bAutoHScroll = true) { return m_TxEdit.SetAutoHScroll(bAutoHScroll); }
	void UISetTextMode(TEXTMODE tm) { m_TxEdit.SetTextMode(tm); }
	CRect  UIGetBorder() { return m_rcBorder; }
	_DECLARE_INLINE void  UISetBorder(CRect rcBorder);
	void UISetReadOnly(bool bReadOnly = true) { m_TxEdit.SetReadOnly(bReadOnly); }
	bool UIGetReadOnly() const { return (UIGetStyle() & UIEditStyle_ReadOnly) != 0; }
	std::wstring UIGetSelText() const { return m_TxEdit.GetSelText(); }
	void UIGetSel(int& iStart, int& iEnd) { m_TxEdit.GetSel(iStart, iEnd); }
	int  UISetSel(int iStart, int iEnd) { return m_TxEdit.SetSel(iStart, iEnd); }
	void UISetEventMask(DWORD dwMask) { m_TxEdit.SetEventMask(dwMask); }
	void UIReplaceSel(LPCWSTR lpszText) { m_TxEdit.ReplaceSel(lpszText, true); }
	void UIAppendText(LPCWSTR lpszText) { m_TxEdit.AppendText(lpszText); }
	void UISetWantTab(bool bWantTab = true) { m_TxEdit.SetWantTab(bWantTab); }
	void UISetWantEnter(bool bWantEnter = true) { m_TxEdit.SetWantEnter(bWantEnter); }
	void UISetWantEscape(bool bWantEscape = true) { m_TxEdit.SetWantEscape(bWantEscape); }
	void UIPaste() { m_TxEdit.Paste(); }
	void UIUndo() { m_TxEdit.Undo(); }
	bool UICanUndo() { return m_TxEdit.CanUndo(); }
	void UICut() { m_TxEdit.Cut(); }
	void UICopy() { m_TxEdit.Copy(); }
	void UISetCaretColor(COLORREF clr);
	COLORREF UIGetCaretColor() { return m_clrCaret;	}

	void UISetScrollbar(IUIScrollbar* pScroll, bool bManaged) override
	{
		m_TxEdit.UISetScrollbar(pScroll, bManaged);
	}

	int  UISelAll()
	{
		return UISetSel(0, -1);
	}

	bool IsSelAll()
	{
		int iSelStart = 0, iSelEnd = 0;
		UIGetSel(iSelStart, iSelEnd);
		// 如果文本为空，则返回 false
		return iSelStart == 0 && iSelStart != iSelEnd && iSelEnd == (int)UIGetTextLength();
	}


	//////////////////////////////////////////////////////////////////////////
	void OnCreate();
	void OnUIGetText(std::wstring& strTextOut);
	int OnUIGetTextLength();
	void OnUISetText(const std::wstring& strText);
	void OnPaintBackground(CDCHandle dc, const DWORD dwStatus);
	void OnPaint(CDCHandle dc, const DWORD dwStatus);
	void OnSize();
	void OnStyleChange(DWORD dwOldStyle);
	void OnSetFocus();
	void OnKillFocus();
	void OnFontChange();
	void OnTextColorChange();
	void OnUIEnableUI(bool bEnable);
	bool OnMeasureClient(SIZE& sz);
	LRESULT OnLeftButtonDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnLeftButtonUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnLeftButtonDblClk(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIEditContextMenu(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIEditKeyDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIEditKeyUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIEditSysKeyDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIEditSysKeyUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIEditChar(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIEditNumberOnly(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

protected:

	void VCenterRelayout();

	CRect		m_rcBorder;
	CUITextHost  m_TxEdit;
	COLORREF	m_clrCaret = 0;
};

//////////////////////////////////////////////////////////////////////////

inline void  CUIEdit::UISetBorder(CRect rcBorder)
{
	m_rcBorder = rcBorder;
	if (UIIsCreated())
		UIFireEvent(UIEvent_Size);
}
