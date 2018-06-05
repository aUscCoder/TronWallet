#include "menu.h"
#include "main_window.h"
#include "common_ui/SkinDef.h"

namespace wallet_gui
{
    CMenu::CMenu(CMainWindow* main_wnd, const std::vector<std::pair<std::wstring, DWORD>>& info) :
        CUIControlPane(), main_wnd_(main_wnd), infos_(info)
    {
    }

    CMenu::~CMenu()
    {
        for (auto& s : seperators_)
            s->UIDestroy();
    }

    bool CMenu::OnHookMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
    {
        if (uMsg == WM_ACTIVATE && wParam == WA_INACTIVE)
        {
            if (UIGetMgr() && UIGetMgr()->IsUIValid())
                UIShow(false);
            return false;
        }

        if (UIGetMgr() && UIGetMgr()->IsUIValid() && UIGetHWND() == main_wnd_->m_hWnd && UIIsVisible())
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ::ClientToScreen(hWnd, &pt);
            auto rc = UIGetClientRect();
            auto rc_screen = UIClientToScreen(rc);

            if (!rc_screen.PtInRect(pt))
            {
                switch (uMsg)
                {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_LBUTTONDBLCLK:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                case WM_RBUTTONDBLCLK:
                    UIShow(false);
                    break;

                case WM_MOUSEMOVE:
                {
                    if (header_)
                    {
                        auto headr_rc = header_->UIGetClientRect();
                        auto headr_rc_screen = header_->UIClientToScreen(headr_rc);
                        CRect gap_rect(rc_screen.left, headr_rc_screen.bottom, rc_screen.right, rc_screen.top);
                        if (!headr_rc_screen.PtInRect(pt) && !gap_rect.PtInRect(pt))
                        {
                            UIShow(false);
                        }
                        break;
                    }
                }

                }
            }
        }
        return false;
    }

    void CMenu::OnUICreate()
    {
        for (size_t i = 0; i < infos_.size(); i++)
        {
            const auto& p = infos_[i];
            auto menu_item = CommonUI::CreateTextColorButton(p.first, p.second, RGB(105, 105, 105), 0x000000, ALIGNLEFT);
            AddButton(menu_item);
            if (i != infos_.size() - 1)
            {
                std::unique_ptr<CUIImage> seprator(new CUIImage);
                seprator->UICreate(this);
                seprator->SetBitmap(Skin::GetSkinBmp(SkinDef::Seperator));
                seperators_.push_back(std::move(seprator));
            }
        }

        auto largest = GetSubLargestSize();
        UIModifyStyle(0, UIStyle_VScroll | UIStyle_AutoScroll);
        SetMargin(10, 0);
        UISetSize({ largest.cx + margin_x_, (int)(largest.cy * infos_.size() + (infos_.size() - 1) * 2)});
        UIGetMgr()->AddMessageHook(this);
    }

    void CMenu::Layout()
    {
        auto rc = UIGetClientRect();
        auto largest = GetSubLargestSize();
        int top = 0;
        for (size_t i = 0; i < m_vecActionButtons.size(); i++)
        {
            m_vecActionButtons[i]->UISetPos(margin_x_, top, largest.cx, largest.cy);
            top += largest.cy;
            if (i != m_vecActionButtons.size() - 1)
            {
                seperators_[i]->UISetPos(0, top, rc.Width(), 2);
            }
        }
    }

    LRESULT CMenu::OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
    {
        UIShow(false);
        return CUIControlPane::OnUIBNClick(pSender, dwControlID, pNotifyHeader);
    }
}



