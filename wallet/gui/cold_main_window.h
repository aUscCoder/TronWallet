#pragma once
#include "wallet_base_window.h"
#include "common_ui/ControlPane.h"
#include "core/transaction_manager.h"
#include "show_panel.h"
#include "status_layer.h"
#include "address_list_form.h"
#include "account_manager_bar.h"
#include "register_form.h"
#include "login_form.h"

using namespace wallet_core;

namespace wallet_gui
{
    class CColdWalletMainWindow : public CWalletBaseWindow
    {
    public:
        static CColdWalletMainWindow* Get();
        CColdWalletMainWindow();

    protected:
        void OnUICreate() override;
        LRESULT OnAccountBarLogin(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
        LRESULT OnUIBTNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

    private:
        void ShowAddressList();
        
    private:
        CAddressListForm address_list_form_;
    };
}