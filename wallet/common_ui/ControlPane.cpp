#include "ControlPane.h"
#include "UI.h"
#include "UILite/Controls/UIButton.h"
#include "base/logging.h"
#include "base/sys_string_conversions.h"

#define BUTTON_LEFT_MARGIN 10

CUIControlPane::CUIControlPane()
{
}

CUIControlPane::~CUIControlPane()
{
    for (auto& a : m_vecActionButtons)
        delete a;
}

void CUIControlPane::SetClickCallback(ControlActionClick callback)
{
    click_callback_ = callback;
}

void CUIControlPane::SetMouseEnterCallback(ControlActionClick callback)
{
    mouse_enter_callback_ = callback;
}

void CUIControlPane::SetMouseLeaveCallback(ControlActionClick callback)
{
    mouse_leave_callback_ = callback;
}

void CUIControlPane::AddButton(CUIButton* btn)
{
    m_vecActionButtons.emplace_back(btn);
    if (UIIsCreated())
    {
        btn->UICreate(this);
        Layout();
    }
}

CUIButton* CUIControlPane::GetButton(DWORD_PTR dwControlID)
{
    auto it = std::find_if(m_vecActionButtons.begin(), m_vecActionButtons.end(),
        [dwControlID](const CUIButton* btn) { return btn->GetControlID() == dwControlID; });
    return it == m_vecActionButtons.end() ? nullptr : *it;
}

void CUIControlPane::OnUICreate()
{
    UIModifyStyle(UIStyle_AutoScroll | UIStyle_VScroll, 0);
    UISetScrollbar(CommonUI::CreateVScrollBar(), true);
    for (auto pBtn : m_vecActionButtons)
    {
        pBtn->UICreate(this);
    }
    Layout();
}

void CUIControlPane::OnUISize()
{
    Layout();
}

LRESULT CUIControlPane::OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    if (click_callback_)
        return click_callback_(pSender, dwControlID, pNotifyHeader);
    return S_OK;
}

LRESULT CUIControlPane::OnMouseEnter(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    if (mouse_enter_callback_)
        return mouse_enter_callback_(pSender, dwControlID, pNotifyHeader);
    return S_OK;
}

LRESULT CUIControlPane::OnMouseLeave(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    if (mouse_leave_callback_)
        return mouse_leave_callback_(pSender, dwControlID, pNotifyHeader);
    return S_OK;
}

void CUIControlPane::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    CRect rc = UIGetClientRect();
    dc.FillSolidRect(rc, background_color_);
}

SIZE CUIControlPane::GetSubLargestSize()
{
    SIZE sz{ 0,  0 };
    for (const auto& btn : m_vecActionButtons)
    {
        auto sub_size = btn->UIMeasure();
        if (sub_size.cx > sz.cx)
            sz = sub_size;
    }
    return sz;
}

void CUIControlPane::Layout()
{
    int marginx = margin_x_ == -1 ? BUTTON_LEFT_MARGIN : margin_x_;
    int marginy = margin_y_ == -1 ? CommonUI::kButtonSpace : margin_y_;

    CRect rcClient = UIGetClientRect();
    int iTemp = marginx;
    int iTopTemp = 0;
 
    if (use_largest_size_)
    {
        auto sz = GetSubLargestSize();
        for (size_t i = 0; i < m_vecActionButtons.size(); ++i)
        {
            if (iTemp + sz.cx > rcClient.right - marginx && i != 0)
            {
                iTemp = marginx;
                iTopTemp += sz.cy + marginy;
            }

            m_vecActionButtons[i]->UISetPos(iTemp, iTopTemp, sz.cx, sz.cy);
            iTemp += sz.cx + marginy;
        }
    }
    else
    {
        for (size_t i = 0; i < m_vecActionButtons.size(); i++)
        {
            auto& btn = m_vecActionButtons[i];
            auto sz = btn->UIMeasure();

            if (iTemp + sz.cx > rcClient.right - marginx &&i != 0)
            {
                iTemp = marginx;
                iTopTemp = sz.cy + marginy;
            }
            m_vecActionButtons[i]->UISetPos(iTemp, iTopTemp, sz.cx, sz.cy);
            iTemp += sz.cx + marginx;
        }
    }
    
    UIUpdateScrollbar();
    UIRedraw();
}
