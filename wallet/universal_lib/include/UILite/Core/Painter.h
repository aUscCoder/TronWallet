/// Author: chenjianjun
#pragma once
#include "../../CommonLib/UI/DC.h"
#include "../../Universal.h"
#include "UIUtil.h"


class CUIObject;
class IPainter
{
public:

	IPainter() {}
	virtual ~IPainter() {}
	virtual LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha = 255) = 0; // NOLINT (4.5.28 这里需要默认参数)
	virtual IPainter* Clone() { UIASSERT(FALSE && "请实现这个接口"); return NULL; }
};

class CNullPainter: public IPainter
{
public:

	LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha = 255) { return 0; }
	IPainter* Clone() override { return new CNullPainter(); }
};

class CPainterGradentFillV: public IPainter
{
public:

	CPainterGradentFillV(const CValueFromStatus& color1, const CValueFromStatus& color2) : m_color1(color1),
		m_color2(color2), m_bDrawBroder(false)
	{
	}

	CPainterGradentFillV(const CValueFromStatus& color1, const CValueFromStatus& color2, const CValueFromStatus& colorBorder) : m_color1(color1),
		m_color2(color2), m_colorBorder(colorBorder), m_bDrawBroder(true)
	{
	}

	~CPainterGradentFillV() {}

	UNIVERSAL_LIB_API LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha /* = 255 */) override;
	UNIVERSAL_LIB_API IPainter* Clone() override;

private:

	CValueFromStatus m_color1;
	CValueFromStatus m_color2;
	CValueFromStatus m_colorBorder;
	bool m_bDrawBroder;
};

class CPainterGradentFillH : public IPainter
{

public:

	CPainterGradentFillH(const CValueFromStatus& color1, const CValueFromStatus& color2) : m_color1(color1),
		m_color2(color2), m_bDrawBroder(false)
	{
	}

	CPainterGradentFillH(const CValueFromStatus& color1, const CValueFromStatus& color2, const CValueFromStatus& colorBorder) : m_color1(color1),
		m_color2(color2), m_colorBorder(colorBorder), m_bDrawBroder(true)
	{
	}

	~CPainterGradentFillH() {}

	UNIVERSAL_LIB_API LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha /* = 255 */) override;
	UNIVERSAL_LIB_API IPainter* Clone() override;

private:

	CValueFromStatus m_color1;
	CValueFromStatus m_color2;
	CValueFromStatus m_colorBorder;
	bool m_bDrawBroder;
};

// 画自定义颜色
class CPainterColor: public IPainter
{
public:

	CPainterColor()
	{
		m_clr.rgbRed = 0;
		m_clr.rgbGreen = 0;
		m_clr.rgbBlue = 0;
		m_clr.rgbReserved = 0;
	}

	explicit CPainterColor(COLORREF clr)
	{
		RGBQuadFromColorRef(clr);
	}

	explicit CPainterColor(RGBQUAD clr) : m_clr(clr)
	{
		m_clr.rgbRed = m_clr.rgbRed * m_clr.rgbReserved / 255;
		m_clr.rgbGreen = m_clr.rgbGreen * m_clr.rgbReserved / 255;
		m_clr.rgbBlue = m_clr.rgbBlue * m_clr.rgbReserved / 255;
	}

	~CPainterColor()
	{

	}

	void SetColor(RGBQUAD clr)
	{
		m_clr = clr;

		m_clr.rgbRed = m_clr.rgbRed * m_clr.rgbReserved / 255;
		m_clr.rgbGreen = m_clr.rgbGreen * m_clr.rgbReserved / 255;
		m_clr.rgbBlue = m_clr.rgbBlue * m_clr.rgbReserved / 255;
	}

	UNIVERSAL_LIB_API LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha = 255) override;
	IPainter* Clone() override { return new CPainterColor(m_clr); }

protected:

	void RGBQuadFromColorRef(COLORREF clr)
	{
		m_clr.rgbRed = GetRValue(clr);
		m_clr.rgbGreen = GetGValue(clr);
		m_clr.rgbBlue = GetBValue(clr);
		m_clr.rgbReserved = 0xff;
	};

	RGBQUAD m_clr;
};

class CPainterStatsColor : public CPainterColor
{
public:
	explicit CPainterStatsColor(const CValueFromStatus& color) : m_color(color), CPainterColor(0) {}
	~CPainterStatsColor() {}

	LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha = 255) override
	{
		RGBQuadFromColorRef((COLORREF)m_color.GetValue(dwStatus));
		return CPainterColor::DoPaint(pUI, dc, rc, dwStatus, cAlpha);
	}

	IPainter* Clone() override { return new CPainterStatsColor(m_color); }

private:

	CValueFromStatus m_color;
};

class CPainterBlack: public CPainterColor
{
public:
	CPainterBlack() : CPainterColor(0) {}
	~CPainterBlack() {}
};

class CPainterWhite: public CPainterColor
{
public:
	CPainterWhite() : CPainterColor(0xFFFFFF) {}
	~CPainterWhite() {}
};


class CPainterImage: public IPainter
{
public:

	// 需要九宫格拉伸，需要给出九宫格拉伸需要的参数
	CPainterImage(HBITMAP hbm, const CUIStatsImageFrameInfo& sifi, const Draw9 &d, DrawOption opt = DrawOption_Opaque)
		: m_sifi(sifi), m_hbm(hbm), m_draw9(d), m_drawOpt(opt) { }
	// 直接贴原图，可能会被拉伸、压缩
	CPainterImage(HBITMAP hbm, const CUIStatsImageFrameInfo& sifi)
		: m_sifi(sifi), m_hbm(hbm), m_draw9(Draw9()), m_drawOpt(DRAWOPT_NO_CHECK()) { }
	~CPainterImage() {}

	LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha = 255) override
	{
		Universal::UI::Paint::DrawImage9Grid(dc, rc, m_hbm,
			m_sifi.ToFraw9Frame(dwStatus), m_draw9, m_drawOpt | DRAWOPT_ALPHA(cAlpha));
		
		return 0;
	}

	IPainter* Clone() override { return new CPainterImage(m_hbm, m_sifi); }

private:

	CBitmapHandle m_hbm;
	CUIStatsImageFrameInfo m_sifi;
	Draw9 m_draw9;
	DrawOption m_drawOpt;
};
