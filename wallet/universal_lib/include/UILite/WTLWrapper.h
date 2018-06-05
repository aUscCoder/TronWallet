// Author: ChenJianjun
#pragma once
#include "../CommonLib/UI/UI.h"
#include "Core/UICoreDef.h"
#include "Core/UIModalRoot.h"
#include "Core/UIRoot.h"
#include "Util/NativeAPI.h"


class CUILiteWindowImplBase: public ICommandHandler, public CNativeAPI
{
public:

	explicit CUILiteWindowImplBase(CUIRoot* p) : CNativeAPI(p), m_pUIRoot(p), m_hOwnerWnd(NULL)
	{
		m_pUIRoot->UIGetMgr()->SetCommandHandler(this);
	}

	~CUILiteWindowImplBase() {}

	// BEGIN_UI_COMMAND_HANDLER_MAP()
	bool OnUICommand(LPCWSTR pszWindowID, DWORD dwControlID, DWORD dwCommand, UINH* pHDR, LRESULT& lResult) override { return false; }

	BEGIN_MSG_MAP(CUILiteWindowImplBase)
		ACQUIRE_HWND(m_hOwnerWnd)
		if (uMsg == WM_NCCREATE) m_hOwnerWnd = hWnd;
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNcDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)

		MESSAGE_HANDLER(0x031E, OnDwmCompositionChanged)
		CHAIN_MSG_MAP(CNativeAPI)
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)lParam;

		::SetPropW(m_hOwnerWnd, L"_is_uilite_", (HANDLE)VERIFY_CODE);

		RECT rc;
		::GetClientRect(m_hOwnerWnd, &rc);
		m_pUIRoot->UIGetMgr()->Create(m_hOwnerWnd, !!(lpCreateStruct->style & WS_VISIBLE), rc);

		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_pUIRoot->UIGetMgr()->Destroy();
		return 0;
	}

	LRESULT OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		m_hOwnerWnd = NULL;
		return 0;
	}

	LRESULT OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& b)
	{
		b = FALSE;
		if ((::GetWindowLong(m_hOwnerWnd, GWL_STYLE) & WS_MINIMIZE) == 0)
		{
			CRect rc;
			::GetClientRect(m_hOwnerWnd, &rc);
			m_pUIRoot->UIGetMgr()->UpdateUIRect(rc);
		}

		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		DWORD dwStyle = ::GetWindowLong(m_hOwnerWnd, GWL_STYLE);
		if ((dwStyle & WS_MINIMIZE) == 0)
		{
			CRect rc;
			::GetClientRect(m_hOwnerWnd, &rc);
			m_pUIRoot->UIGetMgr()->UpdateUIRect(rc);
		}

		return 0;
	}

	LRESULT OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;
		if ((lpWndPos->flags & (SWP_NOSIZE | SWP_NOMOVE)) != (SWP_NOMOVE | SWP_NOSIZE))
		{
			if ((::GetWindowLong(m_hOwnerWnd, GWL_STYLE) & WS_MINIMIZE) == 0)
			{
				// 不是最小化， 那么我更新一下窗口大小
				CRect rc;
				::GetClientRect(m_hOwnerWnd, &rc);
				m_pUIRoot->UIGetMgr()->UpdateUIRect(rc);
			}
		}

		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::DestroyWindow(m_hOwnerWnd);
		return 0;
	}

private:

	CUIRoot* m_pUIRoot;
	HWND m_hOwnerWnd;
};

//////////////////////////////////////////////////////////////////////////
// window
template <class T>
class CUILiteWindowImpl : public CUIRoot, public CWindowImpl<T>,
	public CUILiteWindowImplBase
{
protected:

	CUILiteWindowImpl() : CUILiteWindowImplBase(this)
	{
	}

	virtual ~CUILiteWindowImpl() {}

	BEGIN_MSG_MAP_EX(CUILiteWindowImpl<T>)
		CHAIN_MSG_MAP(CUIRoot)
		CHAIN_MSG_MAP(CUILiteWindowImplBase)
	END_MSG_MAP()

};

//////////////////////////////////////////////////////////////////////////
// Dialog
template <class T>
class CUILiteDialogImpl : public CWindowImpl<T>, public CUIModalRoot,
	public CUILiteWindowImplBase
{
public:

	// 此处忽略SELint的检查，现在的规则要求构造函数和析构函数放在其他普通成员函数声明的前面，同时要求按public、protec、private
	// 的顺序，两个规则相互矛盾，而又想必须子类化，故忽略检查

	CUILiteDialogImpl() : CUILiteWindowImplBase(this)
	{
	}

	virtual ~CUILiteDialogImpl() {}

	int DoModal(HWND hParent = ::GetActiveWindow(),
		CRect rc = CRect(),
		UINT uStyle = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_DLGFRAME | WS_BORDER | WS_CLIPCHILDREN | WS_POPUP,
		UINT uExStyle = 0,
		bool bAutoDpiScale = true)
	{
		if (IsWindow())
			return UIDCANCEL;
		hParent = ::GetAncestor(hParent, GA_ROOT);

		Create(hParent,
			bAutoDpiScale ? UI_SCALE(rc) : rc,
			NULL,
			uStyle & ~(WS_CHILD | WS_VISIBLE),
			uExStyle);

		return ModalUI(hParent);
	}

	bool DoModalCallback(IModalCallback* pCallback, HWND hParent = ::GetActiveWindow(), bool bAutoDpiScale = true)
	{
		if (IsWindow())
			return UIDCANCEL;
		hParent = ::GetAncestor(hParent, GA_ROOT);

		Create(m_hParent,
			CRect(0, 0, bAutoDpiScale ? UI_SCALE(640) : 640, bAutoDpiScale ? UI_SCALE(480) : 480),
			NULL,
			WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
			0);

		return ModalCallbackUI(pCallback, hParent);
	}

	void EndDialog(int iEndCode) { EndModal(iEndCode); }

	BEGIN_MSG_MAP_EX(CUILiteDialogImpl<T>)
		CHAIN_MSG_MAP(CUIModalRoot)
		CHAIN_MSG_MAP(CUILiteWindowImplBase)
	END_MSG_MAP()
};
