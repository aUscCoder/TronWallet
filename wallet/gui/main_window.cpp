#include "main_window.h"
#include <functional>
#include "base/macros.h"
#include "base/utf_string_conversions.h"
#include "common_ui/CommonDef.h"
#include "common_ui/UI.h"
#include "UILite/Controls/UIButton.h"
#include "core/network_thread.h"
#include "core/account_manager.h"
#include "libdevcore/CommonData.h"
#include "core/core_util.h"
#include "common_ui/CommonDef.h"
#include "api/api.pb.h"
#include "core/transaction_manager.h"
#include "loaded_form.h"

#define TOOLBAR_HEIGHT   46
#define LINE_MARGIN      15
#define ACCOUNT_MANAGER_WIDTH 300

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

enum class ControlID
{
    BTN_ACCOUNT = 1,
    BTN_VOTE,
    BTN_TOKENS,
    BTN_OFFLINE_TRANSACTION,
    BTN_WATCHING_WALLET,
    BTN_CONNECTION,

    BTN_BALANCELIST,
    BTN_SEND,
    BTN_RECEIVE,
    BTN_TRANSACTIONS,

    BTN_OFFLINE_SEND,
    BTN_OFFLINE_VOTE,
    BTN_OFFLINE_FREEZE,
    BTN_OFFLINE_LOAD_TRANSACTION,
    BTN_OFFLINE_LOAD_VOTES,
    BTN_OFFLINE_LOAD_FREEZE,

    BTN_WATCHING_WALLET_IMPORT,
    BTN_WATCHING_WALLET_BALANCE_LIST,

    BTN_TOKEN_ISSUE,
    BTN_TOKEN_LIST,
    BTN_MY_TOKEN,

    COUNT,
};

CMainWindow* CMainWindow::Get()
{
    static CMainWindow wnd;
    return &wnd;
}

CMainWindow::CMainWindow() : CSizingBorder(this), mytoken_list_form_(false)
{
    static std::vector<std::pair<std::wstring, DWORD>> account_items = {
        { L"Balance List", (DWORD)ControlID::BTN_BALANCELIST },
        { L"Send", (DWORD)ControlID::BTN_SEND },
        { L"Receive", (DWORD)ControlID::BTN_RECEIVE },
        { L"Transactions", (DWORD)ControlID::BTN_TRANSACTIONS },
    };

    static std::vector<std::pair<std::wstring, DWORD>> token_items = {
        { L"Token Issue", (DWORD)ControlID::BTN_TOKEN_ISSUE },
        { L"Token List", (DWORD)ControlID::BTN_TOKEN_LIST },
        { L"My Token", (DWORD)ControlID::BTN_MY_TOKEN },
    };

    static std::vector<std::pair<std::wstring, DWORD>> offline_items = {
        { L"Send", (DWORD)ControlID::BTN_OFFLINE_SEND },
        { L"Vote", (DWORD)ControlID::BTN_OFFLINE_VOTE },
        { L"Freeze", (DWORD)ControlID::BTN_OFFLINE_FREEZE },
        { L"Load Transaction", (DWORD)ControlID::BTN_OFFLINE_LOAD_TRANSACTION },
        { L"Load Votes", (DWORD)ControlID::BTN_OFFLINE_LOAD_VOTES },
        { L"Load Freeze", (DWORD)ControlID::BTN_OFFLINE_LOAD_FREEZE },
    };

    static std::vector<std::pair<std::wstring, DWORD>> watching_wallet_items = {
        { L"Import watching-only wallet", (DWORD)ControlID::BTN_WATCHING_WALLET_IMPORT},
        { L"Balance list", (DWORD)ControlID::BTN_WATCHING_WALLET_BALANCE_LIST},
    };

    auto CreateMenu = [this](std::unique_ptr<CMenu>& menu, const decltype(account_items)& items)
    {
        using namespace std::placeholders;
        menu.reset(new CMenu(this, items));
        menu->SetBackgrounColor(RGB(238, 238, 238));
        menu->SetClickCallback(std::bind(&CMainWindow::OnUIBTNClick, this, _1, _2, _3));
    };

    CreateMenu(account_menu_, account_items);
    CreateMenu(tokens_menu_, token_items);
    CreateMenu(offline_menu_, offline_items);
    CreateMenu(watching_wallet_menu_, watching_wallet_items);
}

