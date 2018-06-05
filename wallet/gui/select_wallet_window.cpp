#include "select_wallet_window.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "main_window.h"
#include "cold_main_window.h"

enum class ControlID {
    BTN_LOAD = 4000,
};

using namespace wallet_gui;

CSelectWalletWindow::CSelectWalletWindow()
{
    hot_radio_.reset(CommonUI::CreateRadio(L"Hot wallet"));
    cold_radio_.reset(CommonUI::CreateRadio(L"Cold wallet"));
    load_.reset(CommonUI::CreateNormalButton(L"Load", (DWORD)ControlID::BTN_LOAD));
}

CSelectWalletWindow::~CSelectWalletWindow()
{
    hot_radio_->UIDestroy();
    cold_radio_->UIDestroy();
    load_->UIDestroy();
}

void CSelectWalletWindow::OnUICreate()
{
    UISetHandled(false);
    hot_radio_->UICreate(this);
    hot_radio_->SetCheck(true);
    cold_radio_->UICreate(this);
    load_->UICreate(this);
}

void CSelectWalletWindow::OnUISize()
{
    UISetHandled(false);
    CRect rc;
    GetClientRect(rc);
    int top = GetTitleBarHeight() + 10;
    top += 60;
    auto left = (rc.Width() - 120) / 2;
    hot_radio_->UISetPos(left, top, 120, 30);
    top += 20 + 10;
    cold_radio_->UISetPos(left, top, 120, 30);
    top += 60;
    left = (rc.Width() - 120) / 2;
    load_->UISetPos(left, top, 120, CommonUI::normal_btn_height);
}

void CSelectWalletWindow::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    CRect rc;
    GetClientRect(rc);
    dc.FillSolidRect(rc, CommonUI::deep_black);
    int top = GetTitleBarHeight() + 10;
    auto text_rc = CRect(0, top, rc.Width(), top + 30);
    static std::wstring text = L"Choose the wallet type you want to load:";
    auto clrOld = dc.SetTextColor(0xffffff);
    Universal::UI::Paint::DrawTextExW(dc, text.c_str(), text.length(), text_rc, DrawFormat_Center | DrawFormat_VCenter);
    dc.SetTextColor(clrOld);
}

LRESULT CSelectWalletWindow::OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    if (dwControlID == DWORD(ControlID::BTN_LOAD))
    {
        CUIBaseWindow* main_wnd = hot_radio_->GetCheck() ? (CUIBaseWindow*)CMainWindow::Get() : CColdWalletMainWindow::Get();
        main_wnd->Create(NULL, CRect(0, 0, 1200, 800),
            L"", WS_VISIBLE | WS_CLIPCHILDREN | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        main_wnd->CenterWindow();
        main_wnd->ShowWindow(SW_SHOW);
        DestroyWindow();
    }
    return S_OK;
}


