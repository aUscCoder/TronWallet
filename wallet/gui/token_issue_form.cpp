#include "token_issue_form.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"
#include "base/string_number_conversions.h"
#include "common_ui/ui.h"
#include "common_ui/CommonDef.h"
#include "core/Contract.pb.h"
#include "core/core_util.h"
#include "core/network_thread.h"
#include "core/account_manager.h"
#include "wallet_evnets.h"
#include "common_ui/PlaceHolderEdit.h"

using namespace wallet_gui;
using namespace CommonUI;
using namespace wallet_core;

enum class ControlID
{
    EDIT_TOKEN_NAME = 600,
    EDIT_TOTLE_SUPPLY,
    EDIT_DESCRIPTION,
    EDIT_URL,
    EDIT_TRX_AMOUNT,
    EDIT_TOKEN_AMOUNT,
    EDIT_PRICE,
    EDIT_START_DATE,
    EDIT_END_DATE,
    EDIT_TIP,
    BTN_ISSUE,
};

CTokenIssueForm::CTokenIssueForm() : CBaseForm(L"")
{
    SetTitle(L"Issue a Token", 48);
}

void CTokenIssueForm::OnUICreate()
{
    UISetSize(600, 720);
    static const std::wstring exchange_rate_des = L"Specify the price of a single token by defining how many tokens a participant will \nreceive for every TRX they spend. \n \nParticipants will receive X Token for every X TRX";
    static const std::wstring paticipate_des = L"Specify the participation period in which tokens will be issues. During the \nparticipation period users can exchange TRX for XXX tokens.";

    item_infos_ =
    {
        { { { ControlType::LABEL, L"" } },{ 0, 0 }, { 0, 5} },
        { { { ControlType::TITLE_LEBEL, L"Details" } },{ 0, 8 }, {0, 15} },
        { { { ControlType::LABEL, L"Issue a Token" },{ ControlType::LABEL, L"Total supply" } },{ 1, 8 },{ 0, 15 } },
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_TOKEN_NAME },{ ControlType::EDIT, L"100000", (DWORD)ControlID::EDIT_TOTLE_SUPPLY} }, { 1, 18 } },
        { { { ControlType::LABEL, L"Description" } }, { 0, 8 },{ 0, 15 } },
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_DESCRIPTION } },{ 0, 18 } },
        { { { ControlType::LABEL, L"Url" } },{ 0, 8 },{ 0, 15 } },
        { { { ControlType::EDIT, L"http://", (DWORD)ControlID::EDIT_URL} },{ 0, 18 } },
        { { { ControlType::TITLE_LEBEL, L"Exchange Rate" } },{ 0, 8 },{ 0, 15 } },

        { { { ControlType::LABEL, exchange_rate_des} }, {8, 5}, {0, 74} },
        { { { ControlType::LABEL, L"TRX Amount" }, { ControlType::LABEL, L"Token Amount" } },{ 1, 8 },{ 0, 15 } },
        { { { ControlType::EDIT, L"1", (DWORD)ControlID::EDIT_TRX_AMOUNT } ,{ ControlType::EDIT, L"1", (DWORD)ControlID::EDIT_TOKEN_AMOUNT} }, { 1, 10 } },

        { { { ControlType::STATIC_EDIT, L"Price 1 Token = 1 TRX", (DWORD)ControlID::EDIT_PRICE } } , { 0, 10 } },

        { { { ControlType::TITLE_LEBEL, L"Participation" } },{ 0, 8 },{ 0, 15 } },
        { { { ControlType::LABEL, paticipate_des } },{ 0, 5 },{ 0, 56 } },

        { { { ControlType::LABEL, L"Start Date" },{ ControlType::LABEL, L"End Date" } },{ 1, 8 },{ 0, 15 } },
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_START_DATE, L" M/D/Y H:M" },{ ControlType::EDIT, L"", (DWORD)ControlID::EDIT_END_DATE, L" M/D/Y H:M" } },{ 1, 18 } },
        
        { { { ControlType::NORMAL_BUTTON, L"1024 TRX is required to issue a new token", (DWORD)ControlID::BTN_ISSUE } }, {120, 0}, {0, CommonUI::normal_btn_height} },
    };
    CBaseForm::OnUICreate();
    GetSub((DWORD)ControlID::EDIT_TOTLE_SUPPLY)->UIAddOrRemoveStyle(true, UIEditStyle_Number);
    GetSub((DWORD)ControlID::EDIT_TOKEN_AMOUNT)->UIAddOrRemoveStyle(true, UIEditStyle_Number);
    GetSub((DWORD)ControlID::EDIT_TRX_AMOUNT)->UIAddOrRemoveStyle(true, UIEditStyle_Number);
}

void CTokenIssueForm::OnShowUI(bool show)
{
    if (show)
    {
        auto stime_control = (CUIEdit*)GetSub((DWORD)ControlID::EDIT_START_DATE);
        auto etime_control = (CUIEdit*)GetSub((DWORD)ControlID::EDIT_END_DATE);
        if (stime_control && etime_control)
        {
            auto current = currentTimeMillis();
            uint64_t start_time = current + 86400000; // add 1 day
            uint64_t end_time = current + 7776000000; // add 90 days

            stime_control->UISetText(GetTime(start_time, true));
            etime_control->UISetText(GetTime(end_time, true));
        }
    }
}

