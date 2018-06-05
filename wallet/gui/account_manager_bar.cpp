#include "account_manager_bar.h"
#include "common_ui/CommonDef.h"
#include "UILite/Controls/UIButton.h"
#include "wallet_evnets.h"
#include "common_ui/UI.h"
#include "core/account_manager.h"

enum class ControlID
{
    BTN_REGISTER_LOGIN = 50,
    BTN_SIGNOUT,
};

using namespace wallet_core;
using namespace wallet_gui;

CAccountBar::CAccountBar()
{
    btn_registe_login_.reset(CommonUI::CreateNormalButton(L"New wallet / Login", (DWORD)ControlID::BTN_REGISTER_LOGIN));
    btn_signout_.reset(CommonUI::CreateNormalButton(L"Sign Out", (DWORD)ControlID::BTN_SIGNOUT));
}

void CAccountBar::OnUISize()
{
    auto rc = UIGetClientRect();
    auto SetPos = [&rc](std::unique_ptr<CUIButton>& btn)
    {
        auto sz = btn->UIMeasure();
        auto width = sz.cx + 2 * CommonUI::kDefaultMargin;
        btn->UISetPos(rc.right - CommonUI::kDefaultMargin - width, 0, width, 28);
    };
    
    SetPos(btn_registe_login_);
    SetPos(btn_signout_);
    UIRedraw();
}

void CAccountBar::OnUICreate()
{
    btn_registe_login_->UICreate(this);
    btn_signout_->UIShow(false);
    btn_signout_->UICreate(this);

    if (AccountManager::_().HasAccount())
    {
        btn_registe_login_->UISetText(L"Login");
    }
}

void CAccountBar::ShowSignOut(bool show)
{
    btn_registe_login_->UISetText(AccountManager::_().HasAccount() ? L"Login" : L"New wallet / Login");   
    btn_registe_login_->UIShow(!show);
    btn_signout_->UIShow(show);
    OnUISize();
}

LRESULT CAccountBar::OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    ControlID id = (ControlID)dwControlID;
    switch (id)
    {
    case ControlID::BTN_REGISTER_LOGIN:
        if (AccountManager::_().HasAccount())
            ::PostMessage(UIGetHWND(), Event_Show_Login, 0, 0);
        else
            ::PostMessage(UIGetHWND(), Event_Show_Register, 0, 0);
        break;
    case ControlID::BTN_SIGNOUT:
        AccountManager::_().Logout();
        ::PostMessage(UIGetHWND(), Event_AccountBar_Login, 0, AccountManager::_().HasAccount() ? 1 : 0);
        break;
    default:
        break;
    }
    return 0;
}

