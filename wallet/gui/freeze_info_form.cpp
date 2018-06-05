#include "freeze_info_form.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"
#include "core/Contract.pb.h"
#include "UILite/Controls/UIEdit.h"
#include "common_ui/CommonDef.h"
#include "core/core_util.h"

enum class ControlID
{
    EDIT_FREEZE_ADDRESS = 3000,
    BTN_OFFLINE_FREEZE,
    BTN_OFFLINE_CLOSE,
};

using namespace wallet_gui;
using namespace CommonUI;

CFreezeInfoForm::CFreezeInfoForm(std::shared_ptr<protocol::Transaction> transaction, TransactionStatus s)
    : CTransactionInfoForm(transaction, s, L"Freeze")
{
}

void CFreezeInfoForm::OnUICreate()
{
    std::string owner_address;
    int64_t freeze_balance = 0, freeze_duration = 0;
    bool signature = false;
    std::tie(owner_address, freeze_balance, freeze_duration, signature) = ParseTransaction();
    std::wstring status = base::StringPrintf(L"Status: %s", signature ? L"signed" : L"unsigned");
    std::wstring str_amount = base::StringPrintf(L"Amount freeze: %lld TRX", freeze_balance);
    UISetSize(400, 300);
    item_infos_ =
    {
        { { { ControlType::LABEL, L""} }, {20, 10}, {0, 5} },
        { { { ControlType::LABEL, status } }, { 20, 12 }, { 0, 18 } },
        { { { ControlType::LABEL, str_amount } } ,{ 20, 12 },{ 0, 18 } },
        { { { ControlType::LABEL, L"Address" } }, { 20, 5 },{ 0, 15 } },
        { { { ControlType::EDIT, base::UTF8ToWide(AddressEncodeBase58Check(owner_address)), (DWORD)ControlID::EDIT_FREEZE_ADDRESS } }, { 20, 30} },
        { { { ControlType::NORMAL_BUTTON, GetBtnText(s_), (DWORD)ControlID::BTN_OFFLINE_FREEZE },{ ControlType::CANCEL_BUUTON, L"Close", (DWORD)ControlID::BTN_OFFLINE_CLOSE } }, { 5, 20 } },

    };
    CBaseForm::OnUICreate();
    ((CUIEdit*)GetSub((DWORD)ControlID::EDIT_FREEZE_ADDRESS))->UISetReadOnly();
}

void CFreezeInfoForm::OnButtonClick(DWORD dwControlID)
{
    if (dwControlID == (DWORD)ControlID::BTN_OFFLINE_FREEZE)
    {
        auto address = base::WideToUTF8(GetSub((DWORD)ControlID::EDIT_FREEZE_ADDRESS)->UIGetText());
        HandleClickEvent(address);
    }
    else if (dwControlID == (DWORD)ControlID::BTN_OFFLINE_CLOSE)
    {
        UIShow(false);
    }
}

FreezeInfo CFreezeInfoForm::ParseTransaction()
{
    FreezeInfo info;
    if (transaction_)
    {
        bool signature = transaction_->signature_size() > 0;
        auto& raw = transaction_->raw_data();
        if (raw.contract_size() > 0)
        {
            auto contract = raw.contract(0);
            if (contract.type() == protocol::Transaction_Contract_ContractType_FreezeBalanceContract)
            {
                protocol::FreezeBalanceContract freeze;
                if (contract.parameter().UnpackTo(&freeze))
                {
                    info = std::make_tuple(freeze.owner_address(),
                        freeze.frozen_balance(), freeze.frozen_duration(), signature);
                }
            }
        }
    }
    return info;
}

