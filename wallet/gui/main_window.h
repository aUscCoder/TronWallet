#pragma once
#include "wallet_base_window.h"
#include "menu.h"
#include "send_form.h"
#include "transactions_form.h"
#include "receive_form.h"
#include "status_layer.h"
#include "balance_form.h"
#include "wallet_evnets.h"
#include "token_issue_form.h"
#include "vote_form.h"
#include "token_list_form.h"
#include "participate_form.h"
#include "account_manager_bar.h"
#include "freeze_form.h"
#include "transaction_info_form.h"
#include "address_text_form.h"
#include "addresses_action_form.h"
#include "common_ui/SizingBorder.h"
#include "connection_form.h"
#include "token_modify_form.h"

namespace wallet_gui
{
    class CMainWindow : public CWalletBaseWindow, public CSizingBorder
    {
    public:
        static CMainWindow* Get();
        CMainWindow();

    protected:
        BEGIN_MSG_MAP_EX(CMainWindow)
            MESSAGE_HANDLER_EX(Event_Show_Participate, OnShowParticipate)
            MESSAGE_HANDLER_EX(Event_Show_Modify_Token, OnShowTokenModify)
            MESSAGE_HANDLER_EX(Event_Show_MessageBox, OnShowMessage)
            MESSAGE_HANDLER_EX(Event_BandWidth_Not_Enough, OnBandWidthNotEnough)
            MESSAGE_HANDLER_EX(Event_Show_BalanceList, OnShowBalanceList)
            MESSAGE_HANDLER_EX(Event_Show_Freeze, OnShowFreeze)
            CHAIN_MSG_MAP(CSizingBorder)
            CHAIN_MSG_MAP(CWalletBaseWindow)
        END_MSG_MAP()

        LRESULT OnShowParticipate(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnShowTokenModify(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnShowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnBandWidthNotEnough(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnShowBalanceList(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnShowFreeze(UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT OnAccountBarLogin(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

        void OnUICreate() override;
        void OnUISize() override;
        LRESULT OnUIBTNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
        LRESULT OnMouseEnter(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

    private:
        void ShowTransactions(const std::string& address);
        void ShowBalanceList();
        void ShowTokenList();
        void ShowMyTokenList(const std::string& address);
        void UnFreezeToken(const std::string& address);
        void ImportAddress();
        void ShowMenu(DWORD id, bool show);

    private:
        std::unique_ptr<CMenu> account_menu_;
        std::unique_ptr<CMenu> tokens_menu_;
        std::unique_ptr<CMenu> offline_menu_;
        std::unique_ptr<CMenu> watching_wallet_menu_;

        CBalanceForm balance_form_;
        CReceiveForm receive_form_;
        CSendForm send_form_;
        CTransactionsForm transactions_form_;
        CVoteForm vote_form_;
        CTokenIssueForm issue_form_;
        CTokenListForm token_list_form_;
        CFreezeForm freeze_form_;
        CParticipateForm participate_form_;
        CSendActionForm offline_send_form_;
        CVoteActionForm offline_vote_form_;
        CFreezeActionForm offline_freeze_form_;
        CBalanceActionForm balance_list_form_;
        CConnectionForm connection_form_;
        CTokenListForm mytoken_list_form_;
        CTokenModifyForm token_modify_form_;

        std::unique_ptr<CAddressTextForm> address_import_form_;
        std::set<std::string> imported_address_;
    };
}
