#pragma once
#include "base_form.h"
#include "UILite/Controls/ListView/UIListView.h"
#include "address_text_form.h"

namespace wallet_gui
{
    class CAddressListForm : public CBaseForm
    {
    public:
        CAddressListForm();
        void Update(const std::vector<std::string>& addresses);
        void OnAddAddress();

    protected:
        void OnUICreate() override;
        void Layout() override;

    private:
        std::unique_ptr<CUIListView> addresses_;
        std::unique_ptr<CAddressTextForm> address_text_form_;
    };
}