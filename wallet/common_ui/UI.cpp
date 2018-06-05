#include "UI.h"
#include "wallet_rc.h"
#include "UILite/Controls/UIButton.h"
#include "UILite/Controls/UILabel.h"
#include "UILite/Core/Painter.h"
#include "UILite/Controls/UIProgressBar.h"
#include "UILite/Controls/UIComboBox.h"
#include "UILite/Core/Scrollbar/UIDefaultScrollbar.h"
#include "SkinDef.h"
#include "SkinImage.h"
#include "PlaceHolderEdit.h"

CUIObject* CommonUI::CreateControl(const ControlInfo& info)
{
    CUIRichObject* obj = nullptr;
    switch (info.type_)
    {
    case ControlType::NORMAL_BUTTON:
        obj = CommonUI::CreateNormalButton(info.text_.c_str(), info.id_);
        break;
    case ControlType::CANCEL_BUUTON:
        obj = CommonUI::CreateNormalButton(info.text_.c_str(), info.id_);
        break;
    case ControlType::EDIT:
        obj = CommonUI::CreateEdit(info.id_, info.text_, info.placeholder_);
        break;
    case ControlType::LABEL:
        obj = new CUIStatic(info.text_, false, true, true);
        break;
    case ControlType::TITLE_LEBEL:   
       {
           auto label = new CUIStatic(info.text_, false, false, true);
           label->UISetFont(CommonUI::UI::GetMiddleFont());
           obj = label;
       }
        break;
    case ControlType::STATIC_EDIT:
        obj = CommonUI::CreateStaticEdit(info.id_, info.text_);
        break;
    case ControlType::TEXT_COLOR_BUTTON:
        obj = CommonUI::CreateTextColorButton(info.text_, info.id_);
        break;
    case ControlType::HYPERLINK:
        obj = CommonUI::CreateHyperlink(info.text_);
        break;
    case ControlType::PROGRESSBAR:
        obj = CommonUI::CreateProgressBar();
        break;
    }
    if (obj)
    {
        obj->UISetSize(info.size_);
        obj->UISetTextColor(info.color_);
    }
    return obj;
}

class CUIDropdownBtnPainter : public IPainter
{
public:

    CUIDropdownBtnPainter(CUIComboBox* pCmb) : m_pCmb(pCmb)
    {
        m_bmp = Skin::GetSkinBmp(SkinDef::DropdownTriangle);
    }

    ~CUIDropdownBtnPainter()
    {
    }

    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha) override
    {
        CSize sz;
        m_bmp.GetSize(sz);
        int iWidth = sz.cx / 2;
        RECT rcImage = { rc.left + (rc.Width() - iWidth) / 2, rc.top + (rc.Height() - sz.cy) / 2,
            rc.left + (rc.Width() - iWidth) / 2 + iWidth, rc.top + (rc.Height() - sz.cy) / 2 + sz.cy
        };
        int iFrameIndex = m_pCmb->IsDropDownVisible() ? 1 : 0;
        Universal::UI::Paint::DrawImage(dc, rcImage, m_bmp, Draw9Frame(iFrameIndex, 2), DRAWOPT_CENTER_TO_DEST());
        return S_OK;
    }
    CBitmapHandle m_bmp;
    CUIComboBox* m_pCmb = nullptr;
};

class CUIRadioPainter : public IPainter
{
public:

    CUIRadioPainter()
    {
        m_bmp = Skin::GetSkinBmp(SkinDef::Radio);
    }

    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha) override
    {
        CUIRadio* pRadio = (CUIRadio*)pUI;
        int iIndex = (dwStatus & UIBTNStatus_Checked) == 0 ? 0 : 1;
        CSize sz;
        m_bmp.GetSize(sz);
        int iFrameWidth = sz.cx / 2;
        CRect rcBtn(rc.left, (rc.Height() - sz.cy) / 2, rc.left + iFrameWidth, (rc.Height() - sz.cy) / 2 + sz.cy);
        Universal::UI::Paint::DrawImage(dc, rcBtn, m_bmp, Draw9Frame(iIndex, 2), DRAWOPT_CENTER_TO_DEST());
        return S_OK;
    }
    CBitmapHandle m_bmp;
};

