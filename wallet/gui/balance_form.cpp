#include "balance_form.h"
#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "core/core_util.h"
#include "core/account_manager.h"
#include "core/network_thread.h"
#include "wallet_evnets.h"
#include "show_panel.h"

using namespace wallet_gui;
using namespace wallet_core;
using namespace CommonUI;


enum class ControlID
{
    EDIT_PRIVATE_KEY = 5000,
    BTN_SHOW_PRIVATE_KET,
    BTN_FREEZE,
    BTN_UNFREEZE,
};

CWalletBaseInfoForm::CWalletBaseInfoForm(bool offline) : CBaseForm(L"")
{
    head_.reset(new CUIStatic(L"Address", false, false, true));
    address_.reset(CreateStaticEdit(0, L""));
    bandwidth_label_.reset(new CUIStatic(L"Bandwidth", false, false, true));
    bandwidth_.reset(CreateStaticEdit(0, L""));
    if (!offline)
    {
        private_key_.reset(CreateStaticEdit((DWORD)ControlID::EDIT_PRIVATE_KEY));
        show_private_.reset(CreateNormalButton(L"Show your private key", (DWORD)ControlID::BTN_SHOW_PRIVATE_KET));
    }
}

void CWalletBaseInfoForm::OnUICreate()
{
    head_->UICreate(this);
    address_->UICreate(this);
    address_->UISetReadOnly();
    bandwidth_label_->UICreate(this);
    bandwidth_->UICreate(this);

    if (private_key_)
    {
        private_key_->UIShow(false);
        private_key_->UICreate(this);
        private_key_->UISetReadOnly();
    }

    if (show_private_)
        show_private_->UICreate(this);
}

void CWalletBaseInfoForm::Layout()
{
    CBaseForm::Layout();
    auto rc = UIGetClientRect();
    auto top = GetTitleHeight();
    top += kDefaultMargin;
    const static int height = 28;

    head_->UISetPos(kDefaultMargin + 5, top, 100, height);
    top += kDefaultMargin + height;
    address_->UISetPos(kDefaultMargin, top, 380, height);
    int left = rc.Width() - kDefaultMargin - 240;

    if (show_private_)
    {
        show_private_->UISetPos(left, top, 240, height);
        private_key_->UISetPos(rc.Width() - kDefaultMargin - 480, top, 480, height);
    }

    top += height + kDefaultMargin;
    bandwidth_label_->UISetPos(kDefaultMargin + 5, top, 100, height);
    top += height + 5;
    bandwidth_->UISetPos(kDefaultMargin, top, 200, height);
}

void CWalletBaseInfoForm::UpdateBandwidth(std::shared_ptr<protocol::AccountNetMessage> net_msg)
{
    if (net_msg)
    {
        bandwidth_->UISetText(std::to_wstring(net_msg->netlimit() - net_msg->netused()));
    }
}

void CWalletBaseInfoForm::OnButtonClick(DWORD dwControlID)
{
    if (dwControlID == (DWORD)ControlID::BTN_SHOW_PRIVATE_KET)
    {
        auto& manage = AccountManager::_();
        auto& key = manage.GetPrivate(ToAddress(manage.Address()));
        private_key_->UISetText(base::UTF8ToWide(dev::toHex(key.ref())));
        show_private_->UIShow(false);
        private_key_->UIShow(true);
        UIRedraw();
    }
}

void CWalletBaseInfoForm::OnShowUI(bool show)
{
    if (!show)
    {
        if (show_private_)
            show_private_->UIShow(true);

        if (private_key_)
        {
            private_key_->UISetText(L"");
            private_key_->UIShow(false);
        }
    }
}

class CBalanceItem : public CUIObject
{
public:
    CBalanceItem(const std::wstring& name, const std::wstring& balance, const std::wstring& freeze_amounts, bool offline = false);

protected:
    BEGIN_UI_EVENT_MAP(CBalanceItem)
        UI_EVENT_CREATE(OnUICreate)
        UI_EVENT_SIZE(OnUISize)
        UI_COMMAND_CODE(UIBN_CLICK, OnUIBNClick)
    END_UI_EVENT_MAP()

