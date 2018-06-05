#pragma once
#include "base_form.h"
#include "UILite/Controls/UILabel.h"
#include "UILite/Controls/UIEdit.h"

namespace wallet_gui
{
    class CReceiveForm : public CBaseForm
    {
    public:
        CReceiveForm();

    protected:
        BEGIN_UI_EVENT_MAP(CReceiveForm)
            UI_EVENT_SHOWUI(OnUIShow)
        END_UI_EVENT_MAP()

        void OnUICreate() override;
        void Layout() override;
        void OnUIPaint(CDCHandle dc, DWORD dwStatus) override;
        void OnUIShow(bool bShow);

    private:
        std::unique_ptr<CUIEdit> address_;
        std::string str_address_;
        CBitmap bmp_qrcode_;
    };
}

