#include "show_panel.h"
#include "common_ui/UI.h"

namespace wallet_gui
{
    void CShowPanel::SetContent(CUIObject* content_obj)
    {
        if (content_)
            content_->UIShow(false);
        content_ = content_obj;
        Layout();
        content_->UIShow(true);
        UIRedraw();
    }

    void CShowPanel::Layout()
    {
        CBaseForm::Layout();
        if (content_)
        {
            CRect rc_client = UIGetClientRect();
            auto h_center = rc_client.Width() / 2;
            auto v_center = rc_client.Height() / 2;
            auto rc_content = content_->UIGetClientRect();
            content_->UISetPos(h_center - rc_content.Width() / 2, v_center - rc_content.Height() / 2, rc_content.Width(), rc_content.Height());
        }
    }

    void CShowPanel::OnUIPaint(CDCHandle dc, DWORD dwStatus)
    {
    }

    LRESULT CShowPanel::OnSubFormDestroy(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled)
    {
        auto destroy_form = (CUIObject*)wParam;
        if (content_ == destroy_form)
            content_ = nullptr;
        return S_OK;
    }
}
