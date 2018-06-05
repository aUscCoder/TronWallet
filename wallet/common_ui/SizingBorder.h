// Author: TuotuoXP
#pragma once
#include "CommonInclude.h"

class CUIRoot;
class CSizingBorder
{

public:

    CSizingBorder(CUIRoot* pRoot) : m_bByPassClickOnTopBorderWhenMaximize(false), m_pRoot(pRoot) {}
    ~CSizingBorder() {}

    void ResetSizingWnd() { m_wndSizingBorder = NULL; }

    void CalcCursorAndHittest(HWND hWnd, POINT pt, ULONG &uHitTest, LPCWSTR &uMouseCursor);

    void SetBypassClickOnTopBorderWhenMaximize(bool bFlag) { m_bByPassClickOnTopBorderWhenMaximize = bFlag; }
    bool OnClickOnTopBorderWhenMaximize(POINT pt);

    BEGIN_MSG_MAP(CSizingBorder)
        ACQUIRE_HWND(m_wndSizingBorder)
        MESSAGE_HANDLER(0xAE, OnThemeDrawTitle) //WM_NCUAHDRAWCAPTION
        MESSAGE_HANDLER(0xAF, OnThemeDrawFrame) //WM_NCUAHDRAWFRAME
        MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
        MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)

        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    END_MSG_MAP()

    LRESULT OnThemeDrawTitle(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) { return 0; }
    LRESULT OnThemeDrawFrame(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) { return 0; }
    LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) { return 0; }
    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:

    bool m_bByPassClickOnTopBorderWhenMaximize;

    CWindow m_wndSizingBorder;
    CUIRoot* m_pRoot = nullptr;
};
