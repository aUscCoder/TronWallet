#pragma once
#include "base_form.h"
#include "UILite/Controls/ListView/UIListView.h"

namespace protocol { class AssetIssueList; }

namespace wallet_gui
{
    struct IssueItemData
    {
        
        std::wstring name;
        std::wstring issuer;
        uint64_t total_supply = 0;
        uint64_t start_time;
        uint64_t end_time;
        std::wstring description;
        std::wstring url;
        int64_t avaliable_count = 0;
        double exchange_rate = 0.0;
    };

    class CTokenListForm : public CBaseForm
    {
    public:
        CTokenListForm(bool participate = true);
        void UpdateIssueList(std::shared_ptr<protocol::AssetIssueList> issue_list);
        CUIButton* GetActionButton(int64_t stime, int64_t etime);
        void OnActionButtonClick(IssueItemData data);

    protected:
        void OnUICreate() override;
        void Layout() override;

    private:
        bool participate_ = true;
        std::unique_ptr<CUIListView> issue_list_;
    };
}
