#include "login_form.h"
#include "base/sys_string_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "wallet_evnets.h"
#include "core/network_thread.h"
#include "core/account_manager.h"
#include "UILite/Controls/UIEdit.h"

#define LOGIN_WIDTH 420
#define LOGIN_HEIGHT 300

enum class ControlID
{
    EDIT_LOGIN_PASSWORD = 200,
    BTN_LOGIN,
    BTN_CANCEL,
    BTN_REGISTER,
};

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

CLoginForm::CLoginForm() : CBaseForm(L"Login", true)
{
    password_.reset(CreateEdit((DWORD)ControlID::EDIT_LOGIN_PASSWORD, L"", L"Password"));
    login_.reset(CreateNormalButton(L"Login", (DWORD)ControlID::BTN_LOGIN));
    cancel_.reset(CreateNormalButton(L"Cancel", (DWORD)ControlID::BTN_CANCEL));
    register_.reset(CreateTextColorButton(L"Or create a new wallet", (DWORD)ControlID::BTN_REGISTER));
}

void CLoginForm::OnUICreate()
{
    UISetSize(LOGIN_WIDTH, LOGIN_HEIGHT);
    CBaseForm::OnUICreate();
    password_->UICreate(this);
    login_->UICreate(this);
    cancel_->UICreate(this);
    password_->UIModifyStyle(UIEditStyle_Password, 0);
    password_->UISetTextColor(CommonUI::white);
    password_->UISetCaretColor(CommonUI::white);
    register_->UICreate(this);
    SetTitle(L"Login", 80);
}

void CLoginForm::Layout()
{
    CBaseForm::Layout();
    auto top = GetTitleHeight();
    auto rc = UIGetClientRect();
    auto left = (rc.Width() - 320) / 2;
    password_->UISetPos(left, top, 320, CommonUI::normal_btn_height);
    top += 40 + CommonUI::normal_btn_height;
    login_->UISetPos(left, top, 120, CommonUI::normal_btn_height);
    cancel_->UISetPos(rc.Width() - left - 120, top, 120, CommonUI::normal_btn_height);
    top += 20 + CommonUI::normal_btn_height;
    register_->UISetPos((rc.Width() - 200) / 2, top, 200, CommonUI::normal_btn_height);
}

void CLoginForm::OnShowUI(bool show)
{
    if (!show)
    {
        password_->UISetText(L"");
    }
    else
    {
        if (AccountManager::_().HasAccount())
            register_->UIShow(false);
    }
}

void CLoginForm::OnOk()
{
    CNetworkThread::_().Login(base::SysWideToUTF8(password_->UIGetText()), [this](std::string err) {
        AutoHideShowStatus(err.empty() ? L"Login success" : base::UTF8ToWide(err));
        if (err.empty())
        {
            UIShow(false);
            ::PostMessage(UIGetHWND(), Event_AccountBar_Login, 1, 0);
        }
    });
    ShowStatus(L"Logging in");
}

void CLoginForm::OnButtonClick(DWORD dwControlID)
{
    ControlID id = (ControlID)dwControlID;
    switch (id)
    {
    case ControlID::BTN_LOGIN:
        OnOk();
        break;
    case ControlID::BTN_CANCEL:
        UIShow(false);
        break;
    case ControlID::BTN_REGISTER:
        ::PostMessage(UIGetHWND(), Event_Show_Register, 0, 0);
        break;
    default:
        break;
    }
}

LRESULT CLoginForm::OnReturn(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    OnOk();
    return S_OK;
}

void CLoginForm::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    CBaseForm::OnUIPaint(dc, dwStatus);
    if (!AccountManager::_().HasAccount())
    {
        auto rc = UIGetClientRect();
        auto left = (rc.Width() - 320) / 2;
        auto register_rc = register_->UIGetRect();

        Gdiplus::Graphics g(dc);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        Gdiplus::Pen pen(Gdiplus::Color(255, 67, 67, 67), 2);
        pen.SetDashStyle(Gdiplus::DashStyleDash);

        auto y = register_rc.CenterPoint().y;
        Gdiplus::PointF s1(left, y);
        Gdiplus::PointF e1(left + 60, y);
        Gdiplus::PointF s2(rc.Width() - left - 60, y);
        Gdiplus::PointF e2(rc.Width() - left, y);
        g.DrawLine(&pen, s1, e1);
        g.DrawLine(&pen, s2, e2);
    }    
}
