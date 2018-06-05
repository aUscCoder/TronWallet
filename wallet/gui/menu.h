#pragma once
#include <memory>
#include "common_ui/ControlPane.h"
#include "UILite/Core/UICoreDef.h"
#include "UILite/Controls/UIEdit.h"

namespace wallet_gui
{
    class CMainWindow;

    class CMenu : public CUIControlPane, public IHookMessage
    {
    public:
        CMenu(CMainWindow* main_wnd, const std::vector<std::pair<std::wstring, DWORD>>& info);
        ~CMenu();
        bool OnHookMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) override;
        void SetHeader(CUIObject* header) { header_ = header; }

    protected:
        
        void OnUICreate() override;
        void Layout() override;
        LRESULT OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader) override;

    private:
        CUIObject* header_ = nullptr;
        CMainWindow* main_wnd_ = nullptr;
        std::vector<std::pair<std::wstring, DWORD>> infos_;
        std::vector<std::unique_ptr<CUIImage>> seperators_;
    };
}