void CMainWindow::OnUICreate()
{
    AccountManager::_().SetWalletType(HotWallet);
    CWalletBaseWindow::OnUICreate();
    struct
    {
        std::wstring text;
        ControlID id;
        CMenu* menu;
    } toolbar_btns[] = {
        { L"Account", ControlID::BTN_ACCOUNT, account_menu_.get() },
        { L"Vote", ControlID::BTN_VOTE },
        { L"Tokens", ControlID::BTN_TOKENS, tokens_menu_.get() },
        { L"Offline Transaction", ControlID::BTN_OFFLINE_TRANSACTION, offline_menu_.get() },
        { L"Watching Wallet", ControlID::BTN_WATCHING_WALLET, watching_wallet_menu_.get() },
        { L"Connection", ControlID::BTN_CONNECTION},
    };

    auto RegisterItem = [this](const decltype(toolbar_btns)& items, CUIControlPane& pane)
    {
        using namespace std::placeholders;
        pane.SetClickCallback(std::bind(&CMainWindow::OnUIBTNClick, this, _1, _2, _3));
        pane.SetMouseEnterCallback(std::bind(&CMainWindow::OnMouseEnter, this, _1, _2, _3));
        for (const auto& i : items)
        {
            pane.AddButton(CommonUI::CreateTextColorButton(i.text, (DWORD)i.id));
            if (i.menu)
                i.menu->SetHeader(pane.GetButton((DWORD)i.id));
        }
    };

    RegisterItem(toolbar_btns, toolbar_);

    auto UICreateShowPanelSub = [this](CUIObject* obj) {
        obj->UIShow(false);
        obj->UICreate(&show_panel_);
    };

    UICreateShowPanelSub(&register_form_);
    UICreateShowPanelSub(&login_form_);
    UICreateShowPanelSub(&send_form_);
    UICreateShowPanelSub(&transactions_form_);
    UICreateShowPanelSub(&balance_form_);
    UICreateShowPanelSub(&receive_form_);
    UICreateShowPanelSub(&vote_form_);
    UICreateShowPanelSub(&issue_form_);
    UICreateShowPanelSub(&token_list_form_);
    UICreateShowPanelSub(&participate_form_);
    UICreateShowPanelSub(&freeze_form_);
    UICreateShowPanelSub(&offline_send_form_);
    UICreateShowPanelSub(&offline_vote_form_);
    UICreateShowPanelSub(&offline_freeze_form_);
    UICreateShowPanelSub(&balance_list_form_);
    UICreateShowPanelSub(&connection_form_);
    UICreateShowPanelSub(&mytoken_list_form_);
    UICreateShowPanelSub(&token_modify_form_);

    // last.
    UICreateShowPanelSub(account_menu_.get());
    UICreateShowPanelSub(tokens_menu_.get());
    UICreateShowPanelSub(offline_menu_.get());
    UICreateShowPanelSub(watching_wallet_menu_.get());
}

void CMainWindow::OnUISize()
{
    CWalletBaseWindow::OnUISize();
    auto toolbar_left = toolbar_.UIGetRect().left;
    auto SetMenuPos = [this, toolbar_left](ControlID id, CUIObject* menu)
    {
        auto btn = toolbar_.GetButton((DWORD)id);
        if (btn)
        {
            auto pos = btn->UIGetPos();
            menu->UISetPos({ pos.x + toolbar_left, 0 });
        }
    };

    SetMenuPos(ControlID::BTN_ACCOUNT, account_menu_.get());
    SetMenuPos(ControlID::BTN_TOKENS, tokens_menu_.get());
    SetMenuPos(ControlID::BTN_OFFLINE_TRANSACTION, offline_menu_.get());
    SetMenuPos(ControlID::BTN_WATCHING_WALLET, watching_wallet_menu_.get());
}

