#pragma once
#include "common_ui/BaseWindow.h"
#include "common_ui/ControlPane.h"
#include "account_manager_bar.h"
#include "register_form.h"
#include "show_panel.h"
#include "login_form.h"
#include "status_layer.h"
#include "loaded_form.h"
#include "core/transaction_manager.h"

using namespace wallet_core;

namespace wallet_gui
{
    class CWalletBaseWindow : public CUIBaseWindow
    {
    public:
        CWalletBaseWindow();
    protected:
        BEGIN_MSG_MAP_EX(CWalletBaseWindow)
            MESSAGE_HANDLER_EX(Event_Network_Response, OnNetCallback)
            MESSAGE_HANDLER_EX(Event_Show_Register, OnShowRegister)
            MESSAGE_HANDLER_EX(Event_Show_Login, OnShowLogin)
            MESSAGE_HANDLER_EX(Event_AccountBar_Login, OnAccountBarLogin)
            MESSAGE_HANDLER_EX(Event_Show_Status, OnShowStatus)
            MESSAGE_HANDLER_EX(Event_Hide_Status, OnHideStatus)
            CHAIN_MSG_MAP(CUIBaseWindow)
        END_MSG_MAP()

        LRESULT OnNetCallback(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnShowRegister(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnShowLogin(UINT uMsg, WPARAM wParam, LPARAM lParam);
        virtual LRESULT OnAccountBarLogin(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnShowStatus(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnHideStatus(UINT uMsg, WPARAM wParam, LPARAM lParam);

        BEGIN_UI_EVENT_MAP(CWalletBaseWindow)
            UI_EVENT_PAINT(OnUIPaint)
            UI_EVENT_CREATE(OnUICreate)
            UI_EVENT_SIZE(OnUISize)
        END_UI_EVENT_MAP()

        virtual void OnUIPaint(CDCHandle dc, DWORD dwStatus);
        virtual void OnUICreate();
        virtual void OnUISize();

        void ShowStatus(const std::wstring& msg);
        void HideStatus(const std::wstring& msg);
        void LoadTransaction(TransactionStatus s, const std::wstring& title, const std::wstring& prefix);
        void LoadVotes(TransactionStatus s);

    protected:
        CUIImage logo_;
        CUIControlPane toolbar_;
        CAccountBar account_bar_;
        CShowPanel show_panel_;
        CRegisterForm register_form_;
        CLoginForm login_form_;
        CStatusLayer status_;
        std::unique_ptr<CUIObject> loaded_form_;
    };
}

