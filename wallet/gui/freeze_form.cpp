#include "freeze_form.h"
#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"
#include "UILite/Controls/UIEdit.h"
#include "core/network_thread.h"
#include "common_ui/UI.h"
#include "core/core_util.h"
#include "wallet_evnets.h"
#include "show_panel.h"

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

#define FREEZEFORM_WIDTH 400
#define FREEZEFORM_HEIGHT 220

enum class ControlID
{
    EDIT_FREEZE_AMOUNT = 900,
    EDIT_FREEZE_LABEL,
    BTN_FREEZE,
};

CFreezeForm::CFreezeForm(bool offline) : CBaseForm(L"Freeze TRX", true), offline_(offline)
{
}

void CFreezeForm::OnUICreate()
{
    UISetSize(FREEZEFORM_WIDTH, FREEZEFORM_HEIGHT);
    item_infos_ =
    {
        { { { ControlType::LABEL, L"TRX Amount", 0, L"", {0, 0}, CommonUI::white } }, { 16, 5 }, { 0, 24 } },
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_FREEZE_AMOUNT, L"", {0, 0 }, CommonUI::white } }, { 16, 20 } },
        { { { ControlType::NORMAL_BUTTON, L"Freeze", (DWORD)ControlID::BTN_FREEZE } },{ 120, 20 }, {0, 34} },
    };
    CBaseForm::OnUICreate();
    auto amount = ((CUIEdit*)GetSub((DWORD)ControlID::EDIT_FREEZE_AMOUNT));
    amount->UIAddOrRemoveStyle(true, UIEditStyle_Number);
    amount->UISetCaretColor(CommonUI::white);
    SetTitle(L"Freeze TRX", 80);
}

void CFreezeForm::OnShowUI(bool show)
{
    if (!show)
    {
        auto sub = (CUIEdit*)GetSub((DWORD)ControlID::EDIT_FREEZE_AMOUNT);
        sub->UISetText(L"");
    }
}

std::shared_ptr<protocol::FreezeBalanceContract> CFreezeForm::CreateContract()
{
    std::shared_ptr<protocol::FreezeBalanceContract> contract;
    do
    {
        auto sub = GetSub((DWORD)ControlID::EDIT_FREEZE_AMOUNT);
        auto amount_text = sub->UIGetText();
        if (amount_text.empty())
        {
            MessageBox(L"Amount can't empty.");
            break;
        }

        int64_t amount = 0;
        base::StringToInt64(base::WideToUTF8(amount_text), &amount);
        if (amount <= 0)
        {
            MessageBox(L"Frozen balance must be positive");
            break;
        }

        contract = std::make_shared<protocol::FreezeBalanceContract>();
        contract->set_owner_address(address_);
        contract->set_frozen_balance(amount * 1000000);
        contract->set_frozen_duration(3);

    } while (false);
    return contract;
}


void CFreezeForm::OnButtonClick(DWORD dwControlID)
{
    if (dwControlID == DWORD(ControlID::BTN_FREEZE))
    {
        auto contract = CreateContract();
        if (contract)
        {
            CNetworkThread::_().GetAccountWrapper(contract->owner_address(), [contract, this](std::shared_ptr<protocol::Account> account)
            {
                if (account->address().empty())
                {
                    MessageBox(base::StringPrintf(L"Account %s not exists", base::UTF8ToWide(AddressEncodeBase58Check(contract->owner_address())).c_str()));
                    return;
                }

                if (contract->frozen_balance() > account->balance())
                {
                    MessageBox(L"Frozen balance must be less than account balance.");
                    return;
                }

                CNetworkThread::_().FreezeBalance(contract, [this](std::shared_ptr<protocol::Transaction> transaction)
                {
                    if (!offline_)
                    {
                        if (SignTransaction(transaction))
                        {
                            CNetworkThread::_().BroadcastTransaction(transaction, [this](std::shared_ptr<protocol::Return> r) {
                                AutoHideShowStatus(r->result() ? L"Freeze Token Success" : base::UTF8ToWide(r->message()));
                                UIShow(false);
                                ::PostMessage(UIGetHWND(), Event_Show_BalanceList, 0, 0);
                            });
                            ShowStatus(L"Freezing");
                        }
                        else
                        {
                            OnTransactionCreateFailed(transaction);
                        }
                    }
                    else
                    {
                        if (transaction && transaction->raw_data().contract_size() > 0)
                        {
                            if (freeze_info_)
                                freeze_info_->UIDestroy();
                            freeze_info_.reset(new CFreezeInfoForm(transaction, TransactionStatus::SAVE));
                            auto panel = (CShowPanel*)(this->UIGetParent());
                            freeze_info_->UICreate(this->UIGetParent());
                            panel->SetContent(freeze_info_.get());
                        }
                        else
                        {
                            OnTransactionCreateFailed(transaction);
                        }
                    }
                });
            });
        }
    }
}
