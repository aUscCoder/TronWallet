#include "token_modify_form.h"
#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"
#include "common_ui/ui.h"
#include "api/api.pb.h"
#include "core/account_manager.h"
#include "core/network_thread.h"
#include "core/core_util.h"

const static long ONE_DAY_NET_LIMIT = 57600000000;

enum class ControlID
{
    EDIT_DESCRIPTION = 9000,
    EDIT_URL,
    EDIT_BANDWIDTH_LIMIT,
    EDIT_TOTAL_BANDWIDTH_LIMIT,
    BTN_SUBMIT,
};

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

CTokenModifyForm::CTokenModifyForm() : CBaseForm(L"Modify issued token")
{
}

void CTokenModifyForm::OnUICreate()
{
    UISetSize(600, 420);
    item_infos_ =
    {
        { { { ControlType::LABEL, L"" } },{ 0, 10 },{ 0, 10 } },
        { { { ControlType::LABEL, L"Description" } },{ 20, 5 },{ 0, 15 } },
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_DESCRIPTION} }, { 20, 20 },{ 0, normal_btn_height } },
        { { { ControlType::LABEL, L"Url" } },{ 20, 5 },{ 0, 15 } },
        { { { ControlType::EDIT, L"http://", (DWORD)ControlID::EDIT_URL} },{ 20, 20 },{ 0, normal_btn_height } },
        { { { ControlType::LABEL, L"Bandwidth limit" } },{ 20, 5 },{ 0, 15 } },
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_BANDWIDTH_LIMIT } },{ 20, 20 },{ 0, normal_btn_height } },
        { { { ControlType::LABEL, L"Total Bandwidth limit" } },{ 20, 5 },{ 0, 15 } },
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_TOTAL_BANDWIDTH_LIMIT } },{ 20, 20 },{ 0, normal_btn_height } },
        { { { ControlType::NORMAL_BUTTON, L"Submit", (DWORD)ControlID::BTN_SUBMIT } },{ 200, 20 },{ 0, normal_btn_height } },
    };
    CBaseForm::OnUICreate();
    GetSub((DWORD)ControlID::EDIT_BANDWIDTH_LIMIT)->UIAddOrRemoveStyle(true, UIEditStyle_Number);
    GetSub((DWORD)ControlID::EDIT_TOTAL_BANDWIDTH_LIMIT)->UIAddOrRemoveStyle(true, UIEditStyle_Number);
}

void CTokenModifyForm::OnButtonClick(DWORD dwControlID)
{
    if (dwControlID == (DWORD)ControlID::BTN_SUBMIT)
    {

        bool valid = false;
        std::string desc_, url_;
        int64_t new_limit = 0, new_public_linit = 0;
        do 
        {
            auto desc = GetSub((DWORD)ControlID::EDIT_DESCRIPTION)->UIGetText();
            if (desc.empty())
            {
                MessageBox(L"Please input description!");
                break;
            }

            desc_ = base::WideToUTF8(desc);

            auto url = GetSub((DWORD)ControlID::EDIT_URL)->UIGetText();
            if (url.empty())
            {
                MessageBox(L"Please input url!");
                break;
            }

            url_ = base::WideToUTF8(url);


            auto new_limit_text = GetSub((DWORD)ControlID::EDIT_BANDWIDTH_LIMIT)->UIGetText();
            if (new_limit_text.empty())
            {
                MessageBox(L"Please bandwidth limit!");
                break;
            }

            base::StringToInt64(base::WideToUTF8(new_limit_text), &new_limit);
            if (new_limit < 0 || new_limit >= ONE_DAY_NET_LIMIT)
            {
                MessageBox(L"Invalid bandwidth limit");
                break;
            }

            auto new_public_limit_text = GetSub((DWORD)ControlID::EDIT_TOTAL_BANDWIDTH_LIMIT)->UIGetText();
            if (new_public_limit_text.empty())
            {
                MessageBox(L"Please input total bandwidth limit!");
                break;
            }

            base::StringToInt64(base::WideToUTF8(new_public_limit_text), &new_public_linit);
            if (new_public_linit < 0 || new_public_linit >= ONE_DAY_NET_LIMIT)
            {
                MessageBox(L"Invalid total bandwidth limit");
                break;
            }
            valid = true;

        } while (false);


        if (true)
        {
            auto contract = std::make_shared<protocol::UpdateAssetContract>();
            contract->set_owner_address(AccountManager::_().Address());
            contract->set_url(url_);
            contract->set_description(desc_);
            contract->set_new_limit(new_limit);
            contract->set_new_public_limit(new_public_linit);

            CNetworkThread::_().UpdateAsset(contract, [this](std::shared_ptr<protocol::Transaction> transaction) {
                if (SignTransaction(transaction))
                {
                    CNetworkThread::_().BroadcastTransaction(transaction, [this](std::shared_ptr<protocol::Return> r) {
                        AutoHideShowStatus(r->result() ? L"Update assert success" : base::UTF8ToWide(r->message()));
                    });
                }
                else
                {
                    OnTransactionCreateFailed(transaction);
                }

            });
            ShowStatus(L"Updating Assert");
        }
      
    }
}
