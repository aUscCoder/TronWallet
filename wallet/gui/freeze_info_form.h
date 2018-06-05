#pragma once
#include "transaction_info_form.h"

namespace wallet_gui
{
    using FreezeInfo = std::tuple<std::string, int64_t, int64_t, bool>;

    class CFreezeInfoForm : public CTransactionInfoForm
    {
    public:
        CFreezeInfoForm(std::shared_ptr<protocol::Transaction> transaction, TransactionStatus s);

    protected:
        void OnUICreate() override;
        void OnButtonClick(DWORD dwControlID) override;

    private:
        FreezeInfo ParseTransaction();
    };
}