void CMainWindow::ShowMenu(DWORD id, bool show)
{
    ControlID control_id = (ControlID)id;
    switch (control_id)
    {
    case ControlID::BTN_OFFLINE_TRANSACTION:
        offline_menu_->UIShow(show);
        break;
    case ControlID::BTN_WATCHING_WALLET:
        watching_wallet_menu_->UIShow(show);
        break;
    case ControlID::BTN_ACCOUNT:
        account_menu_->UIShow(show);
        break;
    case ControlID::BTN_TOKENS:
        tokens_menu_->UIShow(show);
        break;
    }
}

LRESULT CMainWindow::OnMouseEnter(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    ShowMenu(dwControlID, true);
    return S_OK;
}

LRESULT CMainWindow::OnUIBTNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    ShowMenu(dwControlID, true);

    auto& manage = AccountManager::_();
    ControlID id = (ControlID)dwControlID;

    if (id == ControlID::BTN_SEND || id == ControlID::BTN_RECEIVE || id == ControlID::BTN_TRANSACTIONS ||
        id == ControlID::BTN_BALANCELIST || id == ControlID::BTN_TOKEN_ISSUE || id == ControlID::BTN_MY_TOKEN)
    {
        if (!manage.CheckLogin())
            return S_OK;
    }

    switch (id)
    {
    case ControlID::BTN_CONNECTION:
        show_panel_.SetContent(&connection_form_);
        break;
    case ControlID::BTN_SEND:
        send_form_.UpdteAddress(manage.Address(), [this](bool success) {
            if (success)
                show_panel_.SetContent(&send_form_);
        });
        break;
    case ControlID::BTN_RECEIVE:
        show_panel_.SetContent(&receive_form_);
        break;
    case ControlID::BTN_TRANSACTIONS:
        ShowTransactions(manage.Address());
        break;
    case ControlID::BTN_BALANCELIST:
        ShowBalanceList();
        break;
    case ControlID::BTN_VOTE:
        vote_form_.GetWitnessList([this](bool success) {
            show_panel_.SetContent(&vote_form_);
        });
        break;
    case ControlID::BTN_TOKEN_ISSUE:
        show_panel_.SetContent(&issue_form_);
        break;
    case ControlID::BTN_TOKEN_LIST:
        ShowTokenList();
        break;
    case ControlID::BTN_MY_TOKEN:
        ShowMyTokenList(manage.Address());
        break;
    case ControlID::BTN_OFFLINE_SEND:
        offline_send_form_.Update(imported_address_);
        show_panel_.SetContent(&offline_send_form_);
        break;
    case ControlID::BTN_OFFLINE_VOTE:
        offline_vote_form_.Update(imported_address_);
        show_panel_.SetContent(&offline_vote_form_);
        break;
    case ControlID::BTN_OFFLINE_FREEZE:
        offline_freeze_form_.Update(imported_address_);
        show_panel_.SetContent(&offline_freeze_form_);
        break;
    case ControlID::BTN_OFFLINE_LOAD_TRANSACTION:
        LoadTransaction(TransactionStatus::BROADCAST, L"Transaction ID", L"transaction");
        break;
    case ControlID::BTN_OFFLINE_LOAD_VOTES:
        LoadVotes(TransactionStatus::BROADCAST);
        break;
    case ControlID::BTN_OFFLINE_LOAD_FREEZE:
        LoadTransaction(TransactionStatus::BROADCAST, L"Freeze ID", L"freeze");
        break;
    case ControlID::BTN_WATCHING_WALLET_IMPORT:
        ImportAddress();
        break;
    case ControlID::BTN_WATCHING_WALLET_BALANCE_LIST:
        balance_list_form_.Update(imported_address_);
        show_panel_.SetContent(&balance_list_form_);
        break;
    }
    return S_OK;
}

