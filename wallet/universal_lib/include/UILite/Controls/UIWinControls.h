// Author: ChenJianjun
#pragma once
#include "../Core/UIManager.h"
#include "../Core/UIUtil.h"
#include "UIControls.h"


#define TIMER_AUTO_TRACE_POSITION	0x20

// 通过这个窗口去切掉超出滚动条的部分
class CUIWndContainer: public CWindowImpl<CUIWndContainer>
{
public:

	CUIWndContainer() { m_hWndChild = NULL; }
	~CUIWndContainer() {}

	HWND m_hWndChild; // NOLINT (3.3.3 这里需要)

	BEGIN_MSG_MAP_EX(CUIWndContainer)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
	DECLARE_WND_CLASS(L"_SGDUIFoundationCliper_")
};


template <class TClass, class TWinCtrlBase, class TUIObject = CUIControl>
class CUIWndBase: public TUIObject, public CWindowImpl<TClass, TWinCtrlBase, CNullTraits>
{
public:

	CUIWndBase(DWORD_PTR dwControlID)
	:	m_dwWinStyle(UITestStyle(UIStyle_Visible) ? WS_VISIBLE : 0),
		m_dwWinExStyle(0),
		m_bIgnoreTextChangedOnce(false)
	{
		m_dwWinStyle |= WS_CHILD;
		SetControlID(dwControlID);
	}

	virtual ~CUIWndBase() {}

	bool CreateControl()
	{
		TClass* pT = static_cast<TClass*>(this);
		RECT rcContainer = GetContainerRect();
		m_Container.Create(UIGetHWND(), rcContainer, NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE, 0);
		RECT rcControl = GetControlRect();
		m_Container.m_hWndChild = Create(m_Container.m_hWnd, rcControl, UIGetText().c_str(),
			pT->GetWindowStyle(), pT->GetWindowExStyle(), GetControlID());
		SetFont(UIGetFont());
		m_rcLastContainer.SetRectEmpty();
		m_rcLastWindowRect.SetRectEmpty();
		AdjustVisualSize();

		UIGetMgr()->NotifyWhenWindowPosChanged(this, false);
		return m_hWnd != NULL;
	}

	void DestroyControl()
	{
		if (IsWindow())
		{
			DestroyWindow();
			m_Container.DestroyWindow();
			UIGetMgr()->NotifyWhenWindowPosChanged(this, true);
		}
	}

	void IgnoreTextChangeOnce(bool bIgnore)
	{
		m_bIgnoreTextChangedOnce = bIgnore;
	}

	DWORD GetWindowStyle()
	{
		return m_dwWinStyle;
	}

	DWORD GetWindowExStyle()
	{
		return m_dwWinExStyle;
	}

	void ModifyWindowStyle(DWORD dwAdd, DWORD dwRemove)
	{
		m_dwWinStyle |= dwAdd;
		m_dwWinStyle &= ~dwRemove;
		if (IsWindow())
			SetWindowLongPtr(GWL_STYLE, GetWindowStyle());
	}

	void ModifyWindowExStyle(DWORD dwAdd, DWORD dwRemove)
	{
		m_dwWinExStyle |= dwAdd;
		m_dwWinExStyle &= ~dwRemove;
		if (IsWindow())
			SetWindowLongPtr(GWL_EXSTYLE, GetWindowExStyle());
	}

	bool UpdateUIText()
	{
		if (m_bIgnoreTextChangedOnce)
		{
			m_bIgnoreTextChangedOnce = false;
			return false;
		}

		wchar_t sz[1024] = { 0 };
		wchar_t* p = sz;
		int iLen = GetWindowTextLength();
		if (iLen > _countof(sz) - 1)
			p = new wchar_t[iLen + 1];
		GetWindowText(p, iLen + 1);
		m_bIgnoreTextChangedOnce = true;
		UISetText(p);
		if (p != sz)
			delete [] p;
		return true;
	}

