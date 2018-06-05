// Author: ChenJianjun
#pragma once


class CUIObject;
class CTooltipText
{
public:

	CTooltipText() {}
	virtual ~CTooltipText() {}

	virtual std::wstring GetText(CUIObject* pUI) = 0;
};

class CFixedTooltipText: public CTooltipText
{
public:

	CFixedTooltipText(const std::wstring& strText) : m_strText(strText) {} // NOLINT (3.1.3 可以传LPCWSTR)
	~CFixedTooltipText() {}

	std::wstring GetText(CUIObject* pUI) override { return m_strText; }

private:

	std::wstring m_strText;
};