void CTokenIssueForm::CheckAndIssue()
{
    bool success = false;
    std::string name;
    std::string supply; int64_t supply_num = 0;
    std::string trx; int64_t trx_num = 0;
    std::string token; int64_t token_num = 0;
    std::string description;
    std::string url;
    uint64_t stime = 0, etime = 0;

    do {

        auto Check = [this](std::string& content, ControlID id, const std::wstring& error_msg)
        {
            auto text = base::WideToUTF8(GetSub(DWORD(id))->UIGetText());
            if (text.empty() && !error_msg.empty())
                MessageBox(error_msg);
            content = std::move(text);
            return !content.empty() || error_msg.empty();
        };

        auto CheckNum = [this](int64_t& num, const std::string& num_text, const std::wstring& err)
        {
            bool valide = true;
            base::StringToInt64(num_text, &num);
            if (num <= 0)
            {
                MessageBox(err);
                valide = false;
            }
            return valide;
        };

        auto CheckDataTime = [this](uint64_t& time, ControlID id)
        {
            bool valid = true;
            auto datetime_text = base::WideToUTF8(GetSub(DWORD(id))->UIGetText());
            time = GetTime(datetime_text, true);
            if (time == 0)
            {
                MessageBox(L"Date time format error!");
                valid = false;
            }
            return valid;
        };

        if (!Check(name, ControlID::EDIT_TOKEN_NAME, L"Please input token name!"))
            break;

        if (!Check(supply, ControlID::EDIT_TOTLE_SUPPLY, L"Please input total supply!") || !CheckNum(supply_num, supply, L"Total supply must greater than 0!"))
            break;

        if (!Check(trx, ControlID::EDIT_TRX_AMOUNT, L"Please input trx amount!") || !CheckNum(trx_num, trx, L"TRX amount must greater than 0!"))
            break;

        if (!Check(token, ControlID::EDIT_TOKEN_AMOUNT, L"Please input token amount!") || !CheckNum(token_num, token, L"Token amount must greater than 0!"))
            break;

        if (!Check(description, ControlID::EDIT_DESCRIPTION, L"Please input description!"))
            break;

        if (!Check(url, ControlID::EDIT_URL, L"Please input url!"))
            break;

        if (!CheckDataTime(stime, ControlID::EDIT_START_DATE) || !CheckDataTime(etime, ControlID::EDIT_END_DATE))
            break;

        if (stime < currentTimeMillis())
        {
            MessageBox(L"Start time should be greater than now");
            break;
        }

        if (etime < stime)
        {
            MessageBox(L"End time should be greater than start time");
            break;
        }

        success = true;

    } while (false);


    if (success)
    {
        auto address = AccountManager::_().Address();
        auto contract = std::make_shared<protocol::AssetIssueContract>();
        contract->set_owner_address(address);
        contract->set_name(name);
        contract->set_total_supply(supply_num);
        contract->set_trx_num(ToDrop(trx_num));
        contract->set_num(token_num);
        contract->set_start_time(stime);
        contract->set_end_time(etime);
        contract->set_description(description);
        contract->set_url(url);

        CNetworkThread::_().GetAccountWrapper(contract->owner_address(), [contract, this](std::shared_ptr<protocol::Account> account) {

            if (account->address().empty())
            {
                MessageBox(base::StringPrintf(L"Account %s not exists", base::UTF8ToWide(AddressEncodeBase58Check(contract->owner_address())).c_str()));
                return;
            }
            else if (ToTrx(account->balance()) < 1024)
            {
                MessageBox(base::StringPrintf(L"Balance must be greater than 1024", base::UTF8ToWide(AddressEncodeBase58Check(contract->owner_address())).c_str()));
                return;
            }
            else
            {
                // TODO(jiangru, check token if exists)
                CNetworkThread::_().CreateAssetIssue(contract, [this](std::shared_ptr<protocol::Transaction> transaction) {
                    if (SignTransaction(transaction))
                    {
                        CNetworkThread::_().BroadcastTransaction(transaction, [this](std::shared_ptr<protocol::Return> r)
                        {
                            AutoHideShowStatus(r->result() ? L"Issue Success." : L"Issue Failed.");
                        });
                    }
                    else
                    {
                        OnTransactionCreateFailed(transaction);
                    }
                });
                ShowStatus(L"Issuing a token");
            }
        });
    }
}

void CTokenIssueForm::OnEditTextChange(DWORD dwControlID)
{
    auto id = (ControlID)dwControlID;
    if (id == ControlID::EDIT_TRX_AMOUNT || id == ControlID::EDIT_TOKEN_AMOUNT || id == ControlID::EDIT_TOKEN_NAME)
    {
        auto trx = base::WideToUTF8(GetSub((DWORD)ControlID::EDIT_TRX_AMOUNT)->UIGetText());
        auto token = base::WideToUTF8(GetSub((DWORD)ControlID::EDIT_TOKEN_AMOUNT)->UIGetText());
        auto token_name = GetSub((DWORD)ControlID::EDIT_TOKEN_NAME)->UIGetText();

        if (token_name.empty())
            token_name = L"Token";

        int64_t trx_amount = 0, token_amount = 0;
        base::StringToInt64(trx, &trx_amount);
        base::StringToInt64(token, &token_amount);

        std::wstring price = base::StringPrintf(L"1 %s = %.4f TRX", token_name.c_str(), ((double)trx_amount) / token_amount);
        ((CUIEdit*)(GetSub((DWORD)ControlID::EDIT_PRICE)))->UISetText(price);
    }
}

void CTokenIssueForm::OnButtonClick(DWORD dwControlID)
{
    if (dwControlID == (DWORD)ControlID::BTN_ISSUE)
        CheckAndIssue();
}

