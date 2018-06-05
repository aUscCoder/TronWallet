// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "../Core/Painter.h"
#include "../Util/PaintStatusHelper.h"
#include "UIControlDef.h"
#include "UILabel.h"


class CUIControl: public CUIRichObject
{
public:

	UNIVERSAL_LIB_API explicit CUIControl(DWORD_PTR dwID = 0);
	~CUIControl() {}

	void SetControlID(DWORD_PTR dwID) { m_dwControlID = dwID; }
	DWORD_PTR GetControlID() const { return m_dwControlID; }
	void SetDrawFocusRect(bool bDrawFocusRect = true) { m_bDrawFocusRect = bDrawFocusRect; }

	bool IsDefault() const { return UITestStatus((DWORD)UIControlStatus_Default); }
	bool IsDrawFocusRect() { return UIGetMgr()->IsFoucsRectActivate() && UITestStatus(UIStatus_Focus); }

	UNIVERSAL_LIB_API LRESULT FireCommand(CUIObject* pDest, UINH* pHDR);

	//////////////////////////////////////////////////////////////////////////
	LRESULT OnSetCursor(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnScrollChange(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnLButtonDown(UINT nFlags, POINT point);
	void OnLButtonUp(UINT nFlags, POINT point);
	void OnLButtonDblClk(UINT nFlags, POINT point);
	void OnRButtonDown(UINT nFlags, POINT point);
	void OnRButtonUp(UINT nFlags, POINT point);
	void OnRButtonDblClk(UINT nFlags, POINT point);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnMouseEnter();
	void OnMouseLeave();
	void OnFocusEnter();
	void OnFocusLeave();
	void OnUIGetTooltip(UIGetTooltip* pTooltip);
	LRESULT OnDefaultChanged(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	friend class CUIModalRoot;

	DWORD_PTR m_dwControlID;
	bool m_bLButtonDown;
	bool m_bDblClick;
	bool m_bRButtonDown;
	bool m_bRDblClick;
	bool m_bDrawFocusRect;
};


//////////////////////////////////////////////////////////////////////////
class CUISysLink : public CUIControl
{
public:

	UNIVERSAL_LIB_API explicit CUISysLink(const std::wstring& strText = L"", DWORD_PTR dwControlID = 0, DWORD dwStyle = UIStyle_TabStop,
		COLORREF clrNormal = RGB(0x00, 0x7d, 0xff), COLORREF clrHover = RGB(0x00, 0x7d, 0xff), COLORREF clrActive = RGB(0x00, 0x7d, 0xff));
	~CUISysLink()
	{
	
	}

	void SetColor(COLORREF clrNormal = RGB(0x00, 0x7d, 0xff), COLORREF clrHover = RGB(0x00, 0x7d, 0xff), COLORREF clrActive = RGB(0x00, 0x7d, 0xff))
	{
		UISetTextColor(clrNormal);
		m_clrHover = clrHover;
		m_clrActive = clrActive;
		UIRedraw();
	}

	void SetHoverColor(COLORREF clrHover = RGB(0x00, 0x7d, 0xff))
	{
		m_clrHover = clrHover;
		UIRedraw();
	}

	void SetActiveColor(COLORREF clrActive = RGB(0x00, 0x7d, 0xff))
	{
		m_clrActive = clrActive;
		UIRedraw();
	}

	void SetUnderline(bool bNormal, bool bHover, bool bActive)
	{
		m_bUnderlineNormal = bNormal;
		m_bUnderlineHover = bHover;
		m_bUnderlineActive = bActive;
		UIRedraw();
	}

	void SetNomalUnderline(bool bUnderline)
	{
		m_bUnderlineNormal = bUnderline;
		UIRedraw();
	}

	void SetHoverUnderline(bool bUnderline)
	{
		m_bUnderlineHover = bUnderline;
		UIRedraw();
	}

	void SetActiveUnderline(bool bUnderline)
	{
		m_bUnderlineActive = bUnderline;
		UIRedraw();
	}

	void SetHCenter(bool bHCenter)
	{
		m_bHCenter = bHCenter;
		UIRedraw();
	}

	void SetVCenter(bool bVCenter)
	{
		m_bVCenter = bVCenter;
		UIRedraw();
	}

	//////////////////////////////////////////////////////////////////////////
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnFontChange();
	bool OnSetCursor(UINCHitTestCode ht, int iMouseEvent);
	bool OnMeasureClient(SIZE&);
	LRESULT OnReflectMouseEnter(DWORD dwCommandCode, UINH* pNotifyHeader);
	LRESULT OnReflectMouseLeave(DWORD dwCommandCode, UINH* pNotifyHeader);
	LRESULT OnReflectClick(DWORD dwCommandCode, UINH* pNotifyHeader);
	
	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

protected:

	CFont	 m_fontUnderline;

	COLORREF m_clrHover;
	COLORREF m_clrActive;

	bool m_bUnderlineNormal;
	bool m_bUnderlineHover;
	bool m_bUnderlineActive;
	bool m_bHCenter;
	bool m_bVCenter;
};

//////////////////////////////////////////////////////////////////////////
class CUITitlebar: public CUIObject, public IHookMessage
{
public:

	CUITitlebar() { UIModifyStyle(UIStyle_NoFocus, 0); }
	~CUITitlebar() {}

	BEGIN_UI_EVENT_MAP(CUITitlebar)
		UI_EVENT_LBUTTONDOWN(OnLButtonDown)
		UI_EVENT_LBUTTONDBLCLK(OnLButtonDblClk)
	END_UI_EVENT_MAP()

	void OnLButtonDown(UINT nFlags, POINT point)
	{
		UIClientToWindow(point);
		::SendMessage(UIGetMgr()->GetHWND(), WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	}

	void OnLButtonDblClk(UINT nFlags, POINT point)
	{
		if (::GetWindowLong(UIGetMgr()->GetHWND(), GWL_STYLE) & WS_MAXIMIZE)
			::PostMessage(UIGetMgr()->GetHWND(), WM_SYSCOMMAND, SC_RESTORE, NULL);
		else
			::PostMessage(UIGetMgr()->GetHWND(), WM_SYSCOMMAND, SC_MAXIMIZE, NULL);
	}

	bool OnHookMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) override { return false; }
};