class CUIScrollBarPainter : public IPainter
{
public:

    CUIScrollBarPainter()
    {
    }
    ~CUIScrollBarPainter()
    {
    }

    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha) override
    {
        CUIStatsImageFrameInfo info(3, 0, 1, 2, 2, 2);
        auto d = Draw9Frame(info.GetValue(dwStatus), 3);
        HBITMAP hBmp = Skin::GetSkinBmp(SkinDef::VScrollBar);
        auto rc1 = d.ToRect(hBmp);
        Universal::UI::Paint::DrawImage9Grid(dc, rc, hBmp, d, Draw9VCenter(), DrawOption_Opaque);
        return S_OK;
    }
};

class CUIComboBoxBKPainter : public IPainter
{
public:
    CUIComboBoxBKPainter()
    {
    }

    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha) override
    {
        CSkinImage img(SkinDef::ComboBoxBG);
        CUIStatsImageFrameInfo info(5, 0, 1, 2, 3, 4);
        img.DrawGrid(dc, rc, info.GetValue(dwStatus), Draw9Center(), DrawOption_Opaque);
        return S_OK;
    }
};

class CUIButtonPainter : public IPainter
{
public:
    CUIButtonPainter(bool bHighlight, SkinDef def) : m_bHighlight(bHighlight), skindef_(def)
    {
    }

    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha) override
    {
        CSkinImage imgBK(skindef_);
        CUIStatsImageFrameInfo sInfo(imgBK.GetFrameCount(), 0, 1, 0, 1, 0);
        imgBK.DrawGrid(dc, rc, sInfo.GetValue(dwStatus), Draw9Center(), DrawOption_Opaque);
        CUIButton* pBtn = (CUIButton*)pUI;
        auto clrOld = dc.SetTextColor(CommonUI::normal_text_color);
        Universal::UI::Paint::DrawTextExW(dc, pBtn->UIGetText().c_str(), pBtn->UIGetText().length(), rc, DrawFormat_Center | DrawFormat_VCenter);
        dc.SetTextColor(clrOld);
        return S_OK;
    }

    bool m_bHighlight = false;
    SkinDef skindef_ = SkinDef::Count;
};


class CStaticPainter : public IPainter
{
public:
    CStaticPainter(SkinDef id) : id_(id)
    {
    }

    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha) override
    {
        CSkinImage imgBK(id_);
        CUIStatsImageFrameInfo sInfo(imgBK.GetFrameCount(), 0, 0, 0, 0, 0);
        imgBK.DrawGrid(dc, rc, sInfo.GetValue(dwStatus), Draw9Center(), DrawOption_Opaque);
        CUIStatic* label = (CUIStatic*)pUI;
        auto clrOld = dc.SetTextColor(CommonUI::normal_text_color);
        Universal::UI::Paint::DrawTextExW(dc, label->UIGetText().c_str(), label->UIGetText().length(), rc, DrawFormat_Center | DrawFormat_VCenter);
        dc.SetTextColor(clrOld);
        return S_OK;
    }

    SkinDef id_ = SkinDef::Count;
};

CUIStatic* CommonUI::CreateIndexStatic(const std::wstring& text)
{
    CUIStatic* label = new CUIStatic(text, true, false, true);
    label->UISetPainter(new CStaticPainter(SkinDef::Index), true);
    label->UISetBkPainter(new CNullPainter, true);
    return label;
}

CUISysLink* CommonUI::CreateHyperlink(const std::wstring& text, DWORD dwControlID)
{
    auto link = new CUISysLink(text, dwControlID);
    link->SetColor(0x000000, CommonUI::slected_text_color, CommonUI::slected_text_color);
    return link;
}

CUIButton* CommonUI::CreateNormalButton(const std::wstring& text, DWORD uControlID, bool bHightlight /* = false */)
{
    CUIButton* pBtn = new CUIButton(uControlID, text.c_str());
    pBtn->UISetPainter(new CUIButtonPainter(bHightlight, SkinDef::NormalBtn), true);
    pBtn->UISetBkPainter(new CNullPainter, true);
    return pBtn;
}

