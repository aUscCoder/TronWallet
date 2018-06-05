#pragma once
#include "base_form.h"
#include "common_ui/PlaceHolderEdit.h"
#include "UILite/Controls/UIButton.h"

namespace wallet_gui
{
    class CLoginForm : public CBaseForm
    {
    public:
        CLoginForm();

    protected:

        BEGIN_UI_EVENT_MAP(CLoginForm)
            UI_EVENT_SHOWUI(OnShowUI)
        END_UI_EVENT_MAP()

        void OnShowUI(bool show);
        void OnUICreate() override;
        void Layout() override;
        void OnButtonClick(DWORD dwControlID) override;
        void OnUIPaint(CDCHandle dc, DWORD dwStatus) override;
        LRESULT OnReturn(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader) override;

    private:
        void OnOk();

    private:
        std::unique_ptr<CUIEdit> password_;
        std::unique_ptr<CUIButton> login_;
        std::unique_ptr<CUIButton> cancel_;
        std::unique_ptr<CUIButton> register_;
    };
}

