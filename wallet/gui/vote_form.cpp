#include "vote_form.h"
#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "api/api.pb.h"
#include "core/core_util.h"
#include "core/account_manager.h"
#include "core/network_thread.h"
#include "wallet_evnets.h"
#include "show_panel.h"

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

struct WitnessItemData
{
    std::wstring address;
    std::wstring website;
    std::wstring votes;
    std::wstring myvotes;
};

class CWitnessItemListCreator : public CUIListViewSubItemCreator
{
public:
    CUIObject* OnCreateObject(CUIListView* pView, HLISTVIEWITEM hItem, const int iCol) override
    {
        auto CreateListItem = [](const std::wstring& index, const std::wstring& address, const std::wstring& website,
            const std::wstring& votes, const std::wstring& myvotes, bool head)
        {
            auto item = new CCommonListViewItem();
            auto CreateStatic = [&item](const std::wstring& text, int width)
            {
                std::shared_ptr<CUIEdit> ui(CommonUI::CreateStaticEdit(0, text));
                ui->UISetSize(width, 0);
                item->Push(ui);
            };

            if (head)
            {
                CreateStatic(index, 58);
            }
            else
            {
                auto index_btn = std::shared_ptr<CUIStatic>(CreateIndexStatic(index));
                index_btn->UISetSize(28, 28);
                item->Push(index_btn);
                CreateStatic(L"", 20);
            }

            CreateStatic(address, 360);
            
            if (head)
            {
                CreateStatic(website, 320);
            }
            else
            {
                std::shared_ptr<CUISysLink> ui_website(CommonUI::CreateHyperlink(website));
                ui_website->UISetSize(320, 0);
                item->Push(ui_website);
                item->SetClickCallback([website](DWORD dwControlID) {
                    ShellExecute(NULL, _T("open"), website.c_str(), NULL, NULL, SW_SHOW);
                });
            }
            
            CreateStatic(votes, 80);
            std::shared_ptr<CUIObject> ui_myvotes;
            if (head)
            {
                ui_myvotes = std::static_pointer_cast<CUIObject>(std::make_shared<CUIStatic>(myvotes));
            }
            else
            {
                ui_myvotes = std::shared_ptr<CUIObject>(CommonUI::CreateEdit(0, myvotes));
                ui_myvotes->UIAddOrRemoveStyle(true, UIEditStyle_Number);
            }

            ui_myvotes->UISetSize(80, 0);
            item->Push(ui_myvotes);
            return item;
        };

        CUIObject* item = nullptr;
        int row = pView->GetItemIndex(hItem);
        if (row == 0)
        {
            item = CreateListItem(L"#", L"Address", L"Website", L"Votes", L"My Votes", true);
        }
        else
        {
            auto data = (WitnessItemData*)pView->GetItemData(row);
            if (data)
            {
                item = CreateListItem(std::to_wstring(row), data->address, data->website, data->votes, data->myvotes, false);
                delete data;
            }
        }
        item->UISetPainter(new CListItemBKPainter(row));
        return item;
    }
};

enum class ControlID
{
    BTN_ITEM_VOTE = 500,
};

CVoteForm::CVoteForm(bool offline) : CBaseForm(L"Candidates")/*, offline_(offline)*/
{
    offline_ = offline;
    if (offline_)
        vote_.reset(CommonUI::CreateNormalButton(L"Create unauthorized vote", (DWORD)ControlID::BTN_ITEM_VOTE));
    else
        vote_.reset(CommonUI::CreateNormalButton(L"Vote", (DWORD)ControlID::BTN_ITEM_VOTE));
    tip_.reset(new CUIStatic(L"TRX can be frozen/locked to gain Tron Power and enable votings. You can vote for representatives with TP.", true, false, true));
    witness_.reset(new CUIListView);
    witness_->SetItemBKPainter(new CNullPainter);
    witness_->SetSubItemCreator(new CWitnessItemListCreator);
    witness_->SetScrollBar(CommonUI::CreateVScrollBar());
    witness_->UIModifyStyle(UILVStyle_FullRowSelect | UILVStyle_SimpleList
        | UILVStyle_TrackingMouse | UILVStyle_SingleSelection, 0);

}

void CVoteForm::GetWitnessList(std::function<void(bool)> callback)
{
    auto em = std::make_shared<protocol::EmptyMessage>();
    CNetworkThread::_().ListWitnesses(em, [this, callback](std::shared_ptr<protocol::WitnessList> witness_list) {
        UpdateWitnessList(witness_list);
        AutoHideShowStatus(L"");
        callback(true);
    });
    ShowStatus(L"Loading Witness List");
}

void CVoteForm::UpdateWitnessList(std::shared_ptr<protocol::WitnessList> witness_list)
{
    witness_->Clear();
    int index = 0;
    witness_->InsertItem(index);
    witness_->SetItemHeight(index, 40);
    for (int i = 0; i < witness_list->witnesses_size(); i++)
    {
        index++;
        auto w = witness_list->witnesses(i);
        WitnessItemData* data = new WitnessItemData;
        data->address = base::UTF8ToWide(AddressEncodeBase58Check(w.address()));
        data->website = base::UTF8ToWide(w.url());
        data->votes = std::to_wstring(w.votecount());
        witness_->InsertItem(index, false, nullptr, (DWORD_PTR)data);
        witness_->SetItemHeight(index, 40);
    }
    Layout();
}

void CVoteForm::OnUICreate()
{
    UISetSize(1000, 600);
    CBaseForm::OnUICreate();
    tip_->UICreate(this);
    tip_->UISetTextColor(CommonUI::white);
    tip_->UISetFont(CommonUI::UI::GetMiddleFont());
    vote_->UICreate(this);
    witness_->UICreate(this);
    SetTitle(L"Candidates", 100);
}

