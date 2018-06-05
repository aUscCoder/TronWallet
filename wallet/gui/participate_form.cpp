#include "participate_form.h"
#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "core/network_thread.h"
#include "core/account_manager.h"
#include "core/core_util.h"
#include "wallet_evnets.h"
#include "base/stringprintf.h"

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

enum class ControlID
{
    EDIT_TOKEN_AMOUNT = 800,
    EDIT_TOTAL,
    BTN_PARTICIPATE,
    LINK_WEBSITE,
};

CParticipateForm::CParticipateForm() : CBaseForm(L"")
{
    SetTitle(L"Participating Issued Token", 60);
    amount_assistant_.reset(new CUIStatic(L""));
    progress_.reset(CommonUI::CreateProgressBar());
}

void CParticipateForm::UpdateItemData(const IssueItemData& data)
{
    data_ = data;
    ControlType btn_type = data_.avaliable_count > 0 ? ControlType::NORMAL_BUTTON : ControlType::CANCEL_BUUTON;
    std::wstring btn_text = data_.avaliable_count > 0 ? L"Submit" : L"Finished";
    auto participated = std::to_wstring(data_.total_supply - data_.avaliable_count) + L" / " + std::to_wstring(data.total_supply);

    item_infos_ =
    {
        { { { ControlType::LABEL, L"Token Name" },{ ControlType::LABEL, data.name } } },
        { { { ControlType::LABEL, L"Issuer" },{ ControlType::LABEL, data.issuer } } },
        { { { ControlType::LABEL, L"Website" }, { ControlType::HYPERLINK, data.url, (DWORD)ControlID::LINK_WEBSITE } } },
        { { { ControlType::LABEL, L"Participated / Total supply" },{ ControlType::STATIC_EDIT, participated, (DWORD)ControlID::EDIT_TOTAL } } },
        { { { ControlType::LABEL, L"Price" }, { ControlType::LABEL, std::to_wstring(data.exchange_rate) + L" TRX" } } },
        { { { ControlType::LABEL, L"Start Time" },{ ControlType::LABEL, GetTime(data.start_time) } } },
        { { { ControlType::LABEL, L"End Time" }, { ControlType::LABEL, GetTime(data.end_time) } } },
        { { { ControlType::LABEL, L"Description" },{ ControlType::LABEL, data.description } } },
        { { { ControlType::LABEL, L"Amount" },{ ControlType::EDIT, L"", (DWORD)ControlID::EDIT_TOKEN_AMOUNT } } ,{ 0, 20 } },
        { { { btn_type, btn_text, (DWORD)ControlID::BTN_PARTICIPATE} }, { 180, 0}, {0, 34} },
    };

    for (auto& item : sub_items_)
    {
        item->UIDestroy();
    }
    sub_items_.clear();
    CreateSubItems();

    auto edit_amount = GetSub(DWORD(ControlID::EDIT_TOKEN_AMOUNT));
    auto amount = (CCommonListViewItem*)(edit_amount->UIGetParent());
    amount->SetEqually(false);
    edit_amount->UISetSize(100, 20);
    edit_amount->UIAddOrRemoveStyle(true, UIEditStyle_Number);
    amount_assistant_->UISetText(L"");

    auto total = GetSub(DWORD(ControlID::EDIT_TOTAL));
    auto total_parent = (CCommonListViewItem*)(total->UIGetParent());
    total_parent->SetEqually(false);

    auto size = total->UIMeasure();
    total->UISetSize(size.cx, 20);

    if (data_.avaliable_count <= 0)
        GetSub((DWORD)ControlID::BTN_PARTICIPATE)->UIDisable();

    ((CCommonListViewItem*)sub_items_[2].get())->SetClickCallback([URL = data.url](DWORD id){
        ShellExecute(NULL, _T("open"), URL.c_str(), NULL, NULL, SW_SHOW);
    });

    if (data_.total_supply > 0)
    {
        progress_->SetProgress((data_.total_supply - data_.avaliable_count) * 100 / data_.total_supply );
    }

    amount->Layout();
    total_parent->Layout();
    Layout();
}

void CParticipateForm::OnUICreate()
{
    UISetSize(620, 480);
    CBaseForm::OnUICreate();
    amount_assistant_->UICreate(this);
    progress_->UICreate(this);
}

