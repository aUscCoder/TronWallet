#include "cold_main_window.h"
#include "core/account_manager.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "loaded_form.h"
#include "core/account_manager.h"
#include "core/network_thread.h"

using namespace wallet_gui;
using namespace CommonUI;

#define TOOLBAR_HEIGHT 46
#define ACCOUNT_MANAGER_WIDTH 300

enum class ControlID
{
    BTN_COLD_WALLET_LOAD_TRANSACTION = 2000,
    BTN_COLD_WALLET_LOAD_FREEZE,
    BTN_COLD_WALLET_ADDRESS_LIST,
    BTN_COLD_AUTHORIZE_VOTING,
};

CColdWalletMainWindow* CColdWalletMainWindow::Get()
{
    static CColdWalletMainWindow wnd;
    return &wnd;
}

CColdWalletMainWindow::CColdWalletMainWindow()
{
}

void CColdWalletMainWindow::OnUICreate()
{
    AccountManager::_().SetWalletType(ColdWallet);
    CWalletBaseWindow::OnUICreate();
    auto RegisterItem = [this](const std::vector<std::pair<std::wstring, ControlID>>& items, CUIControlPane& pane) {
        using namespace std::placeholders;
        pane.SetClickCallback(std::bind(&CColdWalletMainWindow::OnUIBTNClick, this, _1, _2, _3));
        for (const auto& p : items)
            pane.AddButton(CommonUI::CreateTextColorButton(p.first, (DWORD)p.second));
    };

    static std::vector<std::pair<std::wstring, ControlID>> toolbar_btns =
    {
        { L"Load Transaction", ControlID::BTN_COLD_WALLET_LOAD_TRANSACTION },
        { L"Load Freeze", ControlID::BTN_COLD_WALLET_LOAD_FREEZE },
        { L"Address List", ControlID::BTN_COLD_WALLET_ADDRESS_LIST },
        { L"Authorize Voting", ControlID::BTN_COLD_AUTHORIZE_VOTING },
    };

    RegisterItem(toolbar_btns, toolbar_);
    auto UICreateShowPanelSub = [this](CUIObject* obj) {
        obj->UIShow(false);
        obj->UICreate(&show_panel_);
    };

    UICreateShowPanelSub(&address_list_form_);
    UICreateShowPanelSub(&register_form_);
    UICreateShowPanelSub(&login_form_);
}

LRESULT CColdWalletMainWindow::OnUIBTNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    auto& manage = AccountManager::_();
    ControlID id = (ControlID)dwControlID;
    if (!manage.CheckLogin())
        return S_OK;

    switch (id)
    {
    case ControlID::BTN_COLD_WALLET_LOAD_TRANSACTION:
        AccountManager::_().LoadAddresses();
        LoadTransaction(TransactionStatus::SIGN, L"Transaction ID", L"transaction");
        break;
    case ControlID::BTN_COLD_WALLET_LOAD_FREEZE:
        AccountManager::_().LoadAddresses();
        LoadTransaction(TransactionStatus::SIGN, L"Freeze ID", L"freeze");
        break;
    case ControlID::BTN_COLD_WALLET_ADDRESS_LIST:
        ShowAddressList();
        break;
    case ControlID::BTN_COLD_AUTHORIZE_VOTING:
        AccountManager::_().LoadAddresses();
        LoadVotes(TransactionStatus::SIGN);
        break;
    }
    return S_OK;
}

void CColdWalletMainWindow::ShowAddressList()
{
    auto addresses = AccountManager::_().GetAddresses();
    address_list_form_.Update(addresses);
    show_panel_.SetContent(&address_list_form_);
}

LRESULT CColdWalletMainWindow::OnAccountBarLogin(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWalletBaseWindow::OnAccountBarLogin(uMsg, wParam, lParam);
    ShowAddressList();
    return S_OK;
}
