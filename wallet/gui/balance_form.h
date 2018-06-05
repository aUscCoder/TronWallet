#pragma once
#include "base_form.h"
#include "UILite/Controls/UIEdit.h"
#include "UILite/Controls/UIButton.h"
#include "UILite/Controls/ListView/UIListView.h"
#include "core/Tron.pb.h"
#include "api/api.pb.h"

namespace protocol { class Account; }

namespace wallet_gui
{
    class CBalanceForm;

    class CWalletBaseInfoForm : public CBaseForm
    {
    public:
        CWalletBaseInfoForm(bool offline = false);
        void UpdateBandwidth(std::shared_ptr<protocol::AccountNetMessage> net_msg);

    protected:

        void OnUICreate() override;
        void Layout() override;
        void OnButtonClick(DWORD dwControlID) override;
        void OnShowUI(bool show);

    private:
        std::unique_ptr<CUIStatic> head_;
        std::unique_ptr<CUIEdit> address_;
        std::unique_ptr<CUIEdit> private_key_;
        std::unique_ptr<CUIButton> show_private_;
        std::unique_ptr<CUIStatic> bandwidth_label_;
        std::unique_ptr<CUIEdit> bandwidth_;
        friend class CBalanceForm;
    };

    class CTokenInfoForm : public CBaseForm
    {
    public:
        CTokenInfoForm(bool offline = false);
        void OnUICreate() override;
        void Layout() override;
        void OnUIPaint(CDCHandle dc, DWORD dwStatus) override;
        void UpdateBalance(std::shared_ptr<protocol::Account> account);


    private:
        std::unique_ptr<CUIListView> tokens_;
    };

    class CBalanceForm : public CBaseForm
    {
    public:
        CBalanceForm(bool offline = false);
        void SetAddress(const std::string& address);
    protected:

        BEGIN_UI_EVENT_MAP(CBalanceForm)
            UI_EVENT_SHOWUI(OnShowUI)
        END_UI_EVENT_MAP()
 
        void OnUICreate() override;
        void Layout() override;
        void OnUIPaint(CDCHandle dc, DWORD dwStatus) override;
        void OnShowUI(bool show);

    private:
        void UpdateBalance(std::shared_ptr<protocol::Account> account);

    private:
        bool offline_ = false;
        std::string address_;
        CWalletBaseInfoForm account_info_form_;
        CTokenInfoForm token_info_form_;
    };
}
