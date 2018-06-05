// Author: ChenJianjun
#pragma once
#include "../TextLayout.h"
#include "../UICrack.h"
#include "UIObject.h"


class CUITextLayout;
class CUIRichObject: public CUIObject, public CUITextLayoutContainter
{
public:

	UNIVERSAL_LIB_API CUIRichObject();
	UNIVERSAL_LIB_API ~CUIRichObject();

	LPCWSTR UIGetObjectID() const override { UICHECK_OBJECT_VALID(); return m_pszObjectID ? m_pszObjectID : L""; }

	// 设置窗口的字体, 同时触发一个onFontChange回调
	void UISetFont(HFONT hNewFont) { UIFireEvent(UIEvent_SetFont, (WPARAM)hNewFont); }

	// 设置窗口ID
	UNIVERSAL_LIB_API void UISetObjectID(const std::wstring& strID);

	// 设置文字颜色
	void UISetTextColor(COLORREF clrText) { UICHECK_OBJECT_VALID(); UIFireEvent(UIEvent_SetTextColor, (WPARAM)clrText); }

	// 设置Disable文字颜色
	void UISetDisabledTextColr(COLORREF clrDisabled) { UICHECK_OBJECT_VALID(); UIFireEvent(UIEvent_SetDisabledTextColor, (WPARAM)clrDisabled); }

	// 设置窗口文字, 这会触发OnUITextChange回调
	void UISetText(const std::wstring& strText);

	void UISetCursor(HCURSOR hCursor) { m_hCursor = hCursor; }

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	HFONT OnUIGetFont();
	void OnUIGetText(std::wstring& strTextOut);
	COLORREF OnUIGetTextColor();
	COLORREF OnUIGetDisabledTextColor();
	int OnUIGetTextLength() { return m_uUITextLen; }
	void OnUISetText(const std::wstring& strText);
	void OnUISetTextColor(COLORREF clrText);
	void OnUISetDisabledTextColor(COLORREF clrText);
	void OnUISetFont(HFONT hNewFont);
	bool OnUISetCursor(UINCHitTestCode ht, int iMouseEvent);

protected:

	// 窗口文字, 像BUTTON,STATIC这些就是用的这个Text, 默认是NULL
	wchar_t* m_pszUIText;

	// 窗口文字的长度
	UINT m_uUITextLen;

	// 窗口文字的颜色
	COLORREF m_clrTextColor;

	// Disable文字颜色
	COLORREF m_clrDisabledTextColor;

	// 窗口用这个字体来画文字
	HFONT m_hFont;

	// 窗口ID
	wchar_t* m_pszObjectID;

	// 窗口光标
	HCURSOR m_hCursor;
};

//////////////////////////////////////////////////////////////////////////

inline void CUIRichObject::UISetText(const std::wstring& strText)
{
	UIFireEvent(UIEvent_SetText, (WPARAM)&strText);
}
