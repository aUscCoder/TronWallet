// Author: ChenJianjun
#pragma once


class CUIObject;
class IRuler
{
public:
	IRuler() {}
	virtual ~IRuler() {}
	virtual const SIZE DoMeasure(CUIObject* pUI) = 0;
};

class CFixedRuler: public IRuler
{
public:

	CFixedRuler(int iWidth, int iHeight) : m_iWidth(iWidth), m_iHeight(iHeight)
	{
	}

	~CFixedRuler() {}

	const SIZE DoMeasure(CUIObject* pUI) override
	{
		SIZE sz = { m_iWidth, m_iHeight };
		return sz;
	}

private:

	int m_iWidth;
	int m_iHeight;
};

class CTextRuler: public IRuler
{
public:
	explicit CTextRuler(const std::wstring& strText) : m_strText(strText) {}
	~CTextRuler() {}

	UNIVERSAL_LIB_API const SIZE DoMeasure(CUIObject* pUI) override;

private:

	std::wstring m_strText;
};

class CUITextRuler: public IRuler
{
public:

	explicit CUITextRuler(int iMarginLeft = 0, int iMarginTop = 0, int iMarginRight = 0, int iMarginBottom = 0) : m_iLeft(iMarginLeft),
		m_iTop(iMarginTop), m_iRight(iMarginRight), m_iBottom(iMarginBottom)
	{
	}

	~CUITextRuler() {}

	UNIVERSAL_LIB_API const SIZE DoMeasure(CUIObject* pUI) override;

private:

	int m_iLeft;
	int m_iTop;
	int m_iRight;
	int m_iBottom;
};

