#pragma once
#include <memory>
#include <string>
#include "common_ui/CommonInclude.h"
#include "UILite/Controls/UIControls.h"
#include "UILite/Controls/UIEdit.h"
#include "UILite/UICrack.h"

namespace wallet_gui
{
    class CTransactionItem : public CUIControl
    {
    public:
        CTransactionItem(const std::wstring& from, const std::wstring& type, const std::wstring& to,
            const std::wstring& value, const std::wstring& time);

    protected:
        BEGIN_UI_EVENT_MAP(CTransactionItem)
            UI_EVENT_CREATE(OnUICreate)
            UI_EVENT_SIZE(OnUISize)
        END_UI_EVENT_MAP()

        void OnUICreate();
        void OnUISize();
        void Layout();

        virtual void SetEditProperty(std::unique_ptr<CUIEdit>& edit);

    private:
        std::unique_ptr<CUIEdit> from_;
        std::unique_ptr<CUIStatic> type_;
        std::unique_ptr<CUIEdit> to_;
        std::unique_ptr<CUIEdit> value_;
        std::unique_ptr<CUIEdit> time_;
    };
}