CUIButton* CommonUI::CreateCancelButton(const std::wstring& text, DWORD uControlID)
{
    CUIButton* pBtn = new CUIButton(uControlID, text.c_str());
    pBtn->UISetPainter(new CUIButtonPainter(false, SkinDef::CancelBtn), true);
    pBtn->UISetBkPainter(new CNullPainter, true);
    return pBtn;
}

class CBackgroundButtonPainter : public IPainter
{
public:
    CBackgroundButtonPainter(COLORREF normal, COLORREF selected, int align) : align_(align), normal_(normal), selected_(selected) {}
    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha) override
    {
        CUIStatsImageFrameInfo sInfo(0, 0, 1, 0, 1, 0);
        auto status = (UIButtonStatus)sInfo.GetValue(dwStatus);
        COLORREF clrOld = dc.SetTextColor(status ? selected_ : normal_);
        CUIButton* pBtn = (CUIButton*)pUI;
        DrawFormat f = DrawFormat_Center;
        if (align_ == ALIGNLEFT)
            f = DrawFormat_Default;
        else if (align_ == ALIGNRIGHT)
            f = DrawFormat_Right;

        Universal::UI::Paint::DrawTextExW(dc, pBtn->UIGetText().c_str(), pBtn->UIGetText().length(), rc, f | DrawFormat_VCenter);
        dc.SetTextColor(clrOld);
        return S_OK;
    }
    int align_ = ALIGNMIDDLE;
    COLORREF normal_;
    COLORREF selected_;
};

CUIButton* CommonUI::CreateTextColorButton(const std::wstring& text, DWORD uControlID,
    COLORREF normal /* = CommonUI::normal_text_color */, COLORREF select /* = CommonUI::slected_text_color */, int align)
{
    CUIButton* pBtn = new CUIButton(uControlID, text.c_str());
    pBtn->UISetPainter(new CBackgroundButtonPainter(normal, select, align), true);
    pBtn->UISetBkPainter(new CNullPainter, true);
    return pBtn;
}

CUIComboBox* CommonUI::CreateComboBox(DWORD dwID)
{
    CUIComboBox* pObj = new CUIComboBox(dwID, UICBStyle_ReadOnly | UIStyle_TabStop
        | UIStyle_Visible | UICBStyle_AutoHScroll | UICBStyle_DropDownList);
    pObj->SetDropDownHeight(200);
    pObj->SetBorder(UI_SCALE(CRect(5, 3, 5, 3)));
    pObj->UISetBkPainter(new CUIComboBoxBKPainter, true);
    pObj->SetButtonWidth(UI_SCALE(19));
    pObj->SetButtonPainter(new CUIDropdownBtnPainter(pObj), true);
    pObj->SetDropdownListTopMargin(0);
    pObj->SetDropdownShadow(Skin::GetSkinBmp(SkinDef::Shadow));
    pObj->UISetScrollbar(CommonUI::CreateVScrollBar(), true);
    return pObj;
}

CUIRadio* CommonUI::CreateRadio(LPCWSTR lpszText)
{
    CUIRadio* pRadio = new CUIRadio();
    pRadio->UISetText(lpszText);
    pRadio->SetPadding(CRect(8, 6, 6, 6));
    pRadio->UISetTextColor(0x939191);
    pRadio->UISetBkPainter(new CUIRadioPainter, true);
    return pRadio;
}

IUIScrollbar* CommonUI::CreateVScrollBar()
{
    CBitmap bmp = Universal::UI::Bitmap::LoadFromRes(NULL, IDB_PNG_VSCROLLBAR, L"PNG");
    CSize sz;
    bmp.GetSize(sz);
    CUIDefaultVScrollbar* pScrollBar = new CUIDefaultVScrollbar(false, sz.cx / 3, 30);
    ScrollbarPainterInfo pi = {
        NULL, false,
        NULL, false,
        new CNullPainter, true,
        new CNullPainter, true,
        new CUIScrollBarPainter(), true,
    };
    pScrollBar->UIAddOrRemoveStyle(true, UIScrollStyle_NoTriangle | UIStyle_Visible);
    pScrollBar->SetScrollPainter(&pi);
    return pScrollBar;
}

