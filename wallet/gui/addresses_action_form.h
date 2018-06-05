#pragma once
#include "base_form.h"
#include "UILite/Controls/ListView/UIListView.h"
#include "send_form.h"
#include "vote_form.h"
#include "freeze_form.h"
#include "balance_form.h"

class CActionItem;

namespace wallet_gui
{
    class CAddressActionForm : public CBaseForm
    {
    public:
        CAddressActionForm(const std::wstring& title, const std::wstring& action_btn_name);
        void Update(const std::set<std::string>& addresses);

    protected:

        void OnUICreate() override;
        void Layout() override;
        virtual void OnActionBtnClick(const std::wstring& address, DWORD id) = 0;

    private:
        CBitmap eye_normal_bmp_;
        CBitmap eye_select_bmp_;
        std::wstring action_btn_name_;
        std::unique_ptr<CUIListView> action_list_;
        friend class CActionItem;
    };

    class CSendActionForm : public CAddressActionForm
    {
    public:
        CSendActionForm() : CAddressActionForm(L"Send From", L"Send"), send_form_(true) {}
        void OnActionBtnClick(const std::wstring& address, DWORD id) override;

    protected:
        void OnUICreate() override;

    private:
        CSendForm send_form_;
    };

    class CVoteActionForm : public CAddressActionForm
    {
    public:
        CVoteActionForm() : CAddressActionForm(L"Vote From", L"Vote"), vote_form_(true) { }
        void OnActionBtnClick(const std::wstring& address, DWORD id) override;

    protected:
        void OnUICreate() override;

    private:
        CVoteForm vote_form_;
    };

    class CFreezeActionForm : public CAddressActionForm
    {
    public:
        CFreezeActionForm() : CAddressActionForm(L"Freeze From", L"Freeze"), freeze_form_(true) {}
        void OnActionBtnClick(const std::wstring& address, DWORD id) override;

    protected:
        void OnUICreate() override;

    private:
        CFreezeForm freeze_form_;
    };


    class CBalanceActionForm : public CAddressActionForm
    {
    public:
        CBalanceActionForm() : CAddressActionForm(L"Balance From", L"show balance"), balance_form_(true){}
        void OnActionBtnClick(const std::wstring& address, DWORD id) override;

    protected:
        void OnUICreate() override;

    private:
        CBalanceForm balance_form_;
    };

}
