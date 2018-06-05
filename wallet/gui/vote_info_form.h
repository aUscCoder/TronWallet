#pragma once
#include "base_form.h"
#include "UILite/Controls/UILabel.h"
#include "UILite/Controls/UIButton.h"
#include "UILite/Controls/ListView/UIListView.h"
#include "core/transaction_manager.h"

using namespace wallet_core;
namespace protocol { class Transaction; }

namespace wallet_gui
{
    class CVoteInfoForm : public CBaseForm
    {
    public:
        CVoteInfoForm(const VoteData& vote_data, TransactionStatus s);

    protected:
        void OnUICreate() override;
        void OnButtonClick(DWORD dwControlID) override;
        void Layout() override;

    private:
        TransactionStatus s_;
        VoteData vote_data_;
        std::string owner_address_;
        std::unique_ptr<CUIStatic> status_;
        std::unique_ptr<CUIListView> infos_;
        std::unique_ptr<CUIButton> save_;
        std::unique_ptr<CUIButton> cancel_;
    };
}