	BEGIN_MSG_MAP(TClass)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMessage)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_SETTEXT, OnSetText)
	END_MSG_MAP()

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (!UIIsTransparent())
			UISetFocus();
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DefWindowProc();
		if (UIGetObjectType() != UIObject_WinEdit)
			UpdateUIText();
		return 0;
	}

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		CPoint pt = UIClientToWindow(CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		LRESULT lResult = 0;
		UIGetMgr()->ProcessWindowMessage(m_hWnd, uMsg, wParam, MAKELPARAM(pt.x, pt.y), lResult);
		return 0;
	}

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		if (wParam == TIMER_AUTO_TRACE_POSITION)
		{
			AdjustVisualSize();
			bHandled = TRUE;
		}
		return 0;
	}

	BEGIN_UI_EVENT_MAP(CUIWndBase)
		UI_DECLARE_OBJECT_TYPE(UIObject_WinControls)
		UI_EVENT_PAINTBACKGROUND(OnPaintBackground)
		UI_EVENT_CREATE(OnCreate)
		UI_EVENT_DESTROY(OnDestroy)
		UI_EVENT_SIZE(OnSize)
		UI_EVENT_UITEXTCHANGE(OnUITextChange)
		UI_EVENT_STYLECHANGE(OnStyleChange)
		UI_EVENT_FONTCHANGE(OnFontChange)
		UI_EVENT_WINDOWCHANGE(OnWindowChanged)
		UI_EVENT_SETCURSOR(OnSetCursor)
	END_UI_EVENT_MAP()

	void OnCreate()
	{
		CreateControl();
	}

	void OnDestroy()
	{
		DestroyControl();
	}

	void OnSize()
	{
		if (IsWindow())
			AdjustVisualSize();
	}

	void OnUITextChange()
	{
		if (m_bIgnoreTextChangedOnce)
		{
			m_bIgnoreTextChangedOnce = false;
			return;
		}

		if (IsWindow())
		{
			m_bIgnoreTextChangedOnce = true;
			SetWindowText(UIGetText().c_str());
		}
	}

	void OnStyleChange(DWORD dwOldStyle)
	{
		// 窗口状态发生了变化
		if (UITestStyle(UIStyle_Visible) && !(dwOldStyle & UIStyle_Visible))
		{
			// 从不可见变成了可见
			UIASSERT(IsWindowVisible());
			ModifyWindowStyle(WS_VISIBLE, 0);
		}
		else if (!UITestStyle(UIStyle_Visible) && dwOldStyle & UIStyle_Visible)
		{
			UIASSERT(!IsWindowVisible());
			ModifyWindowStyle(0, WS_VISIBLE);
		}
	}

	void OnFontChange()
	{
		if (IsWindow())
			SetFont(UIGetFont());
	}

	void OnPaintBackground(CDCHandle dc, DWORD dwStatus)
	{
		// 这个做法比较trick, 在画背景的时候检查一下窗口位置是否需要因为滚动位置
		// 发生变化而做一些调整
		if (!GetParent())
			return;
	}

	bool OnSetCursor(UINCHitTestCode ht, int iMouseEvent)
	{
		return true;
	}

	void OnWindowChanged()
	{
		AdjustVisualSize();
	}

