#pragma once
#include "base_form.h"
#include "freeze_info_form.h"

namespace protocol
{
    class FreezeBalanceContract;
}

namespace wallet_gui
{
    class CFreezeForm : public CBaseForm
    {
    public:
        CFreezeForm(bool offline = false);
        void SetAddress(const std::string& address) { address_ = address; }

        BEGIN_UI_EVENT_MAP(CFreezeForm)
            UI_EVENT_SHOWUI(OnShowUI)
        END_UI_EVENT_MAP()

    protected:
        void OnUICreate() override;
        void OnShowUI(bool show);
        void OnButtonClick(DWORD dwControlID) override;

    private:
        std::shared_ptr<protocol::FreezeBalanceContract> CreateContract();

    private:
        bool offline_ = false;
        std::string address_;
        std::unique_ptr<CTransactionInfoForm> freeze_info_;
    };
}