void CVoteForm::Layout()
{
    CBaseForm::Layout();
    CRect rcClient = UIGetClientRect();
    int top = GetTitleHeight();
    tip_->UISetPos(0, top - 36, rcClient.Width(), 30);
    witness_->UISetPos(CommonUI::kDefaultMargin, top, rcClient.Width() - 2 * CommonUI::kDefaultMargin, 420);
    auto sz = vote_->UIMeasure();
    auto width = std::max<int>(120, sz.cx);
    auto tail_hegiht = rcClient.Height() - witness_->UIGetRect().bottom;
    vote_->UISetPos((rcClient.Width() - width) / 2, rcClient.Height() - tail_hegiht + (tail_hegiht - CommonUI::normal_btn_height) / 2, width, CommonUI::normal_btn_height);
    SetTailHeight(tail_hegiht);
}

void CVoteForm::OnShow(bool show)
{
}

std::shared_ptr<protocol::VoteWitnessContract> CVoteForm::CreateVoteContract()
{
    std::shared_ptr<protocol::VoteWitnessContract> contract;
    std::string address;
    if (!offline_)
    {
        auto& manage = AccountManager::_();
        if (!manage.CheckLogin())
            return contract;

        address = manage.Address();
    }
    else
    {
        address = address_;
    }

    contract = std::make_shared<protocol::VoteWitnessContract>();
    contract->set_owner_address(address);
    auto count = witness_->GetItemCount();
    for (int i = 1; i < count; i++)
    {
        auto item = (CCommonListViewItem*)witness_->GetSubItem(i, 0);
        auto obj = item->GetSub(5);
        auto votes = obj->UIGetText();
        int64_t vote_count = 0;
        if (!votes.empty() && base::StringToInt64(base::WideToUTF8(obj->UIGetText()), &vote_count))
        {
            auto vote_address_obj = item->GetSub(2);
            std::string vote_address;
            auto utf8_vote_address = base::WideToUTF8(vote_address_obj->UIGetText());
            std::tie(std::ignore, vote_address) = AddressDecodeBase58Check(utf8_vote_address);
            auto vote = contract->add_votes();
            vote->set_vote_address(vote_address);
            vote->set_vote_count(vote_count);
            auto website = item->GetSub(3)->UIGetText();
            address_url_.emplace(utf8_vote_address, base::WideToUTF8(website));
        }
    }
    return contract;
}

void CVoteForm::ShowTransaction(std::shared_ptr<protocol::Transaction> transaction)
{
    if (transaction && transaction->raw_data().contract_size() > 0)
    {
        if (vote_info_)
            vote_info_->UIDestroy();

        VoteData data;
        data.transaction = transaction;
        data.address_url = std::move(address_url_);
        address_url_.clear();
        vote_info_.reset(new CVoteInfoForm(data, TransactionStatus::SAVE));
        auto panel = (CShowPanel*)(this->UIGetParent());
        vote_info_->UICreate(panel);
        panel->SetContent(vote_info_.get());

        AutoHideShowStatus(L"");
    }
    else
    {
        OnTransactionCreateFailed(transaction);
    }
}

void CVoteForm::BroadcastTransaction(std::shared_ptr<protocol::Transaction> transaction)
{
    if (SignTransaction(transaction))
    {
        CNetworkThread::_().BroadcastTransaction(transaction, [this](std::shared_ptr<protocol::Return> r) {
            AutoHideShowStatus(r->result() ? L"Vote success" : L"Vote failed");
        });
    }
    else
    {
        OnTransactionCreateFailed(transaction);
    }
}

std::wstring CVoteForm::ValidateContract(std::shared_ptr<protocol::VoteWitnessContract> contract, int64_t tron_power)
{
    std::wstring err;
    do
    {
        if (contract->votes_size() <= 0)
        {
            err = L"VoteNumber must more than 0";
            break;
        }

        int sum = 0;
        for (int i = 0; i < contract->votes_size(); i++)
        {
            const auto& vote = contract->votes(i);
            if (vote.vote_count() < 0)
            {
                err = L"vote count must be greater than 0";
                break;
            }

            sum += vote.vote_count();
        }

        if (err.empty())
        {
            if (tron_power < ToDrop(sum))
            {
                err = base::StringPrintf(L"The total number of votes %.6f is greater than the tronPower %.6f", ToTrx(sum), ToTrx(tron_power));
                break;
            }
        }

    } while (false);

    return err;
}


void CVoteForm::OnButtonClick(DWORD dwControlID)
{
    if (dwControlID == (DWORD)ControlID::BTN_ITEM_VOTE)
    {
        auto contract = CreateVoteContract();
        if (contract)
        {
            CNetworkThread::_().GetAccountWrapper(contract->owner_address(), [contract, this](std::shared_ptr<::protocol::Account> a)
            {
                if (a->address().empty())
                {
                    MessageBox(base::StringPrintf(L"Account %s not exists", base::UTF8ToWide(AddressEncodeBase58Check(contract->owner_address()))).c_str());
                    return;
                }

                auto err = ValidateContract(contract, CalcTronPower(a));
                if (!err.empty())
                {
                    MessageBox(err);
                    return;
                }

                CNetworkThread::_().VoteWitnessAccount(contract, [this](
                    std::shared_ptr<protocol::Transaction> transaction)
                {
                    offline_ ? ShowTransaction(transaction) : BroadcastTransaction(transaction);
                });

                ShowStatus(L"Creating vote witness contract");
            });
        }
    }
}

