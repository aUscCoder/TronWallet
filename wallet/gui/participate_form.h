#pragma once
#include "base_form.h"
#include "token_list_form.h"
#include "UILite/Controls/UILabel.h"
#include "UILite/Controls/UIProgressBar.h"

namespace wallet_gui
{

    class CParticipateForm : public CBaseForm
    {
    public:
        CParticipateForm();
        void UpdateItemData(const IssueItemData& data);

    protected:

        void OnUICreate() override;
        void Layout() override;
        void OnButtonClick(DWORD dwControlID) override;
        void OnEditTextChange(DWORD dwControlID) override;
        
    private:
        void Participate(const std::string& owner, const std::string& to, const std::string& assert_name, int64_t amount);
        IssueItemData data_;
        std::unique_ptr<CUIStatic> amount_assistant_;
        std::unique_ptr<CUIProgressBar> progress_;
    };
}

