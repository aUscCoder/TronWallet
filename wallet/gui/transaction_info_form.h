#pragma once
#include "base_form.h"
#include "core/transaction_manager.h"

namespace protocol { class Transaction; }
using namespace wallet_core;

namespace wallet_gui
{
    using TransactionInfo = std::tuple<std::string, std::string, std::string, double, bool>;
    class CTransactionInfoForm : public CBaseForm
    {
    public:
        CTransactionInfoForm(std::shared_ptr<protocol::Transaction> transaction,
            TransactionStatus s, const std::wstring& title = L"Transaction");

    protected:
        void OnUICreate() override;
        void OnButtonClick(DWORD dwControlID) override;
        void HandleClickEvent(const std::string& address);

    private:
        TransactionInfo ParseTransaction();

    protected:
        TransactionStatus s_ = TransactionStatus::SAVE;
        std::shared_ptr<protocol::Transaction> transaction_;
    };
}