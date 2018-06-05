#pragma once
#include <map>
#include "base_form.h"
#include "UILite/Controls/UIButton.h"
#include "UILite/Controls/UIEdit.h"
#include "UILite/Controls/UIComboBox.h"
#include "transaction_info_form.h"

namespace wallet_gui
{
    class CSendForm : public CBaseForm
    {
    public:
        CSendForm(bool offline = false);
        void UpdteAddress(const std::string& address, std::function<void(bool)> callback);

    protected:
        BEGIN_UI_EVENT_MAP(CSendForm)
            UI_EVENT_SHOWUI(OnShow)
            UI_COMMAND_CODE(UICBN_SELCHANGED, OnUISelChanged)
        END_UI_EVENT_MAP()

        void OnUICreate() override;
        void Layout() override;
        void OnShow(bool bShow);
        void OnButtonClick(DWORD dwControlID) override;
        COLORREF OnUIGetTextColor();
        LRESULT OnUISelChanged(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

    private:
        std::tuple<bool, std::string, std::string, std::string, double> GetSendInfo();
        void SendToken(const std::string& from, const std::string& to, const std::string& type, int64_t amount);

        void CreateTransaction(const std::string& from, const std::string& to,
            const std::string& type, int64_t amount, const std::function<void(std::shared_ptr<protocol::Transaction>)>& callback);

        void CreateUnsignedTransaction(const std::string& from, const std::string& to, const std::string& type, int64_t amount);

        double GetAvaliable(const std::string& type);
        void ShowAvaliable();

    private:
        bool offline_ = false;
        std::string address_;
        std::unique_ptr<CUIStatic> label_to_;
        std::unique_ptr<CUIEdit> to_;
        std::unique_ptr<CUIStatic> label_token_;
        std::unique_ptr<CUIComboBox> token_;
        std::unique_ptr<CUIStatic> label_amount_;
        std::unique_ptr<CUIEdit> amount_;
        std::unique_ptr<CUIStatic> avaliable_amount_;
        std::unique_ptr<CUIButton> btn_send_;
        std::unique_ptr<CUIButton> btn_cancel_;
        std::unique_ptr<CTransactionInfoForm> transaction_info_;
        std::unordered_map<std::string, double> assert_;
    };
}



