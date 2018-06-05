#pragma once
#include "base_form.h"

namespace wallet_gui
{
    class CTokenModifyForm : public CBaseForm
    {
    public:
        CTokenModifyForm();

    protected:
        void OnUICreate() override;
        void OnButtonClick(DWORD dwControlID) override;
    };
}