    void OnUICreate();
    void OnUISize();
    LRESULT OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

private:
    std::unique_ptr<CUIStatic> name_;
    std::unique_ptr<CUIStatic> balance_;
    std::unique_ptr<CUIStatic> freeze_amounts_;
    std::unique_ptr<CUIButton> btn_freeze_;
    std::unique_ptr<CUIButton> btn_unfreeze_;
};


CBalanceItem::CBalanceItem(const std::wstring& name, const std::wstring& balance, const std::wstring& freeze_amounts, bool offline)
{
    name_.reset(new CUIStatic(name.c_str(), false, false, true));
    balance_.reset(new CUIStatic(balance.c_str(), false, false, true));
    balance_->UIAddOrRemoveStyle(true, UIStaticStyle_Right);
    freeze_amounts_.reset(new CUIStatic(freeze_amounts.c_str(), false, false, true));
    freeze_amounts_->UIAddOrRemoveStyle(true, UIStaticStyle_Right);

    if (name == L"TRX" && !offline)
    {
        btn_freeze_.reset(CreateNormalButton(L"Freeze", (DWORD)ControlID::BTN_FREEZE));
        btn_unfreeze_.reset(CreateNormalButton(L"Unfreeze", (DWORD)ControlID::BTN_UNFREEZE));
    }
}

void CBalanceItem::OnUICreate()
{
    name_->UICreate(this);
    balance_->UICreate(this);
    freeze_amounts_->UICreate(this);
    if (btn_freeze_ && btn_unfreeze_)
    {
        btn_freeze_->UICreate(this);
        btn_unfreeze_->UICreate(this);
    }
}

LRESULT CBalanceItem::OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    if (dwControlID == (DWORD)ControlID::BTN_FREEZE)
    {
        ::PostMessage(UIGetHWND(), WalletEvent::Event_Show_Freeze, 1, 0);
    }
    else if (dwControlID == (DWORD)ControlID::BTN_UNFREEZE)
    {
        ::PostMessage(UIGetHWND(), WalletEvent::Event_Show_Freeze, 0, 0);
    }
    return S_OK;
}

void CBalanceItem::OnUISize()
{
    auto rc = UIGetClientRect();
    auto top = (rc.Height() - 28) / 2;
    name_->UISetPos(kDefaultMargin, top, 120, 28);

    int left = rc.Width() - kDefaultMargin - 160;
    freeze_amounts_->UISetPos(left, top, 160, 28);
    left -= (30 + 120);
    balance_->UISetPos(left, top, 120, 28);

    int btn_left = kDefaultMargin + 120;
    if (btn_freeze_)
    {
        btn_freeze_->UISetPos(btn_left, top, 120, 28);
        btn_left += 140;
    }

    if (btn_unfreeze_)
        btn_unfreeze_->UISetPos(btn_left, top, 120, 28);
}

struct BalanceItemData
{
    std::wstring name;
    std::wstring balance;
    std::wstring freeze_amounts;
};

class CBalanceItemListCreator : public CUIListViewSubItemCreator
{
public:
    CBalanceItemListCreator(bool offline) : offline_(offline) {}
    CUIObject* OnCreateObject(CUIListView* pView, HLISTVIEWITEM hItem, const int iCol) override
    {
        CUIObject* item = nullptr;
        int row = pView->GetItemIndex(hItem);
        if (row == 0)
        {
            item = new CBalanceItem(L"Name", L"Balance", L"Freeze amounts");
        }
        else
        {
            auto data = (BalanceItemData*)pView->GetItemData(row);
            if (data)
            {
                item = new CBalanceItem(data->name, data->balance, data->freeze_amounts, offline_);
                delete data;
            }
        }
        return item;
    }

private:
    bool offline_ = false;
};

