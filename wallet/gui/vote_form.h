#pragma once
#include <memory>
#include "CommonInclude.h"
#include "UILite/Controls/UIControlEvent.h"
#include "UILite/Controls/UIControlDef.h"
#include "UILite/Core/UIObject.h"
#include "UILite/Controls/UIButton.h"
#include "UILite/Controls/UIEdit.h"
#include "UILite/Controls/UILabel.h"
#include "UILite/Controls/ListView/UIListView.h"
#include "UILite/UICrack.h"
#include "vote_info_form.h"
#include "base_form.h"

namespace protocol
{
    class WitnessList;
    class VoteWitnessContract;
    class Transaction;
}

namespace wallet_gui
{
    class CVoteForm : public CBaseForm
    {
    public:
        CVoteForm(bool offline = false);
        void SetAddress(const std::string& address) { address_ = address; }
        void GetWitnessList(std::function<void(bool)> callback);

    protected:
        BEGIN_UI_EVENT_MAP(CVoteForm)
            UI_EVENT_SHOWUI(OnShow)
        END_UI_EVENT_MAP()

        void OnUICreate() override;
        void Layout() override;
        void OnButtonClick(DWORD dwControlID) override;
        void OnShow(bool show);

    private:
        void UpdateWitnessList(std::shared_ptr<protocol::WitnessList> witness_list);
        std::shared_ptr<protocol::VoteWitnessContract> CreateVoteContract();
        void ShowTransaction(std::shared_ptr<protocol::Transaction> transaction);
        void BroadcastTransaction(std::shared_ptr<protocol::Transaction> transaction);
        std::wstring ValidateContract(std::shared_ptr<protocol::VoteWitnessContract> contract, int64_t tron_power);

    private:
        bool offline_ = false;
        std::string address_;
        std::unique_ptr<CUIStatic> tip_;
        std::unique_ptr<CUIListView> witness_;
        std::unique_ptr<CUIButton> vote_;
        std::unique_ptr<CVoteInfoForm> vote_info_;
        std::map<std::string, std::string> address_url_;
    };
}
