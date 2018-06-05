// Author: linjian
#pragma once
#include "../Universal.h"


// 绘制多行不同字体文本
/* Demo:
CUITextLayout t;

t << CTextLayoutString(L"文本 words").LineHeight(50) 
	<< CTextLayoutFont(hFontBig)
		<<  CTextLayoutString(L"ccc").TextColor(RGB(0, 0, 255))
	<< ENDFont() 
	<< CTextLayoutString(L"aa").Algin(TLA_RIGHT)
	<< CTextLayoutString(L"vv").Algin(TLA_LEFT)
	<< CUITextLayoutLink(L"aa").ID(1);

t.DrawLayoutText(dc, rc);

*/

// 元素类型
enum UILayoutItemType
{
	LIT_UNKNOWN = 0,
	LIT_FONT,			// 字体
	LIT_TEXT,			// 文本
	LIT_LINK,			// 链接
};

enum TextFormatProperty
{
	TextFormat_Id,							// id
	TextFormat_LinkStyle,					// link类型
	TextFormat_Font,						// 字体
	TextFormat_TextBKColor,					// 文字背景颜色
	TextFormat_TextColor,					// 文字颜色
	TextFormat_TextColor_Hover,				// hover的时候的文字颜色
	TextFormat_TextColor_Press,				// 鼠标左键按下时的文字颜色
};

enum TextFormatLinkStyle
{
	TextFormatLinkStyle_NoUnderline,		// 正常情况下没有下划线
	TextFormatLinkStyle_AlwaysUnderline,	// 总是显示下划线（暂未实现）
	TextFormatLinkStyle_HoverUnderline,		// hover时显示下划线（暂未实现）
};

enum TextLayoutAlign
{
	TLA_TOP,								// 上对齐
	TLA_BOTTOM,								// 下对齐
	TLA_CENTER,								// 居中
};

class CUITextBlock;
#define DECLARE_TEXTFORMAT_PROP(type, name, propertyIndex) \
public: \
	void Set##name(type t) { m_##name = t; m_setProperties.insert(propertyIndex); } \
	type Get##name() { return m_##name; } \
private: \
	type m_##name;

class CUITextFormat
{
public:

	UNIVERSAL_LIB_API CUITextFormat();
	CUITextFormat(CUITextFormat& format);
	~CUITextFormat() {}

	bool HasProperty(TextFormatProperty eProperty)
	{
		return m_setProperties.find(eProperty) != m_setProperties.end();
	}

	DECLARE_TEXTFORMAT_PROP(int, Id, TextFormat_Id)
	DECLARE_TEXTFORMAT_PROP(TextFormatLinkStyle, LinkStyle, TextFormat_LinkStyle)
	DECLARE_TEXTFORMAT_PROP(HFONT, Font, TextFormat_Font)
	DECLARE_TEXTFORMAT_PROP(COLORREF, TextBKColor, TextFormat_TextBKColor)
	DECLARE_TEXTFORMAT_PROP(COLORREF, TextColor, TextFormat_TextColor)
	DECLARE_TEXTFORMAT_PROP(COLORREF, TextColorHover, TextFormat_TextColor_Hover)
	DECLARE_TEXTFORMAT_PROP(COLORREF, TextColorPress, TextFormat_TextColor_Press)

private:

	std::set<TextFormatProperty> m_setProperties;
};

class CUIObject;
class CUITextBlock
{
public:

	UNIVERSAL_LIB_API CUITextBlock(CUITextBlock* pParent = 0);
	explicit CUITextBlock(LPCWSTR lpszText, CUITextBlock* pParent = 0, CUITextFormat* pFormat = NULL)
		: m_pParent(pParent), m_bDirty(false), m_strText(lpszText ? lpszText : L"")
	{
		if (pFormat)
		{
			m_format = *pFormat;
			m_bDirty = true;
		}
	}

	virtual ~CUITextBlock() { RemoveAllChild(); }

