#pragma once
#include "CommonInclude.h"
#include "UILite/UICrack.h"
#include <gdiplus.h>
#include "CommonDef.h"
#include "UILite/Core/UIObject.h"
#include "UILite/Controls/UIControlEvent.h"
#include "UILite/Controls/UIControlDef.h"
#include "UILite/Controls/UIButton.h"

class CUIButton;
class CUIComboBox;
class CUICheckBox;
class CUIRadio;
class CUIEdit;
class CUIStatic;
class CPlaceHolderEdit;
class CUISysLink;
class CUIProgressBar;
__interface IUIScrollbar;

namespace CommonUI
{
    enum class ControlType
    {
        EDIT = 0,
        STATIC_EDIT,
        NORMAL_BUTTON,
        CANCEL_BUUTON,
        TEXT_COLOR_BUTTON,
        LABEL,
        TITLE_LEBEL,
        HYPERLINK,
        PROGRESSBAR,
    };

    CUIComboBox*    CreateComboBox(DWORD dwID);
    CUIRadio*        CreateRadio(LPCWSTR lpszText);
    IUIScrollbar*    CreateVScrollBar();
    CUIEdit*  CreateEdit(DWORD dwID, const std::wstring& text = L"", const std::wstring& placeholder = L"");
    CUIEdit* CreateStaticEdit(DWORD dwID, const std::wstring& text = L"");

    CUIProgressBar*	CreateProgressBar(DWORD dwStyle = UIPBStyle_MoveOverlay | UIStyle_Transparent, int iMin = 0, int iMax = 100);

    CUIButton* CreateNormalButton(const std::wstring& text, DWORD uControlID, bool bHightlight = false);
    CUIButton* CreateTextColorButton(const std::wstring& text, DWORD uControlID,
        COLORREF normal = CommonUI::normal_text_color, COLORREF select = CommonUI::slected_text_color, int align = ALIGNMIDDLE);
    CUIButton* CreateCancelButton(const std::wstring& text, DWORD uControlID);

    CUIStatic* CreateIndexStatic(const std::wstring& text);

    CUISysLink* CreateHyperlink(const std::wstring& text, DWORD dwControlID = 0);

    using OpenFileCallback = std::function<void(const std::vector<std::string>& pathes)>;
    int OpenFileDlg(HWND hWnd, LPCWSTR lpszDefaultExt, LPCWSTR lpszFileName, DWORD dwFlags, LPCWSTR lpszFilter,
        const OpenFileCallback& ok, const OpenFileCallback& cancel);

    using SaveFileCallback = std::function<void(const std::wstring& path)>;
    int OpenSaveDlg(HWND hWnd, LPCWSTR lpszDefaultExt, LPCWSTR lpszFileName, DWORD dwFlags, LPCWSTR lpszFilter,
        const SaveFileCallback& ok, const SaveFileCallback& onCancel);

    namespace Paint
    {
        void DrawRoundRect(Gdiplus::Graphics* pGraphics, Gdiplus::Rect r, Gdiplus::Color color, int radius, int width);
        void FillRoundRect(Gdiplus::Graphics* pGraphics, Gdiplus::Rect r, Gdiplus::Color color, int radius);
    }

    namespace UI
    {
        HFONT GetDefaultFont();
        HFONT GetBigFont();
        HFONT GetMiddleFont();
        std::tuple<const void*, DWORD> LoadDataFromRes(UINT nResourceID, LPCWSTR pResourceType);
    }


    struct ControlInfo
    {
        ControlInfo(ControlType type, const std::wstring text = L"", DWORD id = 0, const std::wstring& placeholder = L"", SIZE size = {0, 0}, COLORREF color = 0x000000)
            : type_(type), text_(text), id_(id), placeholder_(placeholder), size_(size), color_(color) {}
        ControlType type_;
        std::wstring text_ = L"";
        std::wstring placeholder_ = L"";
        SIZE size_;
        DWORD id_ = 0;
        COLORREF color_ =  0x000000;
    };

    struct RowInfo
    {
        RowInfo(const std::vector<ControlInfo>& infos, SIZE margin = { 0, 0 }, SIZE size = {0 , 0}, bool average = true) 
            : average_(average), control_infos(infos), magin_(margin), size_(size) {}
        bool average_ = false;
        std::vector<ControlInfo> control_infos;
        SIZE magin_ = { 0, 0 };
        SIZE size_ = { 0, 0 };
       
    };

    CUIObject* CreateControl(const ControlInfo& info);

    class CListItemBKPainter : public IPainter
    {
    public:
        CListItemBKPainter(int index) : index_(index) { }
        LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha) override;

    private:
        int index_ = 0;
    };


    class CCommonListViewItem : public CUIObject
    {
    public:
        void Push(std::shared_ptr<CUIObject> sub_item);
        std::shared_ptr<CUIObject> GetSub(int index);
        void SetEqually(bool equally) { equally_ = equally; }
        void SetXMagin(int x) { margin_x_ = x; }

        const std::vector<std::shared_ptr<CUIObject>>& SubItems() { return sub_items_; }
        void SetClickCallback(std::function<void(DWORD dwControlID)> callback) { click_callback_ = callback; }
        void SetEditTextChangeCallback(std::function<void(DWORD dwControlID)> callback) { edit_textchange_callback_ = callback; }

    protected:
        BEGIN_UI_EVENT_MAP(CCommonListViewItem)
            UI_EVENT_CREATE(OnUICreate)
            UI_EVENT_SIZE(OnUISize)
            UI_COMMAND_CODE(UIBN_CLICK, OnUIBNClick)
            UI_COMMAND_CODE(UIEN_TEXTCHANGE, OnEnTextChange)
            UI_COMMAND_CODE(UISLN_CLICK, OnUIBNClick)
        END_UI_EVENT_MAP()

        void OnUICreate();
        void OnUISize();
        void Layout();

        LRESULT OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
        LRESULT OnEnTextChange(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

    private:
        bool equally_ = false;
        int margin_x_ = 0;
        std::vector<std::shared_ptr<CUIObject>> sub_items_;
        std::function<void(DWORD dwControlID)> click_callback_;
        std::function<void(DWORD dwControlID)> edit_textchange_callback_;
    };


}