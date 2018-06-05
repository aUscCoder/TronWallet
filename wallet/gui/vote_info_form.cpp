#include "vote_info_form.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "core/Contract.pb.h"
#include "core/Tron.pb.h"
#include "common_ui/CommonDef.h"
#include "wallet_evnets.h"
#include "core/network_thread.h"
#include "core/account_manager.h"
#include "core/core_util.h"

#define VOTEINOF_FORM_WIDTH 480
#define VOTEINOF_FORM_HEIGHT 400

using namespace CommonUI;

enum class ControlID
{
    BTN_SAVE = 1100,
    BTN_CANCEL,
};

using namespace wallet_core;
using namespace wallet_gui;

struct VoteInfoItemData
{
    std::wstring url;
    std::wstring votes;
};

class CInfoItemListCreator : public CUIListViewSubItemCreator
{
public:
    CUIObject* OnCreateObject(CUIListView* pView, HLISTVIEWITEM hItem, const int iCol) override
    {
        auto CreateListItem = [](const std::wstring& url, const std::wstring& votes)
        {
            auto item = new CCommonListViewItem();
            auto CreateStatic = [&item](const std::wstring& text, int width)
            {
                auto ui = std::make_shared<CUIStatic>(text);
                ui->UISetSize(width, 0);
                item->Push(ui);
            };

            CreateStatic(url, 380);
            CreateStatic(votes, 80);
            return item;
        };

        CUIObject* item = nullptr;
        int row = pView->GetItemIndex(hItem);
        if (row == 0)
        {
            item = CreateListItem(L"Name", L"Votes");
        }
        else
        {
            auto data = (VoteInfoItemData*)pView->GetItemData(row);
            if (data)
            {
                item = CreateListItem(data->url, data->votes);
                delete data;
            }
        }

        item->UISetPainter(new CListItemBKPainter(row));
        return item;
    }
};

CVoteInfoForm::CVoteInfoForm(const VoteData& vote_data, TransactionStatus s)
    : CBaseForm(L"Vote For"), s_(s), vote_data_(vote_data)
{
    bool signature = vote_data_.transaction->signature_size() > 0;
    std::wstring status = base::StringPrintf(L"Status: %s", signature ? L"signed" : L"unsigned");

    status_.reset(new CUIStatic(status));
    save_.reset(CommonUI::CreateNormalButton(GetBtnText(s_).c_str(), (DWORD)ControlID::BTN_SAVE));
    cancel_.reset(CommonUI::CreateNormalButton(L"Cancel", (DWORD)ControlID::BTN_CANCEL));

    infos_.reset(new CUIListView);
    infos_->SetItemBKPainter(new CNullPainter);
    infos_->SetSubItemCreator(new CInfoItemListCreator);
    infos_->SetScrollBar(CommonUI::CreateVScrollBar());
    infos_->UIModifyStyle(UILVStyle_FullRowSelect | UILVStyle_SimpleList
        | UILVStyle_TrackingMouse | UILVStyle_SingleSelection, 0);
}

void CVoteInfoForm::OnUICreate()
{
    UISetSize(VOTEINOF_FORM_WIDTH, VOTEINOF_FORM_HEIGHT);
    CBaseForm::OnUICreate();
    status_->UICreate(this);
    save_->UICreate(this);
    cancel_->UICreate(this);
    infos_->UICreate(this);
    infos_->InsertItem(0);
    infos_->SetItemHeight(0, 40);

    auto& raw = vote_data_.transaction->raw_data();
    if (raw.contract_size() > 0)
    {
        auto contract = raw.contract(0);
        if (contract.type() == protocol::Transaction_Contract_ContractType_VoteWitnessContract)
        {
            protocol::VoteWitnessContract vote_contract;
            if (contract.parameter().UnpackTo(&vote_contract))
            {
                owner_address_ = AddressEncodeBase58Check(vote_contract.owner_address());
                for (int i = 0; i < vote_contract.votes_size(); i++)
                {
                    const auto& v = vote_contract.votes(i);
                    VoteInfoItemData* data = new VoteInfoItemData;
                    data->votes = std::to_wstring(v.vote_count());
                    data->url = base::UTF8ToWide(vote_data_.address_url[AddressEncodeBase58Check(v.vote_address())]);
                    infos_->InsertItem(i + 1, false, nullptr, (DWORD_PTR)data);
                    infos_->SetItemHeight(i + 1, 40);
                }
            }
        }
    }
}

void CVoteInfoForm::OnButtonClick(DWORD dwControlID)
{
    if (dwControlID == DWORD(ControlID::BTN_SAVE))
    {
        switch (s_)
        {
        case wallet_core::TransactionStatus::SAVE:
        {
            auto OnOk = [this](const std::wstring& path)
            {
                auto success = SaveVote(base::WideToUTF8(path), vote_data_);
                AutoHideShowStatus(success ? L"Save success" : L"Save failed");
                UIShow(false);
            };
            OpenSaveDlg(UIGetHWND(), L".json", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, L"*.json", OnOk, nullptr);
            break;
        }
        case wallet_core::TransactionStatus::SIGN:
        {
            if (!owner_address_.empty())
            {
                auto key = AccountManager::_().LookupPrivate(owner_address_);
                auto ret = SignTransaction(key, vote_data_.transaction);
                if (ret)
                {
                    auto OnOk = [this](const std::wstring& path)
                    {
                        auto success = SaveVote(base::WideToUTF8(path), vote_data_);
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
        case wallet_core::TransactionStatus::BROADCAST:
        {
            CNetworkThread::_().BroadcastTransaction(vote_data_.transaction, [this](std::shared_ptr<protocol::Return> ret) {
                auto msg = ret->result() ? L"Broadcast success" : base::UTF8ToWide(ret->message());
                if (msg.empty())
                    msg = L"Broadcast failed";
                AutoHideShowStatus(msg);
                UIShow(false);
            });
            break;
        }
        }

    }
    else if (dwControlID == DWORD(ControlID::BTN_CANCEL))
    {
        UIShow(false);
    }
}

void CVoteInfoForm::Layout()
{
    CBaseForm::Layout();
    auto rc_title = title_->UIGetRect();
    auto rc = UIGetClientRect();
    int top = rc_title.bottom + CommonUI::kDefaultMargin;
    status_->UISetPos(CommonUI::kDefaultMargin, top, rc.Width() - 2 * CommonUI::kDefaultMargin, 20);
    top += 20 + CommonUI::kDefaultMargin;
    infos_->UISetPos(0, top, rc.Width(), 220);
    top += 220 + CommonUI::kDefaultMargin;
    auto margin = (rc.Width() - 240) / 3;
    save_->UISetPos(margin, top, 120, 30);
    cancel_->UISetPos(2 * margin + 120, top, 120, 30);
}

