#include "token_list_form.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "api/api.pb.h"
#include "core/core_util.h"
#include "wallet_evnets.h"

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;


class CIssueItemListCreator : public CUIListViewSubItemCreator
{
public:
    CIssueItemListCreator(CTokenListForm* form) : form_(form) {}
    CUIObject* OnCreateObject(CUIListView* pView, HLISTVIEWITEM hItem, const int iCol) override
    {
        auto CreateListItem = [this](const IssueItemData& data, bool head)
        {
            auto item = new CCommonListViewItem();
            auto CreateStatic = [&item](const std::wstring& text, int width)
            {
                auto ui = std::make_shared<CUIStatic>(text);
                ui->UISetSize(width, 0);
                item->Push(ui);
            };

            CreateStatic(L"", 20);
            if (head)
            {
                CreateStatic(L"Name", 120);
                CreateStatic(L"Issuer", 320);
                CreateStatic(L"Total Supply", 120);
                CreateStatic(L"Start Time", 160);
                CreateStatic(L"End Time", 160);
            }
            else
            {
                CreateStatic(data.name, 120);
                CreateStatic(data.issuer, 320);
                CreateStatic(std::to_wstring(data.total_supply), 120);
                CreateStatic(GetTime(data.start_time), 160);
                CreateStatic(GetTime(data.end_time), 160);

                CUIButton* btn = form_->GetActionButton(data.start_time, data.end_time);
                btn->UISetSize(80, 24);
                item->Push(std::shared_ptr<CUIObject>(btn));
            }

            item->SetClickCallback(std::bind(&CTokenListForm::OnActionButtonClick, form_, data));
            return item;
        };

        CUIObject* item = nullptr;
        int row = pView->GetItemIndex(hItem);
        if (row == 0)
        {
            item = CreateListItem({}, true);
        }
        else
        {
            auto data = (IssueItemData*)pView->GetItemData(row);
            if (data)
            {
                item = CreateListItem(*data, false);
                delete data;
            }
        }
        item->UISetPainter(new CommonUI::CListItemBKPainter(row));
        return item;
    }

private:
    CTokenListForm* form_ = nullptr;
};

CTokenListForm::CTokenListForm(bool participate) : CBaseForm(L""), participate_(participate)
{
    issue_list_.reset(new CUIListView);
    issue_list_->SetItemBKPainter(new CNullPainter);
    issue_list_->SetSubItemCreator(new CIssueItemListCreator(this));
    issue_list_->SetScrollBar(CommonUI::CreateVScrollBar());
    issue_list_->UIModifyStyle(UILVStyle_FullRowSelect | UILVStyle_SimpleList
        | UILVStyle_TrackingMouse | UILVStyle_SingleSelection, 0);
}

CUIButton* CTokenListForm::GetActionButton(int64_t stime, int64_t etime)
{
    CUIButton* btn = nullptr;
    if (participate_ && etime < currentTimeMillis())
    {
        btn = CommonUI::CreateCancelButton(L"Expired", 800);
        btn->UIDisable();
    }
    else if (participate_ && stime > currentTimeMillis())
    {
        btn = CommonUI::CreateCancelButton(L"Not started", 800);
        btn->UIDisable();
    }
    else
    {
        btn = CommonUI::CreateNormalButton(participate_ ? L"Participate" : L"Modify", 800);
    }
    return btn;
}

void CTokenListForm::OnActionButtonClick(IssueItemData data)
{
    if (participate_)
    {
        auto forword = new IssueItemData(data);
        ::PostMessage(UIGetHWND(), Event_Show_Participate, (WPARAM)forword, 0);
    }
    else
    {
        ::PostMessage(UIGetHWND(), Event_Show_Modify_Token, 0, 0);
    }
}

void CTokenListForm::UpdateIssueList(std::shared_ptr<protocol::AssetIssueList> issue_list)
{
    issue_list_->Clear();
    int index = 0;
    issue_list_->InsertItem(index);
    issue_list_->SetItemHeight(index, 40);
    for (int i = 0; i < issue_list->assetissue_size(); i++)
    {
        index++;
        const auto& issue = issue_list->assetissue(i);
        IssueItemData* data = new IssueItemData;
        data->url = base::UTF8ToWide(issue.url());
        data->name = base::UTF8ToWide(issue.name());
        data->issuer = base::UTF8ToWide(AddressEncodeBase58Check(issue.owner_address()));
        data->total_supply = issue.total_supply();
        data->start_time = issue.start_time();
        data->end_time = issue.end_time();
        data->description = base::UTF8ToWide(issue.description());
        data->exchange_rate = ToTrx(issue.trx_num()) / issue.num();
        issue_list_->InsertItem(index, false, nullptr, (DWORD_PTR)data);
        issue_list_->SetItemHeight(index, 40);
    }
    Layout();
}

void CTokenListForm::OnUICreate()
{
    CBaseForm::OnUICreate();
    issue_list_->UICreate(this);
    UISetSize(1100, 600);
}

void CTokenListForm::Layout()
{
    CBaseForm::Layout();
    CRect rcClient = UIGetClientRect();
    issue_list_->UISetPos(CommonUI::kDefaultMargin, 0, rcClient.Width() - 2* CommonUI::kDefaultMargin, rcClient.Height());
}