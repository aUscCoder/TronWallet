#include "wallet_base_window.h"
#include "common_ui/UI.h"
#include "common_ui/SkinDef.h"
#include "core/network_thread.h"
#include "core/account_manager.h"

#define TOOLBAR_HEIGHT   36
#define LINE_MARGIN      15
#define ACCOUNT_MANAGER_WIDTH 300

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

CWalletBaseWindow::CWalletBaseWindow()
{
    logo_.SetBitmap(Skin::GetSkinBmp(SkinDef::Logo));
}

LRESULT CWalletBaseWindow::OnNetCallback(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    std::function<void()>* callback = (std::function<void()>*)lParam;
    if (callback)
    {
        (*callback)();
        delete callback;
    }
    return 0;
}

LRESULT CWalletBaseWindow::OnShowRegister(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    show_panel_.SetContent(&register_form_);
    return 0;
}

LRESULT CWalletBaseWindow::OnShowLogin(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    show_panel_.SetContent(&login_form_);
    return 0;
}

LRESULT CWalletBaseWindow::OnAccountBarLogin(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    account_bar_.ShowSignOut(!!wParam);
    if (!wParam)
        show_panel_.SetContent(!!lParam ? (CUIObject*)&login_form_ : &register_form_);
    return 0;
}

void CWalletBaseWindow::ShowStatus(const std::wstring& msg)
{
    status_.Update(msg);
}

LRESULT CWalletBaseWindow::OnShowStatus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    std::wstring* status = (std::wstring*)wParam;
    ShowStatus(*status);
    delete status;
    return 0;
}

void CWalletBaseWindow::HideStatus(const std::wstring& msg)
{
    status_.Show(msg);
}

LRESULT CWalletBaseWindow::OnHideStatus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    std::wstring* status = (std::wstring*)wParam;
    if (status)
    {
        HideStatus(*status);
        delete status;
    }
    else
    {
        HideStatus(L"");
    }
    return 0;
}

void CWalletBaseWindow::LoadTransaction(TransactionStatus s, const std::wstring& title, const std::wstring& prefix)
{
    auto OnOK = [this, s, &title, &prefix](const std::vector<std::string>& pathes)
    {
        std::vector<std::shared_ptr<protocol::Transaction>> transactions;
        transactions.reserve(pathes.size());
        for (const auto& p : pathes)
        {
            auto transaction = wallet_core::LoadTransaction(p);
            if (transaction)
                transactions.push_back(transaction);
        }

        if (transactions.size() > 0)
        {
            if (loaded_form_)
                loaded_form_->UIDestroy();
            using DataType = std::shared_ptr<protocol::Transaction>;
            auto form = new CLoadedForm<DataType>(title, prefix, s);
            form->UICreate(&show_panel_);
            form->LoadDatas(transactions);
            show_panel_.SetContent(form);
            loaded_form_.reset(form);

        }
    };

    OpenFileDlg(m_hWnd, L"json", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT,
        L"*.json", OnOK, nullptr);
}

void CWalletBaseWindow::LoadVotes(TransactionStatus s)
{
    auto OnOK = [this, s](const std::vector<std::string>& pathes)
    {
        std::vector<VoteData> vote_datas;
        vote_datas.reserve(pathes.size());

        for (const auto& p : pathes)
        {
            auto vote_data = wallet_core::LoadVote(p);
            if (vote_data.transaction)
                vote_datas.push_back(std::move(vote_data));
        }

        if (vote_datas.size() > 0)
        {
            using DataType = VoteData;
            auto form = new CLoadedForm<DataType>(L"Authorize voting", L"Vote", s);
            form->UICreate(&show_panel_);
            form->LoadDatas(vote_datas);
            show_panel_.SetContent(form);
            if (loaded_form_)
                loaded_form_->UIDestroy();
            loaded_form_.reset(form);
        }
    };

    OpenFileDlg(m_hWnd, L"json", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT,
        L"*.json", OnOK, nullptr);
}

void CWalletBaseWindow::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    CRect client_rect;
    GetClientRect(client_rect);
    auto head_client = client_rect;
    head_client.bottom = toolbar_.UIGetHeight() + GetTitleBarHeight();
    dc.FillSolidRect(head_client, CommonUI::middle_black);

    auto body_client = client_rect;
    body_client.top = head_client.bottom;
    dc.FillSolidRect(body_client, CommonUI::light_black);
}

void CWalletBaseWindow::OnUICreate()
{
    CNetworkThread::_().SetCallbackWnd(m_hWnd);
    AccountManager::_().SetCallbackWnd(m_hWnd);
    UISetHandled(false);
    logo_.UICreate(this);

    toolbar_.SetUseLargestSize(false);
    toolbar_.UICreate(this);
    account_bar_.UICreate(this);
    show_panel_.UICreate(this);

    status_.UICreate(this);
    status_.UIShow(false);
}

void CWalletBaseWindow::OnUISize()
{
    UISetHandled(false);
    CRect rc;
    GetClientRect(rc);
    int top = GetTitleBarHeight();
    int toolbar_left = CommonUI::kDefaultMargin;
    auto logo_size = logo_.GetSize();
    logo_.UISetPos(toolbar_left, top, logo_size.cx, logo_size.cy);

    toolbar_left += logo_size.cx + CommonUI::kDefaultMargin;
    toolbar_.UISetPos(toolbar_left, top, rc.right - toolbar_left, TOOLBAR_HEIGHT);
    int account_manager_left = rc.right - CommonUI::kDefaultMargin - ACCOUNT_MANAGER_WIDTH;
    account_bar_.UISetPos(account_manager_left, top, ACCOUNT_MANAGER_WIDTH, TOOLBAR_HEIGHT);

    top += TOOLBAR_HEIGHT;

    show_panel_.UISetPos(0, top, rc.Width(), rc.Height() - top);
    status_.UISetPos(0, GetTitleBarHeight(), rc.Width(), rc.Height() - GetTitleBarHeight());
}