void CParticipateForm::Layout()
{
    CBaseForm::Layout();
    if (!sub_items_.empty())
    {
        auto edit_amount = GetSub(DWORD(ControlID::EDIT_TOKEN_AMOUNT));
        auto amount = edit_amount->UIGetParent();
        if (edit_amount && amount)
        {
            auto r1 = edit_amount->UIGetRect();
            auto r2 = amount->UIGetRect();
            auto top = (r2.Height() - r1.Height()) / 2 + r2.top;
            amount_assistant_->UISetPos(r1.right + 10, top, 400, 20);
        }

        auto total = GetSub(DWORD(ControlID::EDIT_TOTAL));
        auto total_parent = total->UIGetParent();
        if (total && total_parent)
        {
            auto r1 = total->UIGetRect();
            auto r2 = total_parent->UIGetRect();
            auto top = (r2.Height() - r1.Height()) / 2 + r2.top;
            progress_->UISetPos(r1.right + 10, top, 120, 20);
        }
    }
}

void CParticipateForm::OnButtonClick(DWORD dwControlID)
{
    if ((DWORD)ControlID::BTN_PARTICIPATE == dwControlID)
    {
        auto& manager = AccountManager::_();
        if (manager.CheckLogin())
        {
            auto sub = GetSub((DWORD)ControlID::EDIT_TOKEN_AMOUNT);
            auto amount_text = sub->UIGetText();
            if (amount_text.empty())
            {
                MessageBox(L"Please input amount");
            }
            else
            {
                int64_t amount = 0;
                base::StringToInt64(base::WideToUTF8(amount_text), &amount);
                if (amount > data_.avaliable_count)
                {
                    MessageBox(L"Participate amount greater than rest amount.");
                    return;
                }

                auto trx_num = data_.exchange_rate * amount;
                amount = ToDrop(trx_num);
                std::string to;
                std::tie(std::ignore, to) = AddressDecodeBase58Check(base::WideToUTF8(data_.issuer));
                if (manager.Address() == to)
                {
                    MessageBox(L"Cannot participate asset issue yourself!");
                    return;
                }

                Participate(manager.Address(), to, base::WideToUTF8(data_.name), amount);
            }
        }
    }
}

void CParticipateForm::OnEditTextChange(DWORD dwControlID)
{
    if (dwControlID == (DWORD)ControlID::EDIT_TOKEN_AMOUNT)
    {
        auto content = base::WideToUTF8(GetSub(dwControlID)->UIGetText());
        if (!content.empty())
        {
            int64_t amount = 0;
            base::StringToInt64(content, &amount);
            if (amount > 0)
            {
                auto trx_num = data_.exchange_rate * amount;
                amount_assistant_->UISetText(base::StringPrintf(L"  =  %.4f TRX", trx_num));
            }
            else
            {
                MessageBox(L"Amount must greater than 0!");
            }
        }
    }
}

void CParticipateForm::Participate(const std::string& owner, const std::string& to, const std::string& assert_name, int64_t amount)
{
    auto contract = std::make_shared<protocol::ParticipateAssetIssueContract>();
    contract->set_amount(amount);
    contract->set_owner_address(owner);
    contract->set_asset_name(assert_name);
    contract->set_to_address(to);

    CNetworkThread::_().GetAccountWrapper(owner, [contract, this](std::shared_ptr<protocol::Account> account) {
        if (account->address().empty())
        {
            MessageBox(base::StringPrintf(L"Account %s not exists", base::UTF8ToWide(AddressEncodeBase58Check(contract->owner_address())).c_str()));
            return;
        }
        else
        {
            CNetworkThread::_().ParticipateAssetIssue(contract, [this](std::shared_ptr<protocol::Transaction> transaction) {
                if (SignTransaction(transaction))
                {
                    CNetworkThread::_().BroadcastTransaction(transaction, [this](std::shared_ptr<protocol::Return> r)
                    {
                        AutoHideShowStatus(r->result() ? L"Participate Success." : L"Participate Failed.");
                    });
                }
                else
                {
                    OnTransactionCreateFailed(transaction);
                }
            });
            ShowStatus(L"Participating");
        }
    });
}
