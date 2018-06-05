#pragma once
#include "CommonInclude.h"
#include "UILite/Controls/UIControlEvent.h"
#include "UILite/Controls/UIControlDef.h"
#include "UILite/Core/UIObject.h"
#include "UILite/Controls/UIButton.h"
#include "UILite/UICrack.h"

namespace wallet_gui
{
    class CAccountBar : public CUIObject
    {
    public:
        CAccountBar();
        void ShowSignOut(bool show);

        BEGIN_UI_EVENT_MAP(CAccountBar)
            UI_EVENT_CREATE(OnUICreate)
            UI_EVENT_SIZE(OnUISize)
            UI_COMMAND_CODE(UIBN_CLICK, OnUIBNClick)
        END_UI_EVENT_MAP()

    protected:
        void OnUICreate();
        void OnUISize();
        LRESULT OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

    private:
        std::unique_ptr<CUIButton> btn_registe_login_;
        std::unique_ptr<CUIButton> btn_signout_;
    };
}