CTokenInfoForm::CTokenInfoForm(bool offline) : CBaseForm(L"")
{
    tokens_.reset(new CUIListView);
    tokens_->SetItemBKPainter(new CNullPainter);
    tokens_->SetSubItemCreator(new CBalanceItemListCreator(offline));
    tokens_->SetScrollBar(CommonUI::CreateVScrollBar());
    tokens_->UIModifyStyle(UILVStyle_FullRowSelect | UILVStyle_SimpleList
        | UILVStyle_TrackingMouse | UILVStyle_SingleSelection, 0);
}

void CTokenInfoForm::OnUICreate()
{
    tokens_->UICreate(this);
    SetTitle(L"Tokens", 60);
}

void CTokenInfoForm::Layout()
{
    CBaseForm::Layout();
    auto top = GetTitleHeight();
    auto rc = UIGetClientRect();
    rc.top = top;
    tokens_->UISetPos(rc);
}

void CTokenInfoForm::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    CBaseForm::OnUIPaint(dc, dwStatus);
    auto top = GetTitleHeight();
    auto rc = UIGetClientRect();
    CRect middle(0, top, rc.Width(), top + 15);
    dc.FillSolidRect(middle, RGB(242, 242, 242));

    rc.top = middle.bottom - 10;
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    CommonUI::Paint::FillRoundRect(&g, Gdiplus::Rect(rc.left, rc.top,
        rc.Width(), rc.Height()), Gdiplus::Color(238, 238, 238), CommonUI::kDefaultMargin);
}

void CTokenInfoForm::UpdateBalance(std::shared_ptr<protocol::Account> account)
{
    tokens_->Clear();
    int index = 0;
    tokens_->InsertItem(index);
    tokens_->SetItemHeight(index, 40);

    auto InsertItem = [&index, this](const std::string& name, uint64_t balance, int64_t freeze = 0) {
        index++;
        BalanceItemData* data = new BalanceItemData;
        data->name = base::UTF8ToWide(name);
        data->balance = base::UTF8ToWide(base::Int64ToString(balance));
        data->freeze_amounts = std::to_wstring(freeze);
        tokens_->InsertItem(index, false, nullptr, (DWORD_PTR)data);
        tokens_->SetItemHeight(index, 40);
    };

    int64_t freeze = CalcTronPower(account);
    InsertItem("TRX", account->balance() / 1000000, freeze / 1000000);
    for (const auto& p : account->asset())
        InsertItem(p.first, p.second);
}

CBalanceForm::CBalanceForm(bool offline) : CBaseForm(L""), offline_(offline), account_info_form_(offline), token_info_form_(offline)
{
}

void CBalanceForm::UpdateBalance(std::shared_ptr<protocol::Account> account)
{
    account_info_form_.address_->UISetText(base::UTF8ToWide(AddressEncodeBase58Check(address_)));
    account_info_form_.SetTitle(offline_ ? L"" : base::UTF8ToWide(AccountManager::_().GetAccountName()), 60);
    token_info_form_.UpdateBalance(account);
}

void CBalanceForm::OnUICreate()
{
    UISetSize(1000, 500);
    account_info_form_.UICreate(this);
    token_info_form_.UICreate(this);
}

void CBalanceForm::Layout()
{
    auto rc = UIGetClientRect();
    account_info_form_.UISetPos(0, 0, rc.Width(), 210);
    token_info_form_.UISetPos(0, 240, rc.Width(), 240);
}

void CBalanceForm::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{

}

void CBalanceForm::OnShowUI(bool show)
{
    account_info_form_.OnShowUI(show);
}

void CBalanceForm::SetAddress(const std::string& address)
{
    if (!address.empty())
    {
        address_ = address;
        auto account = std::make_shared<protocol::Account>();
        account->set_address(address);
        CNetworkThread::_().GetAccount(account, [account, this](std::shared_ptr<::protocol::Account> a) {
            CNetworkThread::_().GetAccountNet(account, [this](std::shared_ptr<::protocol::AccountNetMessage> net_msg) {
                account_info_form_.UpdateBandwidth(net_msg);
            });

            UpdateBalance(a);
            auto panel = (CShowPanel*)UIGetParent();
            panel->SetContent(this);
            AutoHideShowStatus(L"");
        });
        ShowStatus(L"Loading Balance List");
    }
}
