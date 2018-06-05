#pragma once
#include "CommonInclude.h"
#include "UILite/Controls/UIControlEvent.h"
#include "UILite/Controls/UIControlDef.h"
#include "UILite/WTLWrapper.h"
#include "UILite/UICrack.h"
#include "common_ui/GifImage.h"

namespace wallet_gui
{
    class CStatusLayer : public CUIObject
    {
    public:
        CStatusLayer();
        void Update(const std::wstring& status);
        void Show(const std::wstring& status);

    protected:
        BEGIN_UI_EVENT_MAP(CStatusLayer)
            UI_EVENT_PAINT(OnUIPaint)
            UI_EVENT_TIMER(OnUITimer)
        END_UI_EVENT_MAP()

        void OnUICreate();
        void OnUIPaint(CDCHandle dc, DWORD dwStatus);
        void OnUITimer(UINT_PTR nIDEvent, LPARAM lParam);

    private:
        bool no_gif_ = false;
        std::wstring status_;
        const UINT timer_id_ = 1111;
        const UINT show_timer_id_ = 1112;
        std::unique_ptr<CGifImage> image_;
    };
}

