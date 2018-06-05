#pragma once
#include "base_form.h"

namespace wallet_gui
{
    class CConnectionForm : public CBaseForm
    {
    public:
        CConnectionForm();

    protected:
        void OnUICreate() override;
        void OnButtonClick(DWORD dwControlID) override;
        void Save(const std::string& full_address, const std::string& full_port,
            const std::string& solidity_address, const std::string& solidity_port);
        void Reset();
    };
}