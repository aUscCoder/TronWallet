#include "status_layer.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "UILite/Core/Painter.h"
#include "common_ui/SkinResource.h"

using namespace wallet_gui;

CStatusLayer::CStatusLayer()
{
    const void* data = nullptr;
    DWORD data_size = 0;
    std::tie(data, data_size) = CommonUI::UI::LoadDataFromRes(IDB_GIF_LOADING, L"GIF");
    if (data && data_size)
    {
        image_.reset(new CGifImage((LPBYTE)data, data_size));
        UISetTimer(timer_id_, 0);
    }
}

void CStatusLayer::Update(const std::wstring& status)
{
    no_gif_ = false;
    UIShow(true);
    status_ = status;
    UIRedraw();
}

void CStatusLayer::Show(const std::wstring& status)
{
    if (status.empty())
    {
        UIShow(false);
    }
    else
    {
        no_gif_ = true;
        UIShow(true);
        status_ = status;
        UIRedraw();
        UISetTimer(show_timer_id_, 1500);
    }
}

void CStatusLayer::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    auto rc = UIGetClientRect();
    Gdiplus::Graphics g(dc);
    Gdiplus::Color clr(200, 50, 50, 50);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.FillRectangle(&Gdiplus::SolidBrush(clr), Gdiplus::Rect(rc.left, rc.top,
        rc.Width(), rc.Height()));

    CRect text_rc;
    if (!no_gif_ && image_)
    {
        auto x = (rc.Width() - image_->GetWidth()) / 2;
        auto y = (rc.Height() - image_->GetHeight()) / 2;
        image_->Draw(dc, x, y, image_->GetWidth(), image_->GetHeight());
        text_rc = CRect(POINT{ 0, (LONG)(y + image_->GetHeight() + 20) }, SIZE{(LONG)rc.Width() , 30});
    }
    else
    {
        auto y = rc.Height() / 2;
        text_rc = CRect(POINT{ 0, y }, SIZE{ rc.Width() , 30 });
    }

    auto old_crl = dc.SetTextColor(0xffffff);
    auto old_font = dc.SelectFont(CommonUI::UI::GetBigFont());
    Universal::UI::Paint::DrawTextExW(dc, status_.c_str(), status_.length(), text_rc, DrawFormat_Center | DrawFormat_VCenter);
    dc.SetTextColor(old_crl);
    dc.SelectFont(old_font);
}

void CStatusLayer::OnUITimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    if (nIDEvent == timer_id_)
    {
        UIKillTimer(timer_id_);
        UISetTimer(timer_id_, image_->NextFrame());
        UIRedraw();
    }
    else if (nIDEvent == show_timer_id_)
    {
        UIKillTimer(show_timer_id_);
        UIShow(false);
    }
}