	UNIVERSAL_LIB_API CUITextBlock& operator << (CUITextBlock& block);
	UNIVERSAL_LIB_API CUITextBlock& operator = (CUITextBlock& block);

	CUITextFormat* GetTextFormat() { return &m_format; }
	void	SetTextFormat(const CUITextFormat* pFormat) { m_format = *pFormat; m_bDirty = true; }


	void	SetText(const std::wstring& wstr) { m_strText = wstr; m_bDirty = true; }
	const std::wstring& GetText() { return m_strText; }

	void	SetRect(CRect rc) { m_rc = rc; }
	CRect	GetRect() const { return m_rc; }

	CSize	GetSizePrefer(CDCHandle dc);		// measure出来这个TextBlock的宽高

	UNIVERSAL_LIB_API void	RemoveAllChild();
	std::vector<CUITextBlock*>& GetChildren() { return m_vecChild; }
	CUITextBlock* GetPropertyValidBlock(TextFormatProperty eProperty);	// 自己没有的时候，会去取父亲的属性

protected:

	void			InsertChildInternal(CUITextBlock* pChild);	// 内部调用
	void			InsertTextBlock(CUITextBlock* pChildBlock);
	void			SetParent(CUITextBlock* pParent);
	void			TakeOut(CUITextBlock* pChild);				// 把pChild从孩子中移除，但并不释放
	CRect			m_rc;
	bool			m_bDirty;					// 数据不对，需要重新布局
	std::wstring	m_strText;
	CUITextFormat	m_format;
	CUITextBlock*	m_pParent;
	std::vector<CUITextBlock*> m_vecChild;
};

class CUITextLayout : public CUITextBlock
{
public:

	explicit CUITextLayout(CUIObject* pObject = 0) : m_eAlign(TLA_TOP), m_bCapture(false),
		m_pBlockHovered(NULL), m_pBlockPressed(NULL), m_pObject(pObject) {}
	~CUITextLayout() {}

	UNIVERSAL_LIB_API void DrawText(CDCHandle dc, const CRect& rc, UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_BOTTOM);
	
	void			SetObject(CUIObject* pObject) { m_pObject = pObject; }
	void			SetAlign(TextLayoutAlign eAlign) { m_eAlign = eAlign; }
	CUITextBlock*	HitTest(CPoint pt);
	std::wstring	GetPlainText();

	// 需要在使用hover等颜色时，调用一下方法

	void			OnMouseLeave();
	void			OnMouseMove(CPoint pt);
	void			OnLButtonDown(CPoint pt);
	void			OnLButtonUp(CPoint pt);
	void			OnCaptureChanged();

private:

	void			Relayout(CDCHandle dc);
	void			UpdateChildOrder();					// 从左到又排列起来的所有的孩子
	COLORREF		GetTextColor(CUITextBlock* pBlock, COLORREF clrDefault);

	void			SetContext(CDCHandle dc, CUITextBlock* pBlock);


	CUIObject*		m_pObject;
	TextLayoutAlign m_eAlign;
	std::vector<CUITextBlock*> m_vecChildOrder;	// 重新排好序的（深度优先的顺序）
	bool			m_bCapture;
	CUITextBlock*	m_pBlockHovered;
	CUITextBlock*	m_pBlockPressed;
};

class CUITextLayoutContainter
{
public:

	explicit CUITextLayoutContainter(CUIObject* pObj) : m_pTextLayout(NULL), m_bTextLayoutManage(false), m_pObj(pObj) {}
	virtual ~CUITextLayoutContainter();

	UNIVERSAL_LIB_API virtual void SetTextLayout(CUITextLayout* pTextLayout, bool bManaged);
	UNIVERSAL_LIB_API virtual void RemoveTextLayout(bool bKeepText);
	CUITextLayout* GetTextLayout() { return m_pTextLayout; }

protected:

	void DrawTextInternal(CDCHandle dc, CRect rc, LPCWSTR lpszText, DWORD dwFormat);

	CUIObject*		m_pObj;
	CUITextLayout*	m_pTextLayout;
	bool			m_bTextLayoutManage;
};