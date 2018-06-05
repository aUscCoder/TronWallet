#pragma once
#include "CommonInclude.h"
#include "UILite/Controls/UIControlEvent.h"
#include "UILite/Controls/UIControlDef.h"
#include "UILite/Core/UIObject.h"
#include "UILite/Controls/UILabel.h"
#include "UILite/UICrack.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"

namespace protocol { class Transaction; }

namespace wallet_gui
{
    class CBaseForm : public CUIRichObject
    {
    public:
        CBaseForm(const std::wstring& title, bool full = false);
        int GetTitleHeight();
        CUIObject* GetSub(DWORD id);

        void SetTitle(const std::wstring& title, int height = CommonUI::kTitleHeight);
        void SetTailHeight(int height) { tail_height_ = height; }

    protected:
        void AutoHideShowStatus(const std::wstring& status);
        void ShowStatus(const std::wstring& status);
        void MessageBox(const std::wstring& msg);
        void OnTransactionCreateFailed(std::shared_ptr<protocol::Transaction> transaction);

        BEGIN_UI_EVENT_MAP(CBaseForm)
            UI_EVENT_CREATE(OnUICreate)
            UI_EVENT_SIZE(OnUISize)
            UI_EVENT_PAINT(OnUIPaint)
            UI_EVENT_DESTROY(OnUIDestroy)
            UI_COMMAND_CODE(UIBN_CLICK, OnUIBNClick)
            UI_COMMAND_CODE(UIEN_RETURN, OnReturn)
        END_UI_EVENT_MAP()

        virtual void OnUISize();
        virtual void OnUICreate();
        virtual void Layout();
        virtual void OnUIDestroy();
        virtual void OnUIPaint(CDCHandle dc, DWORD dwStatus);
        virtual void OnButtonClick(DWORD dwControlID);
        virtual void OnEditTextChange(DWORD dwControlID);
        virtual LRESULT OnReturn(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
        LRESULT OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
        void CreateSubItems();

    protected:
        bool full_ = false;
        int title_height_ = CommonUI::kTitleHeight;
        int tail_height_ = 0;
        std::unique_ptr<CUIStatic> title_;
        std::vector<CommonUI::RowInfo> item_infos_;
        std::vector<std::unique_ptr<CUIObject>> sub_items_;
    };
}
