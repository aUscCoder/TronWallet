#pragma once
#include "base_form.h"
#include "base/stringprintf.h"
#include "common_ui/CommonDef.h"
#include "common_ui/UI.h"
#include "UILite/Controls/ListView/UIListView.h"
#include "transaction_info_form.h"
#include "vote_info_form.h"
#include "freeze_info_form.h"

namespace wallet_gui
{ 
    template<typename T>
    class CLoadedForm;

    template<typename T>
    class CItemListCreator : public CUIListViewSubItemCreator
    {
    public:
        CItemListCreator(const std::wstring& item_prex, CLoadedForm<T>* form) : item_prefix_(item_prex), form_(form) {}
        CUIObject* OnCreateObject(CUIListView* pView, HLISTVIEWITEM hItem, const int iCol) override
        {
            int row = pView->GetItemIndex(hItem);
            auto name = base::StringPrintf(L"%s %d", item_prefix_.c_str(), row + 1);
            auto item = new CommonUI::CCommonListViewItem;
            item->SetEqually(true);

            auto obj = CommonUI::CreateNormalButton(name.c_str(), 0);
            item->Push(std::shared_ptr<CUIObject>(obj));

            item->SetClickCallback([row, this](DWORD id) {
                form_->OnItemClick(row);
            });
            return item;
        }

    private:
        std::wstring item_prefix_;
        CLoadedForm<T>* form_ = nullptr;
    };

    template<typename T>
    inline std::unique_ptr<CUIObject> CreateShowInfoControl(T& data, TransactionStatus s)
    {
        return nullptr;
    }

    template<>
    inline std::unique_ptr<CUIObject> CreateShowInfoControl(std::shared_ptr<protocol::Transaction>& data, TransactionStatus s)
    {
        if (data)
        {
            if (data->raw_data().contract_size() > 0)
            {
                auto& contract = data->raw_data().contract(0);
                if (contract.type() == protocol::Transaction_Contract_ContractType_TransferContract
                    || contract.type() == protocol::Transaction_Contract_ContractType_TransferAssetContract)
                {
                    return std::unique_ptr<CTransactionInfoForm>(new CTransactionInfoForm(data, s));
                }
                else if (contract.type() == protocol::Transaction_Contract_ContractType_FreezeBalanceContract)
                {

                    return std::unique_ptr<CFreezeInfoForm>(new CFreezeInfoForm(data, s));
                }
            }
        }
        return nullptr;
    }

    template<>
    inline std::unique_ptr<CUIObject> CreateShowInfoControl(wallet_core::VoteData& data, TransactionStatus s)
    {
        return std::unique_ptr<CVoteInfoForm>(new CVoteInfoForm(data, s));
    }

    template<typename T>
    class CLoadedForm : public CBaseForm
    {
    public:
        CLoadedForm(const std::wstring& title, const std::wstring& item_prex, TransactionStatus s) : CBaseForm(title), s_(s)
        {
            items_.reset(new CUIListView);
            items_->SetItemBKPainter(new CNullPainter);
            items_->SetSubItemCreator(new CItemListCreator<T>(item_prex, this));
            items_->SetScrollBar(CommonUI::CreateVScrollBar());
            items_->UIModifyStyle(UILVStyle_FullRowSelect | UILVStyle_SimpleList
                | UILVStyle_TrackingMouse | UILVStyle_SingleSelection, 0);
        }

        ~CLoadedForm()
        {
            items_->UIDestroy();
            info_control_->UIDestroy();
        }

        void LoadDatas(const std::vector<T>& datas)
        {
            datas_ = datas;
            items_->Clear();
            for (size_t i = 0; i < datas.size(); i++)
            {
                items_->InsertItem(i);
                items_->SetItemHeight(i, 40);
            }
        }

        void OnItemClick(int row)
        {
            if ((int)datas_.size() > row)
            {
                if (info_control_)
                    info_control_->UIDestroy();

                info_control_ = std::move(CreateShowInfoControl(datas_[row], s_));
                auto parent = (CShowPanel*)UIGetParent();
                info_control_->UICreate(parent);
                parent->SetContent(info_control_.get());
            }
        }

    protected:
        
        void OnUICreate() override
        {
            UISetSize(400, 400);
            CBaseForm::OnUICreate();
            items_->UICreate(this);
        }

        void Layout() override
        {
            CBaseForm::Layout();
            auto rc_title = title_->UIGetRect();
            auto rc = UIGetClientRect();
            int top = rc_title.bottom + CommonUI::kDefaultMargin;
            items_->UISetPos(CommonUI::kDefaultMargin, top, rc.Width() - 2 * CommonUI::kDefaultMargin, 300);
        }

    private:
        TransactionStatus s_ = TransactionStatus::SAVE;
        std::unique_ptr<CUIListView> items_;
        std::vector<T> datas_;
        std::unique_ptr<CUIObject> info_control_;
    };
}