private:

	void AdjustVisualSize()
	{
		CRect rcContainer = GetContainerRect();
		CRect rc = UIClientToWindow(UIGetClientRect());

		if (rcContainer != m_rcLastContainer)
		{
			CRect rcOwner;
			::GetClientRect(UIGetHWND(), &rcOwner);
			CRect rcIntersect;
			rcIntersect.IntersectRect(&rcOwner, &rcContainer);

			if (rcIntersect.IsRectEmpty())
			{
				if (m_Container.IsWindowVisible())
					m_Container.ShowWindow(SW_HIDE);
			}
			else
			{
				if (!m_Container.IsWindowVisible())
					m_Container.ShowWindow(SW_SHOW);

				m_Container.SetWindowPos(NULL, rcContainer.left, rcContainer.top, rcContainer.Width(), rcContainer.Height(),
							SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_NOREPOSITION | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS);
			}
			m_rcLastContainer = rcContainer;
		}

		bool bRedraw = false;
		if (rc != m_rcLastWindowRect)
		{
			CRect rcControl = GetControlRect();
			SetWindowPos(NULL, rcControl.left, rcControl.top, rcControl.Width(), rcControl.Height(),
					SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_NOREPOSITION | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS);
			m_rcLastWindowRect = rc;
			bRedraw = true;
		}
	}

	CRect GetContainerRect()
	{
		return UIClientToWindow(UIGetVisibleClientRect());
	}

	CRect GetControlRect()
	{
		CRect rcControl = UIGetClientRect();
		CRect rc = UIClientToWindow(UIGetClientRect());
		CRect rcContainer = GetContainerRect();
		rcControl.OffsetRect(std::min<int>(rc.left - rcContainer.left, 0), std::min<int>(0, rc.top - rcContainer.top));

		return rcControl;
	}

public:

	CUIWndContainer m_Container;

private:

	bool m_bIgnoreTextChangedOnce : 1;

	DWORD m_dwWinStyle;
	DWORD m_dwWinExStyle;

	CRect m_rcLastWindowRect;
	CRect m_rcLastContainer;

};

//////////////////////////////////////////////////////////////////////////
// Edit box
//

class CUIEditBox: public CUIWndBase<CUIEditBox, CEdit>
{
public:

	CUIEditBox(LPCWSTR lpszText, CRect rc, DWORD dwStyle = 0, DWORD dwExStyle = 0)
	:	CUIWndBase<CUIEditBox, CEdit>(0), m_iLimitText(-1)
	{
		UIModifyStyle(UIStyle_Transparent, 0);
		UISetPos(rc);
		UISetText(lpszText);
		ModifyWindowStyle(dwStyle, 0);
		ModifyWindowExStyle(dwExStyle, 0);
	}

	~CUIEditBox()
	{
		if (IsWindow())
			DestroyWindow();
	}

	void SetLimitText(int iLimit)
	{
		m_iLimitText = iLimit;
	}

	BEGIN_UI_EVENT_MAP(CUIEditBox)
		UI_DECLARE_OBJECT_TYPE(UIObject_WinEdit)
		UI_EVENT_SETCURSOR(onSetCursor)
	END_UI_EVENT_MAP()

	bool onSetCursor(UINCHitTestCode ht, int iMouseEvent)
	{
		::SetCursor(UI::Util::GetCursorIBeam());
		return true;
	}

	typedef CUIWndBase<CUIEditBox, CEdit> BaseClass;
	BEGIN_MSG_MAP(CUIEditBox)
		CHAIN_MSG_MAP(BaseClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		REFLECTED_COMMAND_CODE_HANDLER(EN_CHANGE, OnChange)
	END_MSG_MAP()
	DECLARE_WND_CLASS(L"_SGDUIFoundationEditBox_")

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		IgnoreTextChangeOnce(UIGetTextLength() > 0);
		DefWindowProc();

		if (m_iLimitText >= 0)
			SetLimitText(m_iLimitText);

		return 0;
	}

	LRESULT OnChange(UINT uNotifyCode, int nID, CWindow wndCtl, BOOL& bHandled)
	{
		if (UpdateUIText())
			UIGetParent()->UIFireEvent(UIEEvent_TextChange);
		return 0;
	}

	LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		bHandled = (wParam == VK_ESCAPE || wParam == VK_TAB);
		return 0;
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		UIGetParent()->UIFireAsyncEvent(UIEEvent_EditBoxKillFocus, wParam, lParam);
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		UIGetParent()->UIFireAsyncEvent(UIEEvent_EditBoxKeyDown, wParam, lParam);
		bHandled = FALSE;
		return 0;
	}

private:

	int m_iLimitText;
};