void CMainWindow::ShowBalanceList()
{
    auto& manage = AccountManager::_();
    balance_form_.SetAddress(manage.Address());
}

void CMainWindow::ShowTokenList()
{
    auto em = std::make_shared<protocol::EmptyMessage>();
    CNetworkThread::_().GetAssetIssueList(em, [this](std::shared_ptr<protocol::AssetIssueList> issue_list) {
        token_list_form_.UpdateIssueList(issue_list);
        show_panel_.SetContent(&token_list_form_);
        HideStatus(L"");
    });
    ShowStatus(L"Loading token list");
}

void CMainWindow::ShowMyTokenList(const std::string& address)
{
    auto account = std::make_shared<protocol::Account>();
    account->set_address(address);
    CNetworkThread::_().GetAssetIssueByAccount(account, [this](std::shared_ptr<protocol::AssetIssueList> issue_list) {

        if (issue_list->assetissue_size() <= 0)
        {
            HideStatus(L"No token list");
        }
        else
        {
            mytoken_list_form_.UpdateIssueList(issue_list);
            show_panel_.SetContent(&mytoken_list_form_);
            HideStatus(L"");
        }
    });
    ShowStatus(L"Loading My token list");
}

void CMainWindow::ShowTransactions(const std::string& address)
{
    auto account = std::make_shared<protocol::AccountPaginated>();
    account->mutable_account()->set_address(address);
    account->set_offset(0);
    account->set_limit(1000);
    auto PushTransactionData = [](std::shared_ptr<::protocol::TransactionList> list, bool in) mutable
    {
        std::vector<TransactionData*> datas;
        for (int i = 0; i < list->transaction_size(); i++)
        {
            auto t = list->transaction(i);
            auto raw = t.raw_data();
            for (int j = 0; j < raw.contract_size(); j++)
            {
                auto contract = raw.contract(j);
                if (contract.type() == ::protocol::Transaction_Contract_ContractType::Transaction_Contract_ContractType_TransferContract)
                {
                    protocol::TransferContract transfer_contract;
                    if (contract.parameter().UnpackTo(&transfer_contract))
                    {
                        auto data = new TransactionData;
                        data->time = GetTime(raw.timestamp());
                        data->in = in;
                        data->to = base::UTF8ToWide(AddressEncodeBase58Check(transfer_contract.to_address()));
                        data->from = base::UTF8ToWide(AddressEncodeBase58Check(transfer_contract.owner_address()));
                        data->value = std::to_wstring((int)ToTrx(transfer_contract.amount()));
                        datas.push_back(data);
                    }
                }
            }
        }
        return datas;
    };

    CNetworkThread::_().GetTransactionsFromThis(account, [account, this, &PushTransactionData](std::shared_ptr<::protocol::TransactionList> list) {
        auto datas = PushTransactionData(list, false);
        transactions_form_.Push(datas);
        CNetworkThread::_().GetTransactionsToThis(account, [this, &PushTransactionData](std::shared_ptr<::protocol::TransactionList> list) {
            auto datas = PushTransactionData(list, true);
            transactions_form_.Push(datas);
            transactions_form_.UpdateTransaction();
            show_panel_.SetContent(&transactions_form_);
            HideStatus(L"");
        });
    });

    ShowStatus(L"Loading Transactions");
}

