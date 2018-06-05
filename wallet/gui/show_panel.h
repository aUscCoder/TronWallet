#pragma once
#include "base_form.h"
#include "wallet_evnets.h"

namespace wallet_gui
{
    class CShowPanel : public CBaseForm
    {
    public:
        CShowPanel() : CBaseForm(L"") {}
        void SetContent(CUIObject* content_obj);

    protected:
        BEGIN_UI_EVENT_MAP(CShowPanel)
            UI_EVENT_HANDLER(Event_Sub_Form_Destoy, OnSubFormDestroy)
        END_UI_EVENT_MAP()
        
        void OnUIPaint(CDCHandle dc, DWORD dwStatus) override;
        void Layout() override;
        LRESULT OnSubFormDestroy(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

    private:
        CUIObject* content_ = nullptr;
    };
}
