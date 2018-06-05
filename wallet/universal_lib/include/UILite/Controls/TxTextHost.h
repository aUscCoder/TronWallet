// Author: ChenJianjun
#pragma once
#include <textserv.h>
#include <tom.h>
#include "../Core/UIRichObject.h"
#include "../UICrack.h"
#include "../WTLWrapper.h"
#include "TextServices.h"
#include "UIControlDef.h"
#include "UIControls.h"


#define INIT_SIZE(var) var.cbSize = sizeof(var)

struct IEditNotify
{
	virtual HRESULT OnEditNotify(DWORD iNotify, void *pv) = 0;
};

class CUITextHost: public CUIRichObject, public ITextHost
{
public:

	UNIVERSAL_LIB_API explicit CUITextHost(CUIObject* pEdit);
	~CUITextHost() {}

	UNIVERSAL_LIB_API LRESULT TxSendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) const;

	_DECLARE_INLINE int AppendText(LPCTSTR lpstrText, bool bCanUndo = false);
	_DECLARE_INLINE void ClearUndoBuffer();
	_DECLARE_INLINE bool CanRedo() const;
	_DECLARE_INLINE bool CanUndo() const;
	_DECLARE_INLINE bool CanPaste() const;
	_DECLARE_INLINE void Clear();
	_DECLARE_INLINE void Copy();
	_DECLARE_INLINE void Cut();
	_DECLARE_INLINE void HideSelection(bool bHide = true, bool bChangeStyle = false);
	_DECLARE_INLINE int  InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo = false);
	_DECLARE_INLINE void Paste();
	_DECLARE_INLINE void ReplaceSel(LPCWSTR lpszNewText, bool bCanUndo = false) const;
	_DECLARE_INLINE bool Redo();
	_DECLARE_INLINE void ScrollCaret();
	_DECLARE_INLINE long StreamIn(int nFormat, EDITSTREAM &es);
	_DECLARE_INLINE long StreamOut(int nFormat, EDITSTREAM &es);
	_DECLARE_INLINE bool Undo();

	_DECLARE_INLINE bool  IsRich() const;
	_DECLARE_INLINE bool  IsAutoHScroll() const;
	_DECLARE_INLINE bool  IsReadOnly() const;
	_DECLARE_INLINE bool  GetAutoURLDetect() const;
	_DECLARE_INLINE CPoint GetCharPos(long lChar) const;
	_DECLARE_INLINE int	  GetCharFromPos(CPoint pt) const;
	_DECLARE_INLINE DWORD GetDefaultCharFormat(CHARFORMAT2W &cf) const;
	_DECLARE_INLINE DWORD GetEventMask() const;
	_DECLARE_INLINE bool  GetModify() const;
	_DECLARE_INLINE DWORD GetLimitText() const;
	_DECLARE_INLINE int	  GetLineHeight() const;
	_DECLARE_INLINE int	  GetLineCount() const;
	_DECLARE_INLINE int	  GetLineIndex(int nLine = -1) const;
	_DECLARE_INLINE int	  GetLineLength(int nLine = -1) const;
	_DECLARE_INLINE bool  GetLineScroll(int nLines, int nChars = 0);
	_DECLARE_INLINE long  GetLineFromChar(long nIndex) const;
	UNIVERSAL_LIB_API std::wstring GetLine(int nIndex, int nMaxLength) const;
	_DECLARE_INLINE DWORD GetParaFormat(PARAFORMAT2 &pf) const;
	_DECLARE_INLINE CPoint GetPosFromChar(UINT nChar) const;
	_DECLARE_INLINE void  GetSel(CHARRANGE &cr) const;
	_DECLARE_INLINE void  GetSel(int& nStart, int& nEnd) const;
	UNIVERSAL_LIB_API std::wstring GetSelText() const;
	_DECLARE_INLINE WORD  GetSelectionType() const;
	_DECLARE_INLINE DWORD GetSelectionCharFormat(CHARFORMAT2W &cf) const;
	_DECLARE_INLINE LONG  GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
	UNIVERSAL_LIB_API std::wstring GetText() const; // NOLINT (3.4.7 与 ITextRange::GetText 重名)
	UNIVERSAL_LIB_API std::wstring GetTextRange(int nStartChar, int nEndChar) const;
	_DECLARE_INLINE bool  GetZoom(int& nNum, int& nDen) const;

	_DECLARE_INLINE void  SetAutoHScroll(bool bAutoHScroll = true);
	_DECLARE_INLINE bool  SetAutoURLDetect(bool bAutoDetect = true);
	_DECLARE_INLINE bool  SetDefaultCharFormat(CHARFORMAT2W &cf);
	_DECLARE_INLINE DWORD SetEventMask(DWORD dwEventMask);
	_DECLARE_INLINE void  SetLimitText(DWORD dwChars);
	_DECLARE_INLINE void  SetModify(bool bModified = true);
	_DECLARE_INLINE void  SetNotify(IEditNotify* pNotify);
	_DECLARE_INLINE bool  SetParaFormat(PARAFORMAT2 &pf);
	_DECLARE_INLINE void  SetRich(bool bRich = true);
	_DECLARE_INLINE void  SetReadOnly(bool bReadOnly = true);
	_DECLARE_INLINE bool  SetSelectionCharFormat(CHARFORMAT2W &cf);
	_DECLARE_INLINE int   SetSel(CHARRANGE &cr);
	_DECLARE_INLINE int   SetSel(long nStart, long nEnd);
	_DECLARE_INLINE int   SetSelAll();
	_DECLARE_INLINE int   SetSelNone();
	_DECLARE_INLINE void  SetTextMode(TEXTMODE tm);
	_DECLARE_INLINE void  SetText(LPCWSTR pstrText);
	_DECLARE_INLINE UINT  SetUndoLimit(UINT nLimit);
	_DECLARE_INLINE void  SetWantTab(bool bWantTab = true);
	_DECLARE_INLINE void  SetWantEnter(bool bWantEnter = true);
	_DECLARE_INLINE void  SetWantEscape(bool bWantEscape = true);
	_DECLARE_INLINE bool  SetWordCharFormat(CHARFORMAT2W &cf);
	_DECLARE_INLINE bool  SetZoom(int nNum, int nDen);
	_DECLARE_INLINE bool  SetZoomOff();

	_DECLARE_INLINE bool  HasService() const;

	//////////////////////////////////////////////////////////////////////////
	// ITextHost
	ULONG STDMETHODCALLTYPE AddRef() { return 1; }
	ULONG STDMETHODCALLTYPE Release() { return 1; }
	UNIVERSAL_LIB_API LRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* ppvObj);

	_DECLARE_INLINE HDC TxGetDC() override;
	_DECLARE_INLINE INT TxReleaseDC(HDC hdc) override;
	UNIVERSAL_LIB_API BOOL TxShowScrollBar(INT fnBar, BOOL fShow) override;
	UNIVERSAL_LIB_API BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) override;
	UNIVERSAL_LIB_API BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) override;
	UNIVERSAL_LIB_API BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) override;
	_DECLARE_INLINE void TxInvalidateRect(LPCRECT prc, BOOL fMode) override;
	_DECLARE_INLINE void TxViewChange(BOOL fUpdate) override;
	UNIVERSAL_LIB_API BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) override;
	UNIVERSAL_LIB_API BOOL TxShowCaret(BOOL fShow) override;
	UNIVERSAL_LIB_API BOOL TxSetCaretPos(INT x, INT y) override;
	_DECLARE_INLINE BOOL TxSetTimer(UINT idTimer, UINT uTimeout) override;
	_DECLARE_INLINE void TxKillTimer(UINT idTimer) override;
	_DECLARE_INLINE void TxScrollWindowEx(INT dx, INT dy, LPCRECT lpScroll, LPCRECT lpClip, HRGN rgnUpdate, LPRECT lpUpdate, UINT fuScroll) override;
	_DECLARE_INLINE void TxSetCapture(BOOL fCapture) override;
	_DECLARE_INLINE void TxSetFocus() override;
	_DECLARE_INLINE void TxSetCursor(HCURSOR hcur, BOOL fText) override;
	_DECLARE_INLINE BOOL TxScreenToClient(LPPOINT lppt) override;
	_DECLARE_INLINE BOOL TxClientToScreen(LPPOINT lppt) override;
	_DECLARE_INLINE HRESULT TxActivate(LONG * plOldState) override;
	_DECLARE_INLINE HRESULT TxDeactivate(LONG lNewState) override;
	_DECLARE_INLINE HRESULT TxGetClientRect(LPRECT prc) override;
	_DECLARE_INLINE HRESULT TxGetViewInset(LPRECT prc) override;
	_DECLARE_INLINE HRESULT TxGetCharFormat(const CHARFORMATW **ppCF) override;
	_DECLARE_INLINE HRESULT TxGetParaFormat(const PARAFORMAT **ppPF) override;
	_DECLARE_INLINE COLORREF TxGetSysColor(int nIndex) override;
	_DECLARE_INLINE HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle) override;
	_DECLARE_INLINE HRESULT TxGetMaxLength(DWORD *plength) override;
	UNIVERSAL_LIB_API HRESULT TxGetScrollBars(DWORD *pdwScrollBar) override;
	_DECLARE_INLINE HRESULT TxGetPasswordChar(WCHAR *pch) override;
	_DECLARE_INLINE HRESULT TxGetAcceleratorPos(LONG *pcp) override;
	_DECLARE_INLINE HRESULT TxGetExtent(LPSIZEL lpExtent) override;
	_DECLARE_INLINE HRESULT OnTxCharFormatChange(const CHARFORMATW * pcf) override;
	_DECLARE_INLINE HRESULT OnTxParaFormatChange(const PARAFORMAT * ppf) override;
	UNIVERSAL_LIB_API HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) override;
	_DECLARE_INLINE HRESULT TxNotify(DWORD iNotify, void *pv) override;
	_DECLARE_INLINE HIMC TxImmGetContext() override;
	_DECLARE_INLINE void TxImmReleaseContext(HIMC himc) override;
	_DECLARE_INLINE HRESULT TxGetSelectionBarWidth(LONG *lSelBarWidth) override;

	//////////////////////////////////////////////////////////////////////////
	//

	void OnSetFocus();
	void OnKillFocus();
	void OnUITextColorChange();
	void OnStyleChange(DWORD dwOldStyle);

	//////////////////////////////////////////////////////////////////////////
	void OnCreate();
	void OnDestroy();
	int OnUIGetTextLength();
	void OnUIGetText(std::wstring& strTextOut);
	void OnUISetText(const std::wstring& strText);
	void OnWindowChange();
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus) {}
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	void OnUIFontChange();
	void OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
	void OnSize();
	bool OnSetCursor(UINCHitTestCode ht, int iMouseEvent);
	void OnUIActive(bool bActive);
	void OnActiveApp(bool bActive, int iThreadID);
	void OnUIEnableUI(bool bEnable);
	void OnUIContextMenu(CUIObject* pSender, CPoint ptScreen);
	LRESULT OnDefaultColorChange(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnScrollChange(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnCalcChildrenRect(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnLeftButtonDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnLeftButtonUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnLeftButtonDblClk(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnRightButtonDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnRightButtonUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnRightButtonDblClk(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnMouseMove(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnKeyDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnKeyUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnSysKeyDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnSysKeyUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnChar(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnWantArrow(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled) { return TRUE; }
	LRESULT OnWantTab(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled) { return m_bWantTab ? TRUE : FALSE; }
	LRESULT OnWantEnter(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled) { return m_bWantEnter ? TRUE : FALSE; }
	LRESULT OnWantEscape(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled) { return m_bWantEscape ? TRUE : FALSE; }
	LRESULT OnImeSetContext(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnImeNotify(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnAsyncScroll(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnVScroll(UIVScrollEvent eEvent, int iDelta);

	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	void BlinkCaret();
	UNIVERSAL_LIB_API LRESULT CreateTextServices() const;
	void ReleaseService();
	void InitDefaultFormat();
	UNIVERSAL_LIB_API void NotifyPropertyBitsChange(DWORD dwMask, DWORD dwBits);
	void SetFont(HFONT hFont);
	void ShowCaret(bool bShow = true);
	void UpdateImePos();
	UNIVERSAL_LIB_API bool IsClipboardTextNumeric();
	_DECLARE_INLINE void NotifyNumberOnly();

	mutable std::unique_ptr<CTextServices> m_pTextServices;
	
	CUIObject* m_pEdit;

	LOGFONT m_LogFont;
	CHARFORMAT2W m_CharFormat;
	int m_iLineHeight;
	PARAFORMAT2 m_ParaFormat;
	RECT m_rcBorder;
	DWORD m_dwEventMask;
	UINT m_nUndoLimit;

	DWORD m_dwLimitText;
	bool m_bWantTab;
	bool m_bWantEnter;
	bool m_bWantEscape;
	bool m_bMenuPopup;
	bool m_bTxHScrollVisible;
	bool m_bTxVScrollVisible;

	HBITMAP m_hCaret;
	CSize   m_sizeCaret;
	CPoint  m_ptCaret;
	bool    m_bCaretShown;
	bool    m_bCaretBlinkingVisible;

	bool m_bScrolling;

	IEditNotify* m_pNotify;
};

//////////////////////////////////////////////////////////////////////////
// 下面这些函数很小, 导出会比较亏, 直接inline吧

inline int CUITextHost::AppendText(LPCTSTR lpstrText, bool bCanUndo)
{
	CreateTextServices();
	int nRet = SetSel(-1, -1);
	ReplaceSel(lpstrText, bCanUndo);
	return nRet;
}

inline void CUITextHost::ClearUndoBuffer()
{
	TxSendMessage(EM_EMPTYUNDOBUFFER, 0, 0);
}

inline bool CUITextHost::CanRedo() const
{
	return TxSendMessage(EM_CANREDO, 0, 0) != FALSE;
}

inline bool CUITextHost::CanUndo() const
{
	return TxSendMessage(EM_CANUNDO, 0, 0) != FALSE;
}

inline bool CUITextHost::CanPaste() const
{
	CreateTextServices();
	return TxSendMessage(EM_CANPASTE, 0, 0) != FALSE;
}

inline void CUITextHost::Clear()
{
	CreateTextServices();
	TxSendMessage(WM_CLEAR, 0, 0);
}

inline void CUITextHost::Copy()
{
	CreateTextServices();
	TxSendMessage(WM_COPY, 0, 0);
}

inline void CUITextHost::Cut()
{
	CreateTextServices();
	TxSendMessage(WM_CUT, 0, 0);
}

inline void CUITextHost::HideSelection(bool bHide, bool bChangeStyle)
{
	CreateTextServices();
	TxSendMessage(EM_HIDESELECTION, bHide, bChangeStyle);
}

inline int CUITextHost::InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo)
{
	CreateTextServices();
	int nRet = SetSel(nInsertAfterChar, nInsertAfterChar);
	ReplaceSel(lpstrText, bCanUndo);
	return nRet;
}

inline void CUITextHost::Paste()
{
	CreateTextServices();
	if (m_pEdit->UITestStyle(UIEditStyle_Number) && !IsClipboardTextNumeric())
		NotifyNumberOnly();
	else
		TxSendMessage(WM_PASTE, 0, 0);
}

inline void CUITextHost::ReplaceSel(LPCWSTR lpszNewText, bool bCanUndo) const
{
	CreateTextServices();
	TxSendMessage(EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)lpszNewText);
}

inline bool CUITextHost::Redo()
{
	CreateTextServices();
	return TxSendMessage(EM_REDO, 0, 0) != FALSE;
}

inline void CUITextHost::ScrollCaret()
{
	CreateTextServices();
	TxSendMessage(EM_SCROLLCARET, 0, 0);
}

inline long CUITextHost::StreamIn(int nFormat, EDITSTREAM &es)
{
	CreateTextServices();
	return (long)TxSendMessage(EM_STREAMIN, nFormat, (LPARAM)&es);
}

inline long CUITextHost::StreamOut(int nFormat, EDITSTREAM &es)
{
	CreateTextServices();
	return (long)TxSendMessage(EM_STREAMOUT, nFormat, (LPARAM)&es);
}

inline bool CUITextHost::Undo()
{
	CreateTextServices();
	return TxSendMessage(EM_UNDO, 0, 0) != FALSE;
}

inline bool CUITextHost::IsRich() const
{
	return m_pEdit->UITestStyle(UIEditStyle_RichEdit);
}

inline bool CUITextHost::IsAutoHScroll() const
{
	return m_pEdit->UITestStyle(UIEditStyle_AutoHScroll);
}

inline bool CUITextHost::IsReadOnly() const
{
	return m_pEdit->UITestStyle(UIEditStyle_ReadOnly);
}

inline bool CUITextHost::GetAutoURLDetect() const
{
	CreateTextServices();
	return TxSendMessage(EM_GETAUTOURLDETECT, 0, 0) != FALSE;
}

inline CPoint CUITextHost::GetCharPos(long lChar) const
{
	CreateTextServices();

	CPoint pt;
	TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)lChar);
	return pt;
}

inline int CUITextHost::GetCharFromPos(CPoint pt) const
{
	CreateTextServices();
	return (int)TxSendMessage(EM_CHARFROMPOS, 0, (LPARAM)&pt);
}

inline DWORD CUITextHost::GetDefaultCharFormat(CHARFORMAT2W &cf) const
{
	CreateTextServices();
	INIT_SIZE(cf);
	return (DWORD)TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
}

inline DWORD CUITextHost::GetEventMask() const
{
	CreateTextServices();
	return (DWORD)TxSendMessage(EM_GETEVENTMASK, 0, 0);
}

inline bool CUITextHost::GetModify() const
{
	CreateTextServices();
	return TxSendMessage(EM_GETMODIFY, 0, 0) != FALSE;
}

inline DWORD CUITextHost::GetLimitText() const
{
	return m_dwLimitText;
}

inline int CUITextHost::GetLineHeight() const
{
	return m_iLineHeight;
}

inline int CUITextHost::GetLineCount() const
{
	CreateTextServices();
	return (int)TxSendMessage(EM_GETLINECOUNT, 0, 0);
}

inline int CUITextHost::GetLineIndex(int nLine) const
{
	CreateTextServices();
	return (int)TxSendMessage(EM_LINEINDEX, nLine, 0);
}

inline int CUITextHost::GetLineLength(int nLine) const
{
	CreateTextServices();
	return (int)TxSendMessage(EM_LINELENGTH, nLine, 0);
}

inline bool CUITextHost::GetLineScroll(int nLines, int nChars)
{
	CreateTextServices();
	return TxSendMessage(EM_LINESCROLL, nChars, nLines) != FALSE;
}

inline long CUITextHost::GetLineFromChar(long nIndex) const
{
	CreateTextServices();
	return (long)TxSendMessage(EM_EXLINEFROMCHAR, 0, nIndex);
}

inline DWORD CUITextHost::GetParaFormat(PARAFORMAT2 &pf) const
{
	CreateTextServices();
	INIT_SIZE(pf);
	return (DWORD)TxSendMessage(EM_GETPARAFORMAT, 0, (LPARAM)&pf);
}

inline CPoint CUITextHost::GetPosFromChar(UINT nChar) const
{
	CreateTextServices();
	CPoint pt;
	TxSendMessage(EM_POSFROMCHAR, (WPARAM)&pt, nChar);
	return pt;
}

inline void  CUITextHost::GetSel(CHARRANGE &cr) const
{
	CreateTextServices();
	TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
}

inline void CUITextHost::GetSel(int& nStart, int& nEnd) const
{
	CreateTextServices();
	CHARRANGE cr = { 0 };
	TxSendMessage(EM_EXGETSEL, 0, (LPARAM)&cr);
	
	nStart = cr.cpMin;
	nEnd = cr.cpMax;
}

inline WORD CUITextHost::GetSelectionType() const
{
	CreateTextServices();
	return (WORD)TxSendMessage(EM_SELECTIONTYPE, 0, 0);
}

inline DWORD CUITextHost::GetSelectionCharFormat(CHARFORMAT2W &cf) const
{
	CreateTextServices();
	INIT_SIZE(cf);
	return (DWORD)TxSendMessage(EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
}

inline LONG CUITextHost::GetTextLength(DWORD dwFlags/* = GTL_DEFAULT */) const
{
	CreateTextServices();
	GETTEXTLENGTHEX Len = { dwFlags, 1200 };
	return (LONG)TxSendMessage(EM_GETTEXTLENGTHEX, (WPARAM)&Len, 0);
}

inline bool CUITextHost::GetZoom(int& nNum, int& nDen) const
{
	CreateTextServices();
	return TxSendMessage(EM_GETZOOM, (WPARAM)&nNum, (LPARAM)&nDen) != FALSE;
}

inline void CUITextHost::SetAutoHScroll(bool bAutoHScroll)
{
	m_pEdit->UIAddOrRemoveStyle(bAutoHScroll, UIEditStyle_AutoHScroll);
}

inline bool CUITextHost::SetAutoURLDetect(bool bAutoDetect)
{
	CreateTextServices();
	return TxSendMessage(EM_AUTOURLDETECT, bAutoDetect, 0) == S_OK;
}

inline bool CUITextHost::SetDefaultCharFormat(CHARFORMAT2W &cf)
{
	CreateTextServices();
	cf.cbSize = sizeof(CHARFORMAT2W);
	if (TxSendMessage(EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf))
	{
		TxSendMessage(EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&m_CharFormat);
		return true;
	}
	return false;
}

inline DWORD CUITextHost::SetEventMask(DWORD dwEventMask)
{
	m_dwEventMask = dwEventMask;
	return (DWORD)TxSendMessage(EM_SETEVENTMASK, 0, dwEventMask);
}

inline void CUITextHost::SetLimitText(DWORD dwChars)
{
	m_dwLimitText = dwChars;
	NotifyPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
}

inline void  CUITextHost::SetModify(bool bModified)
{
	CreateTextServices();
	TxSendMessage(EM_SETMODIFY, bModified, 0);
}

inline void  CUITextHost::SetNotify(IEditNotify* pNotify)
{
	m_pNotify = pNotify;
}

inline bool CUITextHost::SetParaFormat(PARAFORMAT2 &pf)
{
	CreateTextServices();
	pf.cbSize = sizeof(PARAFORMAT2);
	if (TxSendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf))
	{
		GetParaFormat(m_ParaFormat);
		return true;
	}
	return false;
}

inline void CUITextHost::SetRich(bool bRich)
{
	m_pEdit->UIAddOrRemoveStyle(bRich, UIEditStyle_RichEdit);
	NotifyPropertyBitsChange(TXTBIT_RICHTEXT, bRich ? TXTBIT_RICHTEXT : 0);
}

inline void CUITextHost::SetReadOnly(bool bReadOnly)
{
	m_pEdit->UIAddOrRemoveStyle(bReadOnly, UIEditStyle_ReadOnly);
	NotifyPropertyBitsChange(TXTBIT_READONLY, bReadOnly ? TXTBIT_READONLY : 0);
}

inline bool CUITextHost::SetSelectionCharFormat(CHARFORMAT2W &cf)
{
	CreateTextServices();
	INIT_SIZE(cf);
	return TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf) != FALSE;
}

inline int CUITextHost::SetSel(CHARRANGE &cr)
{
	if (!m_pTextServices)
	{
		if ((cr.cpMin == cr.cpMax) && (LONG)UIGetTextLength() == cr.cpMin || cr.cpMax == 0)
			return 0;

		CreateTextServices();
	}
	return (int)TxSendMessage(EM_EXSETSEL, 0, (LPARAM)&cr);
}

inline int CUITextHost::SetSel(long nStart, long nEnd)
{
	CHARRANGE cr = { nStart, nEnd };
	return SetSel(cr);
}

inline int CUITextHost::SetSelAll()
{
	return SetSel(0, -1);
}

inline int CUITextHost::SetSelNone()
{
	return SetSel(-1, 0);
}

inline void CUITextHost::SetTextMode(TEXTMODE tm)
{
	CreateTextServices();
	TxSendMessage(EM_SETTEXTMODE, tm, 0);
}

inline void CUITextHost::SetText(LPCWSTR pstrText)
{
	SetSel(0, -1);
	ReplaceSel(pstrText, FALSE);
}

inline UINT  CUITextHost::SetUndoLimit(UINT nLimit)
{
	m_nUndoLimit = nLimit;
	return (UINT)TxSendMessage(EM_SETUNDOLIMIT, (WPARAM)nLimit, 0);
}

inline void  CUITextHost::SetWantTab(bool bWantTab)
{
	m_bWantTab = bWantTab;
}

inline void  CUITextHost::SetWantEnter(bool bWantEnter)
{
	m_bWantEnter = bWantEnter;
}

inline void  CUITextHost::SetWantEscape(bool bWantEscape)
{
	m_bWantEscape = bWantEscape;
}

inline bool CUITextHost::SetWordCharFormat(CHARFORMAT2W &cf)
{
	INIT_SIZE(cf);
	CreateTextServices();
	return TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION | SCF_WORD, (LPARAM)&cf) != 0;
}

inline bool CUITextHost::SetZoom(int nNum, int nDen)
{
	if (nNum < 0 || nNum > 64 || nDen < 0 || nDen > 64)
		return false;
	CreateTextServices();
	return TxSendMessage(EM_SETZOOM, nNum, nDen) != FALSE;
}

inline bool  CUITextHost::SetZoomOff()
{
	CreateTextServices();
	return TxSendMessage(EM_SETZOOM, 0, 0) != FALSE;
}

inline bool CUITextHost::HasService() const
{
	return m_pTextServices != nullptr && m_pTextServices->IsWorking();
}

inline HDC CUITextHost::TxGetDC()
{
	UICHECK_OBJECT_VALID();
	return Universal::UI::Cache::GetDC();
}

inline INT CUITextHost::TxReleaseDC(HDC hdc)
{
	UICHECK_OBJECT_VALID();
	if (hdc)
		Universal::UI::Cache::ReleaseDC(hdc);
	return 1;
}

inline void CUITextHost::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
	UICHECK_OBJECT_VALID();
	UIRedraw();
}

inline void CUITextHost::TxViewChange(BOOL fUpdate)
{
	UICHECK_OBJECT_VALID();

}

inline BOOL CUITextHost::TxSetTimer(UINT idTimer, UINT uTimeout)
{
	UICHECK_OBJECT_VALID();
	UISetTimer(idTimer, uTimeout);
	return TRUE;
}

inline void CUITextHost::TxKillTimer(UINT idTimer)
{
	UICHECK_OBJECT_VALID();
	UIKillTimer(idTimer);
}

inline void CUITextHost::TxScrollWindowEx(INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll)
{
	UICHECK_OBJECT_VALID();
}

inline void CUITextHost::TxSetCapture(BOOL fCapture)
{
	UICHECK_OBJECT_VALID();
}

inline void CUITextHost::TxSetFocus()
{
	UICHECK_OBJECT_VALID();
}

inline void CUITextHost::TxSetCursor(HCURSOR hcur, BOOL fText)
{
	UICHECK_OBJECT_VALID();
	::SetCursor(hcur);
}

inline BOOL CUITextHost::TxScreenToClient(LPPOINT lppt)
{
	UICHECK_OBJECT_VALID();
	return ::ScreenToClient(UIGetHWND(), lppt);
}

inline BOOL CUITextHost::TxClientToScreen(LPPOINT lppt)
{
	UICHECK_OBJECT_VALID();
	return ::ClientToScreen(UIGetHWND(), lppt);
}

inline HRESULT CUITextHost::TxActivate(LONG * plOldState)
{
	UICHECK_OBJECT_VALID();
	return S_OK;
}

inline HRESULT CUITextHost::TxDeactivate(LONG lNewState)
{
	UICHECK_OBJECT_VALID();
	return S_OK;
}

inline HRESULT CUITextHost::TxGetClientRect(LPRECT prc)
{
	UICHECK_OBJECT_VALID();
	CRect rc = UIGetRectNonClientCliped();
	rc.OffsetRect(-rc.left, -rc.top);
	*prc = rc;

	return S_OK;
}

inline HRESULT CUITextHost::TxGetViewInset(LPRECT prc)
{
	UICHECK_OBJECT_VALID();
	*prc = CRect();
	return S_OK;
}

inline HRESULT CUITextHost::TxGetCharFormat(const CHARFORMATW **ppCF)
{
	UICHECK_OBJECT_VALID();
	m_CharFormat.crTextColor = UIIsDisabled() ? m_pEdit->UIGetDisabledTextColor() : m_pEdit->UIGetTextColor();
	*ppCF = &m_CharFormat;
	return S_OK;
}

inline HRESULT CUITextHost::TxGetParaFormat(const PARAFORMAT **ppPF)
{
	UICHECK_OBJECT_VALID();
	*ppPF = &m_ParaFormat;
	return S_OK;
}

inline COLORREF CUITextHost::TxGetSysColor(int nIndex)
{
	UICHECK_OBJECT_VALID();
	return ::GetSysColor(nIndex);
}

inline HRESULT CUITextHost::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
	UICHECK_OBJECT_VALID();
	*pstyle = TXTBACK_TRANSPARENT;
	return S_OK;
}

inline HRESULT CUITextHost::TxGetMaxLength(DWORD *plength)
{
	UICHECK_OBJECT_VALID();
	*plength = m_dwLimitText;
	return S_OK;
}

inline HRESULT CUITextHost::TxGetPasswordChar(WCHAR *pch)
{
	UICHECK_OBJECT_VALID();
	if (m_pEdit->UITestStyle(UIEditStyle_Password))
	{
		*pch = L'●';
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

inline HRESULT CUITextHost::TxGetAcceleratorPos(LONG *pcp)
{
	UICHECK_OBJECT_VALID();
	*pcp = -1;
	return S_OK;
}

inline HRESULT CUITextHost::TxGetExtent(LPSIZEL lpExtent)
{
	UICHECK_OBJECT_VALID();
	//	*lpExtent = m_sizeExtent;
	return E_NOTIMPL;
}

inline HRESULT CUITextHost::OnTxCharFormatChange(const CHARFORMATW * pcf)
{
	return S_OK;
}

inline HRESULT CUITextHost::OnTxParaFormatChange(const PARAFORMAT * ppf)
{
	return S_OK;
}

inline HRESULT CUITextHost::TxNotify(DWORD iNotify, void *pv)
{
	UICHECK_OBJECT_VALID();
	if (m_pNotify)
		return m_pNotify->OnEditNotify(iNotify, pv);
	return S_OK;
}

inline HIMC CUITextHost::TxImmGetContext()
{
	UICHECK_OBJECT_VALID();
	return ::ImmGetContext(UIGetHWND());
}

inline void CUITextHost::TxImmReleaseContext(HIMC himc)
{
	UICHECK_OBJECT_VALID();
	::ImmReleaseContext(UIGetHWND(), himc);
}

inline HRESULT CUITextHost::TxGetSelectionBarWidth(LONG *lSelBarWidth)
{
	UICHECK_OBJECT_VALID();
	*lSelBarWidth = 0;
	return S_OK;
}

inline int CUITextHost::OnUIGetTextLength()
{
	if (m_pTextServices)
		return GetTextLength();
	else
		UISetHandled(false);

	return 0;
}

inline void CUITextHost::OnUIGetText(std::wstring& strTextOut)
{
	if (m_pTextServices)
		strTextOut = GetText();
	else
		UISetHandled(false);
}

inline void CUITextHost::OnUISetText(const std::wstring& strText)
{
	if (m_pTextServices)
		SetText(strText.c_str());
	else
		UISetHandled(false);
}

inline void CUITextHost::NotifyNumberOnly()
{
	if (m_pEdit)
		m_pEdit->UIFireEvent(UIEEvent_NumberOnly);
}
