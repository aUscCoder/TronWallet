#pragma once
#include "base_form.h"
#include "UILite/Controls/UIEdit.h"
#include "UILite/Controls/UIButton.h"

namespace wallet_gui
{
    class CTokenIssueForm : public CBaseForm
    {
    public:
        CTokenIssueForm();

        BEGIN_UI_EVENT_MAP(CTokenIssueForm)
            UI_EVENT_SHOWUI(OnShowUI)
        END_UI_EVENT_MAP()

        void OnShowUI(bool show);
    private:
        void OnUICreate() override;
        void OnButtonClick(DWORD dwControlID) override;
        void OnEditTextChange(DWORD dwControlID) override;
        void CheckAndIssue();
    };
}
