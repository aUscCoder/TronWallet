#pragma once
#include "CommonInclude.h"
#include "UILite/Controls/UIControls.h"
#include "UILite/UICrack.h"


class CUIImageButton;
class CUIBaseWindow;
class CUIBaseTitleBar : public CUITitlebar
{
public:

    CUIBaseTitleBar(CUIBaseWindow* pWindow);
    ~CUIBaseTitleBar();

protected:

    BEGIN_UI_EVENT_MAP(CUIBaseTitleBar)
        UI_EVENT_CREATE(OnUICreate)
        UI_EVENT_SIZE(OnUISize)
        UI_COMMAND_CODE(UIBN_CLICK, OnBNClick)
    END_UI_EVENT_MAP()
    void OnUICreate();
    void OnUISize();
    LRESULT OnBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

private:
    std::unique_ptr<CUIImageButton> m_pBtnMin;
    std::unique_ptr<CUIImageButton> m_pBtnMax;
    std::unique_ptr<CUIImageButton> m_pBtnClose;
    std::unique_ptr<CUIImageButton> m_pBtnRestore;
    CUIBaseWindow* m_pWindow = nullptr;
};