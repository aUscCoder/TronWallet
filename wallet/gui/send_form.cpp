#include "send_form.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"
#include "base/string_number_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "UILite/Controls/UILabel.h"
#include "core/Tron.pb.h"
#include "core/Contract.pb.h"
#include "core/network_thread.h"
#include "core/core_util.h"
#include "wallet_evnets.h"
#include "show_panel.h"

#define SENDFORM_ITEM_WIDTH 380
#define SENDFORM_WIDTH 480
#define SENDFORM_HEIGHT 400

using namespace wallet_gui;
using namespace wallet_core;

enum class ControlID
{
    EDIT_TO = 400,
    EDIT_AMOUNT,
    COMBOBOX_TOKEN,
    BTN_SEND,
    BTN_CANCEL,
};

CSendForm::CSendForm(bool offline) : CBaseForm(L"Send TRX", true), offline_(offline)
{
    label_to_.reset(new CUIStatic(L"TO"));
    to_.reset(CommonUI::CreateEdit((DWORD)ControlID::EDIT_TO));
    label_token_.reset(new CUIStatic(L"Token"));
    token_.reset(CommonUI::CreateComboBox((DWORD)ControlID::COMBOBOX_TOKEN));
    label_amount_.reset(new CUIStatic(L"Amount"));
    amount_.reset(CommonUI::CreateEdit((DWORD)ControlID::EDIT_AMOUNT));
    avaliable_amount_.reset(new CUIStatic(L"0 avaliable"));
    if (offline_)
    {
        btn_send_.reset(CommonUI::CreateNormalButton(L"Create unsigned transaction", (DWORD)ControlID::BTN_SEND));
        btn_cancel_.reset(CommonUI::CreateNormalButton(L"Close", (DWORD)ControlID::BTN_CANCEL));
    }
    else
    {
        btn_send_.reset(CommonUI::CreateNormalButton(L"Send", (DWORD)ControlID::BTN_SEND));
        btn_cancel_.reset(CommonUI::CreateNormalButton(L"Cancel", (DWORD)ControlID::BTN_CANCEL));
    }
}

void CSendForm::OnUICreate()
{
    UISetSize(SENDFORM_WIDTH, SENDFORM_HEIGHT);
    CBaseForm::OnUICreate();
    std::vector<CUIRichObject*> controls = {
        label_to_.get(), to_.get(), label_token_.get(), token_.get(), label_amount_.get(),
        amount_.get(), avaliable_amount_.get(), btn_send_.get(), btn_cancel_.get()
    };
    for (auto c : controls)
    {
        c->UICreate(this);
        c->UISetTextColor(CommonUI::white);
    }
    to_->UISetCaretColor(CommonUI::white);
    amount_->UISetCaretColor(CommonUI::white);
    token_->UISetTextColor(0x000000);
    avaliable_amount_->UISetTextColor(CommonUI::slected_text_color);
    Layout();
}

void CSendForm::UpdteAddress(const std::string& address, std::function<void(bool)> callback)
{
    address_ = address;
    CNetworkThread::_().GetAccountWrapper(address, [this, callback](std::shared_ptr<protocol::Account> a)
    {
        assert_["TRX"] = ToTrx(a->balance());
        for (const auto& p : a->asset())
            assert_[p.first] = p.second;

        token_->DeleteAllItems();
        for (auto& p : assert_)
        {
            token_->AddString(base::UTF8ToWide(p.first).c_str());
        }
        token_->SetCurSel(0);
        ShowAvaliable();

        if (callback)
            callback(true);
    });
}

void CSendForm::OnShow(bool bShow)
{
    if (bShow && !address_.empty())
    {
    }
    
    if (!bShow)
    {
        to_->UISetText(L"");
        amount_->UISetText(L"");
    }
}

double CSendForm::GetAvaliable(const std::string& type)
{
    return assert_[type];
}

void CSendForm::ShowAvaliable()
{
    int cur = token_->GetCurSel();
    if (cur >= 0)
    {
        auto text = token_->GetString(cur);
        auto type = base::WideToUTF8(text);
        auto avaliable = GetAvaliable(type);
        avaliable_amount_->UISetText(base::StringPrintf(L"%.4f %s avaliable", avaliable, text));
    }
}

void CSendForm::Layout()
{
    CBaseForm::Layout();
    CRect rcClient = UIGetClientRect();
    int top = GetTitleHeight() + CommonUI::kDefaultMargin;
    auto left = (rcClient.Width() - SENDFORM_ITEM_WIDTH) / 2;
    label_to_->UISetPos(left, top, SENDFORM_ITEM_WIDTH, 15);
    top += 20;
    to_->UISetPos(left, top, SENDFORM_ITEM_WIDTH, CommonUI::normal_btn_height);
    top += CommonUI::normal_btn_height + CommonUI::kDefaultMargin;
    label_token_->UISetPos(left, top, SENDFORM_ITEM_WIDTH, 15);
    top += 20;
    token_->UISetPos(left, top, SENDFORM_ITEM_WIDTH, CommonUI::normal_btn_height);
    top += CommonUI::normal_btn_height + CommonUI::kDefaultMargin;
    label_amount_->UISetPos(left, top, SENDFORM_ITEM_WIDTH, 15);
    top += 20;
    amount_->UISetPos(left, top, SENDFORM_ITEM_WIDTH, CommonUI::normal_btn_height);
    top += CommonUI::normal_btn_height + 5;
    avaliable_amount_->UISetPos(left, top, SENDFORM_ITEM_WIDTH, 15);
    top += 40;

    auto btn_send_size = btn_send_->UIMeasure();
    auto btn_send_width = std::max<int>(btn_send_size.cx, 120);

    btn_send_->UISetPos(left, top, btn_send_width, CommonUI::normal_btn_height);
    btn_cancel_->UISetPos(rcClient.Width() - left - 120, top, 120, CommonUI::normal_btn_height);
    UIRedraw();
}

