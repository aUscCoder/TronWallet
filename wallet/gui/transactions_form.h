#pragma once
#include "base_form.h"
#include "UILite/Controls/ListView/UIListView.h"

namespace wallet_gui
{
    struct TransactionData
    {
        bool in;
        std::wstring from;
        std::wstring to;
        std::wstring value;
        std::wstring time;
    };

    class CTransactionsForm : public CBaseForm
    {
    public:
        CTransactionsForm();
        void Push(const std::vector<TransactionData*>& datas);
        void UpdateTransaction();

    protected:

        void OnUICreate() override;
        void Layout() override;

    private:
        std::unique_ptr<CUIListView> transctions_;
        std::vector<TransactionData*> datas_;
    };
}