CUIEdit* CommonUI::CreateEdit(DWORD dwID, const std::wstring& text, const std::wstring& placeholder)
{
    CPlaceHolderEdit* pEdit = new CPlaceHolderEdit(dwID, text.c_str(), UIEditStyle_ClientEdge | UIEditStyle_VCenter | UIEditStyle_AutoHScroll);
    pEdit->SetPlaceholder(placeholder);
    return pEdit;
}

CUIEdit* CommonUI::CreateStaticEdit(DWORD dwID, const std::wstring& text /* = L"" */)
{
    CUIEdit* pEdit = new CUIEdit(dwID, text.c_str(), UIEditStyle_ClientEdge | UIEditStyle_VCenter | UIEditStyle_ReadOnly);
    pEdit->UISetBkPainter(new CNullPainter);
    return pEdit;
}

CUIProgressBar* CommonUI::CreateProgressBar(DWORD dwStyle, int iMin, int iMax)
{
    CUIProgressBar* p = new CUIProgressBar(dwStyle);
    p->SetRange(iMin, iMax);
    return p;
}

int CommonUI::OpenFileDlg(HWND hWnd, LPCWSTR lpszDefaultExt, LPCWSTR lpszFileName, DWORD dwFlags, LPCWSTR lpszFilter,
    const OpenFileCallback& ok, const OpenFileCallback& cancel)
{
    SetThreadUILanguage(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
    CMultiFileDialog dlg(lpszDefaultExt, lpszFileName, dwFlags, lpszFilter, hWnd);
    std::vector<std::string> pathes;
    int iRet = dlg.DoModal(hWnd);
    if (IDOK == iRet && ok)
    {
        CString dir;
        dlg.GetDirectory(dir);
        std::string std_dir(CW2A(dir.GetString()));
        std_dir += "\\";
        auto file_name = dlg.GetFirstFileName();
        pathes.push_back(std_dir + std::string(CW2A(file_name)));

        while (true)
        {
            auto file_name = dlg.GetNextFileName();
            if (file_name)
                pathes.push_back(std_dir + std::string(CW2A(file_name)));
            else
                break;
        }
        ok(pathes);
    }
    else if (IDCANCEL == iRet && cancel)
    {
        cancel(pathes);
    }
    return iRet;
}

int CommonUI::OpenSaveDlg(HWND hWnd, LPCWSTR lpszDefaultExt, LPCWSTR lpszFileName, DWORD dwFlags, LPCWSTR lpszFilter,
    const SaveFileCallback& ok, const SaveFileCallback& cancel)
{
    SetThreadUILanguage(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
    CFileDialog dlg(FALSE, lpszDefaultExt, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, lpszFilter, hWnd);
    int iRet = dlg.DoModal(hWnd);
    if (iRet == IDOK && ok)
    {
        ok(dlg.m_szFileName);
    }
    else if (IDCANCEL == iRet && cancel)
    {
        cancel(dlg.m_szFileName);
    }
    return iRet;
}


static void GetRoundRectPath(Gdiplus::GraphicsPath *pPath, Gdiplus::Rect r, int dia)
{
    if (dia > r.Width)    dia = r.Width;
    if (dia > r.Height)    dia = r.Height;

    Gdiplus::Rect rcCorner(r.X, r.Y, dia, dia);
    pPath->Reset();
    pPath->AddArc(rcCorner, 180, 90);
    rcCorner.X += (r.Width - dia - 1);
    pPath->AddArc(rcCorner, 270, 90);
    rcCorner.Y += (r.Height - dia - 1);
    pPath->AddArc(rcCorner, 0, 90);
    rcCorner.X -= (r.Width - dia - 1);
    pPath->AddArc(rcCorner, 90, 90);
    pPath->CloseFigure();
}

void CommonUI::Paint::DrawRoundRect(Gdiplus::Graphics* pGraphics, Gdiplus::Rect r, Gdiplus::Color color, int radius, int width)
{
    int dia = 2 * radius;
    Gdiplus::Pen pen(color, (Gdiplus::REAL)width);
    Gdiplus::GraphicsPath path;
    GetRoundRectPath(&path, r, dia);
    pGraphics->DrawPath(&pen, &path);
}

void CommonUI::Paint::FillRoundRect(Gdiplus::Graphics* pGraphics, Gdiplus::Rect r, Gdiplus::Color color, int radius)
{
    int dia = 2 * radius;
    Gdiplus::GraphicsPath path;
    GetRoundRectPath(&path, r, dia);
    pGraphics->FillPath(&Gdiplus::SolidBrush(color), &path);
}

static CFont g_defaultFont;
static CFont g_bigFont;
static CFont g_middleFont;
HFONT CommonUI::UI::GetDefaultFont()
{
    if (!g_defaultFont)
    {
        g_defaultFont = ::CreateFont(16, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvetica");
    }
    return g_defaultFont;
}

HFONT CommonUI::UI::GetMiddleFont()
{
    if (!g_middleFont)
    {
        g_middleFont = ::CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvetica");
    }
    return g_middleFont;
}

HFONT CommonUI::UI::GetBigFont()
{
    if (!g_bigFont)
    {
        g_bigFont = ::CreateFont(28, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Helvetica");
    }
    return g_bigFont;
}

std::tuple<const void*, DWORD> CommonUI::UI::LoadDataFromRes(UINT nResourceID, LPCWSTR pResourceType)
{
    HRSRC hResource = ::FindResource(NULL, MAKEINTRESOURCE(nResourceID), pResourceType);
    if (hResource)
    {
        DWORD dwResSize = ::SizeofResource(NULL, hResource);
        if (dwResSize)
        {
            const void* pResData = ::LockResource(LoadResource(NULL, hResource));
            if (pResData)
                return std::make_tuple(pResData, dwResSize);
        }
    }
    return std::make_tuple(nullptr, 0);
}

using namespace CommonUI;


LRESULT CListItemBKPainter::DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha)
{
    if (index_ % 2 == 0)
    {
        dc.FillSolidRect(rc, CommonUI::white);
    }
    else
    {
        dc.FillSolidRect(rc, RGB(242, 242, 242));
    }
    return S_OK;
}

void CCommonListViewItem::Push(std::shared_ptr<CUIObject> sub_item)
{
    sub_items_.push_back(sub_item);
}

std::shared_ptr<CUIObject> CCommonListViewItem::GetSub(int index)
{
    if (index >= 0 && index < sub_items_.size())
        return sub_items_[index];
    return nullptr;
}

void CCommonListViewItem::OnUICreate()
{
    for (auto s : sub_items_)
        s->UICreate(this);
}

void CCommonListViewItem::OnUISize()
{
    Layout();
}

void CCommonListViewItem::Layout()
{
    auto rc = UIGetClientRect();
    int margin_x = margin_x_ ? margin_x_ : CommonUI::kDefaultMargin;
    int left = CommonUI::kDefaultMargin;
    int average_width = 0;
    if (equally_)
        average_width = (rc.Width() - margin_x * (sub_items_.size() - 1) - 2 * left) / sub_items_.size();

    for (auto sub : sub_items_)
    {
        if (sub->UIIsVisible())
        {
            auto size = sub->UIGetSize();
            auto width = average_width == 0 ? size.cx : average_width;
            auto s = sub->UIMeasure();
            auto height = size.cy == 0 ? s.cy : size.cy;
            sub->UISetPos(left, (rc.Height() - height) / 2, width, height);
            left += width + margin_x;
        }
    }
}

LRESULT CCommonListViewItem::OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    if (click_callback_)
        click_callback_(dwControlID);

    return S_OK;
}

LRESULT CCommonListViewItem::OnEnTextChange(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    if (edit_textchange_callback_)
        edit_textchange_callback_(dwControlID);
    return S_OK;
}