void CSendForm::OnButtonClick(DWORD dwControlID)
{
    ControlID id = (ControlID)dwControlID;
    switch (id)
    {
    case ControlID::BTN_SEND:
    {
        bool valid = false;
        std::string from;
        std::string to;
        std::string token_type;
        double amount = 0;
        std::tie(valid, from, to, token_type, amount) = GetSendInfo();
        if (valid)
        {
            if (offline_)
                CreateUnsignedTransaction(from, to, token_type, ToDrop(amount));
            else
                SendToken(from, to, token_type, ToDrop(amount));
        }
        break;
    }
    case ControlID::BTN_CANCEL:
        UIShow(false);
        break;
    default:
        break;
    }
}

LRESULT CSendForm::OnUISelChanged(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    ShowAvaliable();
    return S_OK;
}

std::tuple<bool, std::string, std::string, std::string, double> CSendForm::GetSendInfo()
{
    std::wstring err;
    std::string from;
    std::string to;
    std::string token_type;
    double amount_num;
    do
    {
        from = address_;
        to = base::WideToUTF8(to_->UIGetText());
        if (to.empty())
        {
            err = L"To address can't empty!";
            break;
        }

        if (!CheckAddress(to))
        {
            err = L"Invalid to address";
            break;
        }

        if (from == to)
        {
            err = L"Cannot transfer to yourself";
            break;
        }

        auto amount = base::WideToUTF8(amount_->UIGetText());
        if (amount.empty())
        {
            err = L"Amount can't empty!";
            break;
        }

        if (!base::StringToDouble(amount, &amount_num))
        {
            err = L"Invalid amount";
            break;
        }

        if (amount_num <= 0)
        {
            err = L"Amount must greater than 0";
            break;
        }

        int cur = token_->GetCurSel();
        if (cur >= 0)
        {
            token_type = base::WideToUTF8(token_->GetString(cur));
            auto avaliable = GetAvaliable(token_type);
            if (amount_num > avaliable)
            {
                err = L"Balance is not sufficient";
                break;
            }
        }

    } while (false);

    if (!err.empty())
    {
        ::MessageBox(UIGetHWND(), err.c_str(), L"", MB_OK);
    }

    return std::make_tuple(err.empty(), from, to, token_type, amount_num);
}

void CSendForm::CreateTransaction(const std::string& from, const std::string& to, const std::string& type,
    int64_t amount, const std::function<void(std::shared_ptr<protocol::Transaction>)>& callback)
{
    if (type == "TRX")
    {
        auto transfer = std::make_shared<protocol::TransferContract>();
        transfer->set_amount(amount);
        transfer->set_to_address(to);
        transfer->set_owner_address(from);
        CNetworkThread::_().CreateTransaction(transfer, [callback](std::shared_ptr<protocol::Transaction> transaction)
        {
            callback(transaction);
        });
    }
    else
    {
        auto transfer = std::make_shared<protocol::TransferAssetContract>();
        transfer->set_to_address(to);
        transfer->set_owner_address(from);
        transfer->set_amount(amount);
        transfer->set_asset_name(type);
        CNetworkThread::_().TransferAsset(transfer, [callback](std::shared_ptr<protocol::Transaction> transaction)
        {
            callback(transaction);
        });
    }
}

void CSendForm::CreateUnsignedTransaction(const std::string& from, const std::string& to, const std::string& type, int64_t amount)
{
    auto SaveTransaction = [this](std::shared_ptr<protocol::Transaction> transaction)
    {
        if (transaction && transaction->raw_data().contract_size() > 0)
        {
            if (transaction_info_)
                transaction_info_->UIDestroy();
            transaction_info_.reset(new CTransactionInfoForm(transaction, TransactionStatus::SAVE));
            auto panel = (CShowPanel*)(this->UIGetParent());
            transaction_info_->UICreate(this->UIGetParent());
            panel->SetContent(transaction_info_.get());
        }
        else
        {
            OnTransactionCreateFailed(transaction);
        }
    };
    CreateTransaction(from, to, type, amount, SaveTransaction);
}

void CSendForm::SendToken(const std::string& from, const std::string& to, const std::string& type, int64_t amount)
{
    auto BroadcastTransaction = [this](std::shared_ptr<protocol::Transaction> transaction)
    {
        if (SignTransaction(transaction))
        {
            CNetworkThread::_().BroadcastTransaction(transaction, [this](std::shared_ptr<protocol::Return> ret) {
                AutoHideShowStatus(ret->result() ? L"Send Success" : base::UTF8ToWide(ret->message()));
            });
        }
        else
        {
            OnTransactionCreateFailed(transaction);
        }
    };

    CreateTransaction(from, to, type, amount, BroadcastTransaction);
    ShowStatus(L"Sending");
}


