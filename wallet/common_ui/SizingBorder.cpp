#include "SizingBorder.h"
#include "UILite/Core/UIRoot.h"


void CSizingBorder::CalcCursorAndHittest(HWND hWnd, POINT pt, ULONG &uHitTest, LPCWSTR &uMouseCursor)
{
    RECT rcWnd;
    ::GetWindowRect(hWnd, &rcWnd);
    const int iBorder = 10;
    const int iResizeRange = 10;

    uHitTest = HTCLIENT;
    uMouseCursor = IDC_ARROW;
    if (pt.x <= rcWnd.left + iBorder)
    {
        if (pt.y <= rcWnd.top + iResizeRange)
        {
            uHitTest = HTTOPLEFT;
            uMouseCursor = IDC_SIZENWSE;
        }
        else if (pt.y >= rcWnd.bottom - iResizeRange)
        {
            uHitTest = HTBOTTOMLEFT;
            uMouseCursor = IDC_SIZENESW;
        }
        else
        {
            uHitTest = HTLEFT;
            uMouseCursor = IDC_SIZEWE;
        }
    }
    else if (pt.x >= rcWnd.right - iBorder)
    {
        if (pt.y <= rcWnd.top + iResizeRange)
        {
            uHitTest = HTTOPRIGHT;
            uMouseCursor = IDC_SIZENESW;
        }
        else if (pt.y >= rcWnd.bottom - iResizeRange)
        {
            uHitTest = HTBOTTOMRIGHT;
            uMouseCursor = IDC_SIZENWSE;
        }
        else
        {
            uHitTest = HTRIGHT;
            uMouseCursor = IDC_SIZEWE;
        }
    }
    else if (pt.y <= rcWnd.top + iBorder)
    {
        if (pt.x <= rcWnd.left + iResizeRange)
        {
            uHitTest = HTTOPLEFT;
            uMouseCursor = IDC_SIZENWSE;
        }
        else if (pt.x >= rcWnd.right - iResizeRange)
        {
            uHitTest = HTTOPRIGHT;
            uMouseCursor = IDC_SIZENESW;
        }
        else
        {
            uHitTest = HTTOP;
            uMouseCursor = IDC_SIZENS;
        }
    }
    else if (pt.y >= rcWnd.bottom - iBorder)
    {
        if (pt.x <= rcWnd.left + iResizeRange)
        {
            uHitTest = HTBOTTOMLEFT;
            uMouseCursor = IDC_SIZENESW;
        }
        else if (pt.x >= rcWnd.right - iResizeRange)
        {
            uHitTest = HTBOTTOMRIGHT;
            uMouseCursor = IDC_SIZENWSE;
        }
        else
        {
            uHitTest = HTBOTTOM;
            uMouseCursor = IDC_SIZENS;
        }
    }
}

bool CSizingBorder::OnClickOnTopBorderWhenMaximize(POINT pt)
{
    if (m_bByPassClickOnTopBorderWhenMaximize)
    {
        m_wndSizingBorder.ClientToScreen(&pt);

        CWindow hRoot = ::GetAncestor(m_wndSizingBorder, GA_ROOTOWNER);
        // 双屏下，副屏幕浏览器最大化时，标签页不能切换，在此取差值
        HMONITOR hMonitor = ::MonitorFromWindow(hRoot.m_hWnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO info = { sizeof(MONITORINFO) };
        ::GetMonitorInfo(hMonitor, &info);
        if ((pt.y - info.rcWork.top) < UI_SCALE(3))
        {
            if (hRoot.GetStyle() & WS_MAXIMIZE)
            {
                ::SendMessage(hRoot, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x, pt.y));
                return true;
            }
        }
    }
    return false;
}

LRESULT CSizingBorder::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return HTCLIENT;
}

LRESULT CSizingBorder::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    ::ClientToScreen(m_wndSizingBorder, &pt);

    HWND hCapture = ::GetCapture();
    CWindow hRoot = ::GetAncestor(m_wndSizingBorder, GA_ROOTOWNER);
    if (hCapture != hRoot && hCapture != NULL)
    {
        bHandled = FALSE;
        return 0;
    }

    if (hRoot.GetStyle() & WS_MAXIMIZE)
    {
        bHandled = FALSE;
        return 0;
    }

    ULONG uHitTest = HTCLIENT;
    LPCWSTR uMouseCursor = IDC_ARROW;
    CalcCursorAndHittest(hRoot, pt, uHitTest, uMouseCursor);
    bHandled = uMouseCursor != IDC_ARROW;
    if (bHandled)
        ::SetCursor(::LoadCursor(NULL, uMouseCursor));
    return 0;
}

LRESULT CSizingBorder::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    POINT pt;
    ::GetCursorPos(&pt);
    CWindow hRoot = ::GetAncestor(m_wndSizingBorder, GA_ROOTOWNER);
    if (hRoot.GetStyle() & WS_MAXIMIZE)
    {
        bHandled = FALSE;
        return 0;
    }

    ULONG uHitTest = HTCLIENT;
    LPCWSTR uMouseCursor = IDC_ARROW;
    CalcCursorAndHittest(hRoot, pt, uHitTest, uMouseCursor);
    SetCursor(::LoadCursor(NULL, uMouseCursor));

    bHandled = uMouseCursor != IDC_ARROW;
    if (uHitTest != HTCLIENT)
    {
        ::SendMessage(hRoot, WM_NCLBUTTONDOWN, uHitTest, MAKELPARAM(pt.x, pt.y));
    }
    return 0;
}
