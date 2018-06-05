#include "transactions_form.h"
#include "UILite/Controls/UILabel.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "transaction_item.h"
#include "core/core_util.h"

#define TRANSACTION_LIST_HEAD_HEIGHT 35
#define TRANSACTION_LIST_ITEM_HEIGHT 20
#define TRANSACTION_ITEM_VERT_GAP 20
#define TRANSACTION_ITEM_VERT_HEIGHT 30

using namespace wallet_core;

namespace wallet_gui
{
    class CTransactionsItemListCreator : public CUIListViewSubItemCreator
    {
    public:

        CUIObject* OnCreateObject(CUIListView* pView, HLISTVIEWITEM hItem, const int iCol) override
        {
            CUIObject* item = nullptr;
            int row = pView->GetItemIndex(hItem);
            if (row == 0)
                item = new CTransactionItem(L"From", L"", L"To", L"Value", L"Time");
            else
            {
                auto data = (TransactionData*)pView->GetItemData(row);
                if (data)
                {
                    item = new CTransactionItem(data->from, data->in ? L"in" : L"out", data->to, data->value, data->time);
                    delete data;
                }
            }
            item->UISetPainter(new CommonUI::CListItemBKPainter(row));
            return item;
        }
    };

    CTransactionsForm::CTransactionsForm() : CBaseForm(L"")
    {
        transctions_.reset(new CUIListView);
        transctions_->SetItemBKPainter(new CNullPainter);
        transctions_->SetSubItemCreator(new CTransactionsItemListCreator);
        transctions_->SetScrollBar(CommonUI::CreateVScrollBar());

        transctions_->UIModifyStyle(UILVStyle_FullRowSelect | UILVStyle_SimpleList
            | UILVStyle_TrackingMouse | UILVStyle_SingleSelection, 0);
    }

    void CTransactionsForm::Push(const std::vector<TransactionData*>& datas)
    {
        datas_.insert(datas_.end(), datas.begin(), datas.end());
    }

    void CTransactionsForm::UpdateTransaction()
    {
        transctions_->Clear();
        int index = 0;
        transctions_->InsertItem(index);
        transctions_->SetItemHeight(index, 40);
        for (auto data : datas_)
        {
            index++;
            transctions_->InsertItem(index, false, nullptr, (DWORD_PTR)data);
            transctions_->SetItemHeight(index, 40);
        }
        datas_.clear();
    }

    void CTransactionsForm::OnUICreate()
    {
        UISetSize(1100, 680);
        CBaseForm::OnUICreate();
        transctions_->UICreate(this);
        SetTitle(L"Transactions", 80);
    }

    void CTransactionsForm::Layout()
    {
        CBaseForm::Layout();
        CRect rcClient = UIGetClientRect();
        int top = GetTitleHeight();
        transctions_->UISetPos(0, top, rcClient.Width(), rcClient.Height() - top);
    }
}


