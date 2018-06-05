#pragma once
#include "common_ui/BaseWindow.h"
#include "UILite/Controls/UIComboBox.h"
#include "UILite/Controls/UIButton.h"

namespace wallet_gui
{
    class CSelectWalletWindow : public CUIBaseWindow
    {
    public:
        CSelectWalletWindow();
        ~CSelectWalletWindow();

    protected:
        BEGIN_UI_EVENT_MAP(CSelectWalletWindow)
            UI_EVENT_CREATE(OnUICreate)
            UI_EVENT_SIZE(OnUISize)
            UI_EVENT_PAINT(OnUIPaint)
            UI_COMMAND_CODE(UIBN_CLICK, OnUIBNClick)
        END_UI_EVENT_MAP()

        void OnUICreate();
        void OnUISize();
        void OnUIPaint(CDCHandle dc, DWORD dwStatus);
        LRESULT OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

    private:
        std::unique_ptr<CUIRadio> hot_radio_;
        std::unique_ptr<CUIRadio> cold_radio_;
        std::unique_ptr<CUIButton> load_;
    };

}
