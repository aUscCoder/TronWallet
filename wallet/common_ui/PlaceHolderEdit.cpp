#include "PlaceHolderEdit.h"
#include "SkinDef.h"
#include "SkinImage.h"

#define PLACEHODLER_TEXTCOLOR	0xb3b3b3

class CPlaceHoderEditPainter : public IPainter
{
public:
    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStats, BYTE cAlpha = 255) override
    {
        if (!pUI->UITestStyle(UIEditStyle_ClientEdge))
            return 0;

        if (dwStats & UIStatus_HoverChild)
            dwStats |= UIStatus_Hover;

        if (dwStats & UIStatus_FocusChild)
            dwStats |= UIStatus_Focus;

        // CValueFromStatus::GetValue 的优先级是 Disabled、Pressed、Hover、Focus……
        // 而这里我们需要的优先级顺序是 Disabled、Focus、Hover（Pressed 没可能出来，因为一 Press 就 Focus 了）
        // 这里搞个小 Patch，如果发现是 Focus 状态，就手工去除掉 Hover 和 Pressed
        if ((dwStats & UIStatus_Focus) != 0)
            dwStats &= ~(UIStatus_Hover | UIStatus_Pressed);

        CValueFromStatus vfs(0, 1, 1, 1, 2);
        CSkinImage img(SkinDef::EditBG);
        img.DrawGrid(dc, rc, vfs.GetValue(dwStats), Draw9Center(), DrawOption_Opaque);

        auto edit = (CPlaceHolderEdit*)pUI;
        const auto& placeholder = edit->Placeholder();

        if (((dwStats & (UIStatus_Focus | UIStatus_FocusChild)) == 0)
            && edit->UIGetText() == L"" && placeholder.length() != 0)
        {
            COLORREF color = dc.SetTextColor(PLACEHODLER_TEXTCOLOR);
            CRect rcText = rc;
            rcText.left = 10;
            Universal::UI::Paint::DrawText(dc, placeholder.c_str(), -1, rcText, DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX, 180);
            dc.SetTextColor(color);
        }
        return 0;
    }

    IPainter* Clone() override { return new CPlaceHoderEditPainter(); }
};


CPlaceHolderEdit::CPlaceHolderEdit(DWORD dwID, const std::wstring& text, DWORD dwEditStyle) : CUIEdit(dwID, text.c_str(), dwEditStyle)
{
    UISetBkPainter(new CPlaceHoderEditPainter);
    UISetBorder(UI_SCALE(CRect(8, 5, 8, 5)));
}

void CPlaceHolderEdit::SetPlaceholder(const std::wstring& placeholder)
{
    placeholder_ = placeholder;
}

std::wstring CPlaceHolderEdit::Placeholder()
{
    return placeholder_;
}

