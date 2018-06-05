#include "transaction_info_form.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"
#include "core/Tron.pb.h"
#include "core/Contract.pb.h"
#include "common_ui/UI.h"
#include "UILite/Controls/UIEdit.h"
#include "core/core_util.h"
#include "core/transaction_manager.h"
#include "core/account_manager.h"
#include "core/network_thread.h"
#include "wallet_evnets.h"

enum class ControlID
{
    BTN_SAVE = 1000,
    BTN_CLOSE,

    EDIT_INPUTS,
    EDIT_OUTPUTS,
};

#define TRANSACTION_INFO_WIDTH 400
#define TRANSACTION_INFO_HEIGHT 335

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

CTransactionInfoForm::CTransactionInfoForm(std::shared_ptr<protocol::Transaction> transaction,
    TransactionStatus s, const std::wstring& title)
    : CBaseForm(title), s_(s), transaction_(transaction)
{
}

void CTransactionInfoForm::OnUICreate()
{
    std::string from;
    std::string to;
    std::string name;
    double amount = 0;
    bool signature = false;
    std::tie(from, to, name, amount, signature) = ParseTransaction();
    std::wstring status = base::StringPrintf(L"Status: %s", signature ? L"signed" : L"unsigned");
    std::wstring str_amount = base::StringPrintf(L"Amount send: %.4f %s", amount, base::UTF8ToWide(name).c_str());
    UISetSize(TRANSACTION_INFO_WIDTH, TRANSACTION_INFO_HEIGHT);

    item_infos_ =
    {
        { { { ControlType::LABEL, L"" } }, {0, 5},{ 0, 10 } },
        { { { ControlType::LABEL, status } } ,{ 0, 10 }, { 0, 16 } },
        { { { ControlType::LABEL, str_amount } },{ 0, 10 },{ 0, 16 } },
        { { { ControlType::LABEL, L"Inputs"} }, {10, 5}, {0, 15} },
        { { { ControlType::EDIT, base::UTF8ToWide(AddressEncodeBase58Check(to)), (DWORD)ControlID::EDIT_INPUTS } } },
        { { { ControlType::LABEL, L"Outputs" } },{ 10, 5 },{ 0, 15 } },
        { { { ControlType::EDIT, base::UTF8ToWide(AddressEncodeBase58Check(from)), (DWORD)ControlID::EDIT_OUTPUTS } }, {10, 28} },
        { { { ControlType::NORMAL_BUTTON, GetBtnText(s_), (DWORD)ControlID::BTN_SAVE, L"", {0, 34} },{ ControlType::CANCEL_BUUTON, L"Close", (DWORD)ControlID::BTN_CLOSE, L"", { 0, 34 } } },{ 10, 0 }, { 0, 50} },
    };
    CBaseForm::OnUICreate();
    auto inputs = (CUIEdit*)GetSub((DWORD)ControlID::EDIT_INPUTS);
    auto outputs = (CUIEdit*)GetSub((DWORD)ControlID::EDIT_OUTPUTS);
    inputs->UISetReadOnly();
    outputs->UISetReadOnly();
}

void CTransactionInfoForm::HandleClickEvent(const std::string& address)
{
    switch (s_)
    {
    case TransactionStatus::SAVE:
    {
        auto OnOk = [this](const std::wstring& path)
        {
            auto success = SaveTransaction(base::WideToUTF8(path), transaction_);
            AutoHideShowStatus(success ? L"Save success" : L"Save failed");
            UIShow(false);
        };
        OpenSaveDlg(UIGetHWND(), L".json", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"*.json", OnOk, nullptr);
        break;
    }
    case TransactionStatus::SIGN:
    {
        if (!address.empty())
        {
            auto key = AccountManager::_().LookupPrivate(address);
            if (!key)
            {
                AutoHideShowStatus(base::StringPrintf(L"Address %s not have private key!", base::UTF8ToWide(address).c_str()));
                UIShow(false);
                return;
            }

            auto ret = SignTransaction(key, transaction_);
            if (ret)
            {
                auto OnOk = [this](const std::wstring& path)
                {
                    auto success = SaveTransaction(base::WideToUTF8(path), transaction_);
                    AutoHideShowStatus(success ? L"Save success" : L"Save failed");
                    UIShow(false);
                };
                OpenSaveDlg(UIGetHWND(), L".json", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"*.json", OnOk, nullptr);
            }
            else
            {
                AutoHideShowStatus(L"Sign failed");
                UIShow(false);
            }
        }
        break;
    }
    case TransactionStatus::BROADCAST:
    {
        CNetworkThread::_().BroadcastTransaction(transaction_, [this](std::shared_ptr<protocol::Return> ret) {
            auto msg = ret->result() ? L"Broadcast success" : base::UTF8ToWide(ret->message());
            AutoHideShowStatus(msg);
            UIShow(false);
        });
        ShowStatus(L"Broadcasting");
        break;
    }
    }
}

void CTransactionInfoForm::OnButtonClick(DWORD dwControlID)
{
    if (dwControlID == (DWORD)ControlID::BTN_SAVE)
    {
        auto address = base::WideToUTF8(GetSub((DWORD)ControlID::EDIT_OUTPUTS)->UIGetText());
        HandleClickEvent(address);
    }
    else if (dwControlID == (DWORD)ControlID::BTN_CLOSE)
    {
        UIShow(false);
    }
}

TransactionInfo CTransactionInfoForm::ParseTransaction()
{
    TransactionInfo result;
    if (transaction_)
    {
        bool signature = transaction_->signature_size() > 0;
        auto& raw = transaction_->raw_data();
        if (raw.contract_size() > 0)
        {
            auto contract = raw.contract(0);
            switch (contract.type())
            {
            case protocol::Transaction_Contract_ContractType_TransferContract:
            {
                protocol::TransferContract transfer;
                if (contract.parameter().UnpackTo(&transfer))
                {
                    result = std::make_tuple(transfer.owner_address(), transfer.to_address(), "TRX",
                        ToTrx(transfer.amount()), signature);
                }
                break;
            }
            case protocol::Transaction_Contract_ContractType_TransferAssetContract:
            {
                protocol::TransferAssetContract transfer_assert;
                if (contract.parameter().UnpackTo(&transfer_assert))
                {
                    result = std::make_tuple(transfer_assert.owner_address(), transfer_assert.to_address(),
                        transfer_assert.asset_name(), transfer_assert.amount(), signature);
                }
                break;
            }
            }
        }
    }
    return result;
}
