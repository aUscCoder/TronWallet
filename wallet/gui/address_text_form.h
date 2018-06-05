#pragma once
#include "base_form.h"

namespace wallet_gui
{
    using Receive = std::function<void(const std::string&)>;

    class CAddressTextForm : public CBaseForm
    {
    public:
        enum Type
        {
            Import = 0,
            Generate,
        };
        CAddressTextForm(Type t, Receive cllback);

    protected:
        void OnUICreate() override;
        void Layout() override;
        void OnButtonClick(DWORD dwControlID) override;

    private:
        void GenerateAddresses();

    private:
        Type type_;
        Receive callback_ = nullptr;
        std::unique_ptr<CUIStatic> detail_;
        std::unique_ptr<CUIEdit> textbox_;
        std::unique_ptr<CUIButton> btn_;
    };
}