void CMainWindow::UnFreezeToken(const std::string& address)
{
    CNetworkThread::_().GetAccountWrapper(address, [address, this](std::shared_ptr<protocol::Account> account) {
        int64_t now = currentTimeMillis();
        int64_t expire_freeze_count = 0, frozen_count = 0;
        for (int i = 0; i < account->frozen_size(); i++)
        {
            auto& f = account->frozen(i);
            if (f.expire_time() <= now)
            {
                expire_freeze_count += f.frozen_balance();
            }
            frozen_count += f.frozen_balance();
        }

        if (frozen_count <= 0)
        {
            MessageBox(L"No frozen balance.");
            return;
        }

        if (expire_freeze_count <= 0)
        {
            MessageBox(L"It's not time to unfreeze.");
            return;
        }

        auto contract = std::make_shared<protocol::UnfreezeBalanceContract>();
        contract->set_owner_address(address);
        CNetworkThread::_().UnfreezeBalance(contract, [this](std::shared_ptr<protocol::Transaction> transaction) {
            if (SignTransaction(transaction))
            {
                CNetworkThread::_().BroadcastTransaction(transaction, [this](std::shared_ptr<protocol::Return> r) {
                    HideStatus(r->result() ? L"UnFreeze Token Success" : base::UTF8ToWide(r->message()));
                });
            }
            else
            {
                HideStatus(L"Create unfreeze transaction failed!");
            }
        });

        ShowStatus(L"Unfreezing");
    });
}

void CMainWindow::ImportAddress()
{
    if (address_import_form_)
        address_import_form_->UIDestroy();

    address_import_form_.reset(new CAddressTextForm(CAddressTextForm::Type::Import, [this](const std::string& address) {
        auto addresses = base::SplitString(address, "\r", base::SEPARATOR_ALL, base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);
        for (auto& a : addresses)
        {
            bool valide = false;
            std::tie(valide, std::ignore) = AddressDecodeBase58Check(a);
            if (!valide)
            {
                ::MessageBox(m_hWnd, L"Address format error.", nullptr, MB_OK);
                return;
            }
        }

        for (auto& a : addresses)
            imported_address_.insert(std::move(a));

    }));

    address_import_form_->UICreate(&show_panel_);
    show_panel_.SetContent(address_import_form_.get());
}

LRESULT CMainWindow::OnShowParticipate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto data = (IssueItemData*)wParam;
    auto encode_address = base::WideToUTF8(data->issuer);
    bool valid = false;
    std::string address;
    std::tie(valid, address) = AddressDecodeBase58Check(encode_address);
    if (valid)
    {
        CNetworkThread::_().GetAccountWrapper(address, [this, data](std::shared_ptr<protocol::Account> account) {
            auto it = account->asset().find(base::WideToUTF8(data->name));
            if (it != account->asset().end())
                data->avaliable_count = it->second;
            
            participate_form_.UpdateItemData(*data);
            show_panel_.SetContent(&participate_form_);
            delete data;
        });
    }
    else
    {
        HideStatus(base::StringPrintf(L"Address %s format error!", data->issuer.c_str()));
        delete data;
    }

    return 0;
}

LRESULT CMainWindow::OnShowTokenModify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    show_panel_.SetContent(&token_modify_form_);
    return 0;
}

LRESULT CMainWindow::OnShowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    std::wstring* msg = (std::wstring*)wParam;
    HideStatus(*msg);
    delete msg;
    return 0;
}

LRESULT CMainWindow::OnBandWidthNotEnough(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HideStatus(L"Bandwidth not enough!");
    auto address = (std::string*)wParam;
    delete address;
    return 0;
}

LRESULT CMainWindow::OnShowBalanceList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ShowBalanceList();
    return 0;
}

LRESULT CMainWindow::OnShowFreeze(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto& manager = AccountManager::_();
    if (wParam)
    {
        freeze_form_.SetAddress(manager.Address());
        show_panel_.SetContent(&freeze_form_);
    }
    else
    {
        UnFreezeToken(manager.Address());
    }
    return S_OK;
}

LRESULT CMainWindow::OnAccountBarLogin(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWalletBaseWindow::OnAccountBarLogin(uMsg, wParam, lParam);
    if (wParam)
        ShowBalanceList();
    return S_OK;
}

