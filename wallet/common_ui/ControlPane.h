#pragma once
#include <memory>
#include "CommonInclude.h"
#include "UILite/Controls/UIControlEvent.h"
#include "UILite/Core/UIObject.h"
#include "UILite/Controls/UIControlDef.h"
#include "UILite/UICrack.h"
#include "CommonDef.h"


class CUIButton;
using ControlActionClick = std::function<LRESULT(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)>;
class CUIControlPane : public CUIObject
{
public:
    CUIControlPane();
    virtual ~CUIControlPane();
    void SetClickCallback(ControlActionClick callback);
    void SetMouseEnterCallback(ControlActionClick callback);
    void SetMouseLeaveCallback(ControlActionClick callback);
    void AddButton(CUIButton* btn);
    CUIButton* GetButton(DWORD_PTR dwControlID);
    void SetMargin(int x, int y) { margin_x_ = x; margin_y_ = y; }
    void SetUseLargestSize(bool use) { use_largest_size_ = use; }
    void SetBackgrounColor(COLORREF color) { background_color_ = color; }

protected:
    BEGIN_UI_EVENT_MAP(CUIControlPane)
        UI_EVENT_CREATE(OnUICreate)
        UI_EVENT_SIZE(OnUISize)
        UI_EVENT_PAINT(OnUIPaint)
        UI_COMMAND_CODE(UIBN_CLICK, OnUIBNClick)
        UI_COMMAND_CODE(UINM_MOUSEENTER, OnMouseEnter)
        UI_COMMAND_CODE(UINM_MOUSELEAVE, OnMouseLeave)
    END_UI_EVENT_MAP()

    virtual void OnUICreate();
    virtual void OnUISize();
    virtual void Layout();
    virtual void OnUIPaint(CDCHandle dc, DWORD dwStatus);
    virtual LRESULT OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
    virtual LRESULT OnMouseEnter(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
    virtual LRESULT OnMouseLeave(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
    SIZE GetSubLargestSize();

protected:
    int margin_x_ = -1;
    int margin_y_ = -1;
    bool use_largest_size_ = true;
    std::vector<CUIButton*> m_vecActionButtons;
    ControlActionClick click_callback_ = nullptr;
    ControlActionClick mouse_enter_callback_ = nullptr;
    ControlActionClick mouse_leave_callback_ = nullptr;
    COLORREF background_color_ = CommonUI::middle_black;
};