#include "register_form.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "core/network_thread.h"
#include "wallet_evnets.h"
#include "UILite/Controls/UIEdit.h"

#define REGIESTRE_WIDTH 520
#define REGIESTRE_HEIGHT 400

enum class ControlID
{
    EDIT_PASSWORD = 300,
    EDIT_NAME,
    EDIT_CONFIRM_PASSWORD,
    BTN_CREATE_ACCOUNT,
    BTN_LOGIN,
};

static const std::vector<ControlID> edit_ids_ = { ControlID::EDIT_PASSWORD, ControlID::EDIT_NAME, ControlID::EDIT_CONFIRM_PASSWORD };

using namespace wallet_gui;
using namespace CommonUI;

CRegisterForm::CRegisterForm() : CBaseForm(L"Create Wallet", true)
{
}

void CRegisterForm::OnUICreate()
{
    UISetSize(REGIESTRE_WIDTH, REGIESTRE_HEIGHT);
    item_infos_ =
    {
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_NAME, L"Name" } }, { 120, 28 }, { 0, normal_btn_height} },
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_PASSWORD, L"Password" } }, { 120, 28},{ 0, normal_btn_height } },
        { { { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_CONFIRM_PASSWORD, L"Confirm password" } } , { 120, 40},{ 0, normal_btn_height } },
        { { { ControlType::NORMAL_BUTTON, L"Create wallet", (DWORD)ControlID::BTN_CREATE_ACCOUNT } }, { 180, 20 },{ 0, normal_btn_height } },
        { { { ControlType::TEXT_COLOR_BUTTON, L"Or login", (DWORD)ControlID::BTN_LOGIN }},{ 220, 20 },{ 0, normal_btn_height } },

    };
    CBaseForm::OnUICreate();

    for (auto i : edit_ids_)
    {
        CUIEdit* edit = (CUIEdit*)GetSub((DWORD)i);
        if (i != ControlID::EDIT_NAME)
            edit->UIModifyStyle(UIEditStyle_Password, 0);

        edit->UISetTextColor(white);
        edit->UISetCaretColor(white);
    }
    SetTitle(L"Create Wallet", 80);
}

void CRegisterForm::OnShowUI(bool show)
{
    if (!show)
    {
        for (auto i : edit_ids_)
        {
            CUIEdit* edit = (CUIEdit*)GetSub((DWORD)i);
            edit->UISetText(L"");
        }
    }
}

std::tuple<std::string, std::string> CRegisterForm::GetPassword()
{
    std::tuple<std::string, std::string> result;
    auto password = GetSub((DWORD)ControlID::EDIT_PASSWORD)->UIGetText();
    auto confirm = GetSub((DWORD)ControlID::EDIT_CONFIRM_PASSWORD)->UIGetText();
    auto name = GetSub((DWORD)(ControlID::EDIT_NAME))->UIGetText();

    do
    {
        if (name.empty())
        {
            ::MessageBox(UIGetHWND(), L"Please input wallet name", L"", MB_OK);
            break;
        }
        if (password.empty() || confirm.empty())
        {
            ::MessageBox(UIGetHWND(), L"Please input password and confirm password", L"", MB_OK);
            break;
        }
        else if (password != confirm)
        {
            ::MessageBox(UIGetHWND(), L"Password and Confirm password must same", L"", MB_OK);
            break;
        }
        else
        {
            result = std::make_tuple(base::WideToUTF8(name), base::WideToUTF8(password));
        }

    } while (false);

    return result;
}

void CRegisterForm::OnOk()
{
    std::string name, pass;
    std::tie(name, pass) = GetPassword();
    if (!name.empty() && !pass.empty())
    {
        CNetworkThread::_().CreateAccountWrapper(name, pass, [this]()
        {
            UIShow(false);
            AutoHideShowStatus(L"Create success");
            ::PostMessage(UIGetHWND(), Event_AccountBar_Login, 1, 0);
        });

        ShowStatus(L"Registering");
    }
}

void CRegisterForm::OnButtonClick(DWORD dwControlID)
{
    ControlID id = (ControlID)dwControlID;
    if (id == ControlID::BTN_CREATE_ACCOUNT)
    {
        OnOk();
    }
    else if (id == ControlID::BTN_LOGIN)
    {
        ::PostMessage(UIGetHWND(), WalletEvent::Event_Show_Login, 0, 0);
    }
}

void CRegisterForm::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    CBaseForm::OnUIPaint(dc, dwStatus);
    auto btn_login = GetSub((DWORD)ControlID::BTN_LOGIN);
    auto login_rc = btn_login->UIGetRect();
    auto rc = UIGetClientRect();

    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    Gdiplus::Pen pen(Gdiplus::Color(255, 67, 67, 67), 2);
    pen.SetDashStyle(Gdiplus::DashStyleDash);

    auto y = login_rc.CenterPoint().y;
    Gdiplus::PointF s1(120, y);
    Gdiplus::PointF e1(120 + 90, y);

    Gdiplus::PointF s2(rc.Width() - 120 - 90, y);
    Gdiplus::PointF e2(rc.Width() - 120, y);

    g.DrawLine(&pen, s1, e1);
    g.DrawLine(&pen, s2, e2);

}

LRESULT CRegisterForm::OnReturn(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    OnOk();
    return S_OK;
}
