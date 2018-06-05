#include "BaseWindow.h"
#include "BaseTitleBar.h"
#include "SkinResource.h"
#include "ui.h"

#define TITLE_HEIGHT    32


class CUIBaseWindowShadow : public CUILiteWindowImpl<CUIBaseWindowShadow>
{
public:

    CUIBaseWindowShadow();
    ~CUIBaseWindowShadow();

    CSize GetPreferredSize();

private:
    
    BEGIN_MSG_MAP_EX(CUIBaseWindowShadow)
        MESSAGE_HANDLER_EX(WM_NCPAINT, OnNCPaint)
        MESSAGE_HANDLER_EX(WM_MOUSEACTIVATE, OnMouseActive)
        CHAIN_MSG_MAP(CUILiteWindowImpl<CUIBaseWindowShadow>)
    END_MSG_MAP()

    LRESULT OnMouseActive(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnNCPaint(UINT uMsg, WPARAM wParam, LPARAM lParam);

    BEGIN_UI_EVENT_MAP(CUIBaseWindowShadow)
        UI_EVENT_PAINT(OnUIPaint)
    END_UI_EVENT_MAP()

    void OnUIPaint(CDCHandle dc, DWORD dwStatus);

private:

    CBitmap m_hBmp;
};

CUIBaseWindowShadow::CUIBaseWindowShadow()
{
    m_hBmp = Universal::UI::Bitmap::LoadFromRes(NULL, IDB_PNG_SHADOW, L"PNG");
}

CUIBaseWindowShadow::~CUIBaseWindowShadow()
{

}

CSize CUIBaseWindowShadow::GetPreferredSize()
{
    CSize sz;
    m_hBmp.GetSize(sz);
    return CSize(sz.cx / 2, sz.cy / 2);
}

LRESULT CUIBaseWindowShadow::OnMouseActive(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return MA_NOACTIVATE;
}

LRESULT CUIBaseWindowShadow::OnNCPaint(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return S_OK;
}

void CUIBaseWindowShadow::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    CRect rc = UIGetClientRect();
    Universal::UI::Paint::DrawImage9Grid(dc, rc, m_hBmp, Draw9Center());
}

//////////////////////////////////////////////////////////////////////////

CUIBaseWindow::CUIBaseWindow()
{
    m_pShadow = std::make_unique<CUIBaseWindowShadow>();
    m_pTitleBar = std::make_unique<CUIBaseTitleBar>(this);
}

CUIBaseWindow::~CUIBaseWindow()
{
    m_pTitleBar->UIDestroy();
    m_pShadow->UIDestroy();
}

int CUIBaseWindow::GetTitleBarHeight()
{
    return TITLE_HEIGHT;
}

void CUIBaseWindow::OnClose()
{
    PostMessage(WM_CLOSE);
}

void CUIBaseWindow::UpdateBorder()
{
    CRect rc;
    GetWindowRect(&rc);
    CSize sz = m_pShadow->GetPreferredSize();
    DWORD dwFlag = SWP_NOZORDER | SWP_NOACTIVATE;
    if (GetWindowLong(GWL_STYLE) & WS_MAXIMIZE)
        dwFlag |= SWP_HIDEWINDOW;
    
    if (m_pShadow->IsWindow())
    {
        m_pShadow->SetWindowPos(NULL, rc.left - sz.cx / 2, rc.top - sz.cy / 2,
            rc.Width() + sz.cx, rc.Height() + sz.cy, dwFlag);
        m_pShadow->UpdateWindow();
    }
}

LRESULT CUIBaseWindow::OnThemeDrawTitle(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

LRESULT CUIBaseWindow::OnThemeDrawFrame(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

LRESULT CUIBaseWindow::OnNCActive(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient = { 0 };
    GetClientRect(&rcClient);
    InvalidateRect(&rcClient, TRUE);
    ::PostMessage(m_hWnd, WM_NCPAINT, NULL, NULL);
    return TRUE;
}

LRESULT CUIBaseWindow::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return S_OK;
}

void CUIBaseWindow::OnWindowPosChanged(LPWINDOWPOS lpWndPos)
{
    SetMsgHandled(FALSE);
    if ((lpWndPos->flags & (SWP_NOMOVE | SWP_NOSIZE)) != (SWP_NOSIZE | SWP_NOMOVE))
    {
        UpdateBorder();
    }
}

void CUIBaseWindow::OnUICreate()
{
    CSize sz = m_pShadow->GetPreferredSize();
    m_pShadow->Create(m_hWnd, CRect(0, 0, sz.cx, sz.cy), L"border",
        WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    m_pTitleBar->UICreate(this);
    UIGetMgr()->SetDefaultFont(CommonUI::UI::GetDefaultFont());
}

void CUIBaseWindow::OnUISize()
{
    CRect rcClient = UIGetClientRect();
    m_pTitleBar->UISetPos(0, 0, rcClient.Width(), TITLE_HEIGHT);
}

LRESULT CUIBaseWindow::OnWndClose(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DestroyWindow();
    PostQuitMessage(0);
    return 0;
}

LRESULT CUIBaseWindow::OnWndDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UIGetMgr()->Destroy();
    return 0;
}
