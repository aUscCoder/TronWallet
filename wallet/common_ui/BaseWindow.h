#pragma once
#include "CommonInclude.h"
#include "UILite/WTLWrapper.h"
#include "SizingBorder.h"


class CUIBaseWindowShadow;
class CUIBaseTitleBar;
class CUIBaseWindow : public CUILiteWindowImpl<CUIBaseWindow>
{
public:
    CUIBaseWindow();
    ~CUIBaseWindow();
    int GetTitleBarHeight();
    virtual void OnClose();

private:
    BEGIN_MSG_MAP_EX(CUIBaseWindow)
        MESSAGE_HANDLER_EX(WM_NCACTIVATE, OnNCActive)
        MESSAGE_HANDLER_EX(WM_NCCALCSIZE, OnNcCalcSize)
        MESSAGE_HANDLER_EX(WM_DESTROY, OnWndDestroy)
        MESSAGE_HANDLER_EX(WM_CLOSE, OnWndClose)
        MSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
        CHAIN_MSG_MAP(CUILiteWindowImpl<CUIBaseWindow>)
    END_MSG_MAP()
    LRESULT OnThemeDrawTitle(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnThemeDrawFrame(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnNCActive(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnWndDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnWndClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnWindowPosChanged(LPWINDOWPOS lpWndPos);

    BEGIN_UI_EVENT_MAP(CUIBaseWindow)
        UI_EVENT_CREATE(OnUICreate)
        UI_EVENT_SIZE(OnUISize)
    END_UI_EVENT_MAP()
    void OnUICreate();
    void OnUISize();
    void UpdateBorder();

protected:
    std::unique_ptr<CUIBaseTitleBar> m_pTitleBar;
    std::unique_ptr<CUIBaseWindowShadow> m_pShadow;
};