#pragma once
#include "CommonInclude.h"
#include "base_form.h"

namespace wallet_gui
{
    class CRegisterForm : public CBaseForm
    {
    public:
        CRegisterForm();

    protected:
        BEGIN_UI_EVENT_MAP(CLoginForm)
            UI_EVENT_SHOWUI(OnShowUI)
        END_UI_EVENT_MAP()

        void OnShowUI(bool show);
        void OnUICreate() override;
        void OnButtonClick(DWORD dwControlID) override;
        void OnUIPaint(CDCHandle dc, DWORD dwStatus) override;
        LRESULT OnReturn(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader) override;
        std::tuple<std::string, std::string> GetPassword();

    private:
        void OnOk();
    };
}



