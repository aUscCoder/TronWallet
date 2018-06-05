// Author: chenjianjun
#pragma once
#include "Cached.h"


class CCacheDC: public CDC
{
public:
	explicit CCacheDC(bool bAutoSaveDC = false)
	{
		m_hDC = Universal::UI::Cache::GetDC();
		TuoAssert(::GetObjectType(m_hDC) == OBJ_MEMDC && L"拿到的DC就是坏的");
		if (bAutoSaveDC)
		{
			m_iSavedDC = SaveDC();

			// 居然遇到了 SaveDC 失败的情况（下载保存文件对话框），怀疑与多线程有关，先加个 assert，yangqi，20151223
#ifdef _DEBUG
			if (m_iSavedDC == 0)
			{
				int nErr = ::GetLastError();
				nErr;
				TuoAssert(false && "SaveDC() 失败了！");
			}
#endif
		}
#ifdef _DEBUG
		else
		{
			m_iOldBkMode = GetBkMode();
			m_hOldFont = GetCurrentFont();
			m_hOldBitmap = GetCurrentBitmap();
			m_hOldPen = GetCurrentPen();
			m_hOldBrush = GetCurrentBrush();
		}
#endif
	}

	~CCacheDC()
	{
		TuoAssert(::GetObjectType(m_hDC) == OBJ_MEMDC && L"卧槽, DC被Delete了");

		if (m_iSavedDC != 0)
		{
			RestoreDC(m_iSavedDC);
		}
#ifdef _DEBUG
		else
		{
			TuoAssert(GetBkMode() == m_iOldBkMode && L"用完BkMode后请还原BkMode?");
			TuoAssert(GetCurrentFont() == m_hOldFont && L"用完DC后请还原FONT?");
			TuoAssert(GetCurrentBitmap() == m_hOldBitmap && L"用完DC后请还原BITMAP?");
			TuoAssert(GetCurrentPen() == m_hOldPen && L"用完DC后请还原PEN?");
			TuoAssert(GetCurrentBrush() == m_hOldBrush && L"用完DC后请还原BURSH?");
		}
#endif
		Universal::UI::Cache::ReleaseDC(m_hDC);
		m_hDC = NULL;
	}

protected:
	bool IsDCSaved() const { return m_iSavedDC != 0; }

private:
	explicit CCacheDC(const CCacheDC&) = delete;
	explicit CCacheDC(const CDC&) = delete;
	explicit CCacheDC(const CDCHandle&) = delete;
	CCacheDC& operator = (const CCacheDC&) = delete;
	CCacheDC& operator = (const HBITMAP) = delete;
	void Attach(HDC) = delete;
	HDC Detach() = delete;

private:
	int m_iSavedDC = 0;

#ifdef _DEBUG
	int		m_iOldBkMode	= (int)INVALID_HANDLE_VALUE;
	HFONT	m_hOldFont		= (HFONT)INVALID_HANDLE_VALUE;
	HBRUSH	m_hOldBrush		= (HBRUSH)INVALID_HANDLE_VALUE;
	HPEN	m_hOldPen		= (HPEN)INVALID_HANDLE_VALUE;
	HBITMAP	m_hOldBitmap	= (HBITMAP)INVALID_HANDLE_VALUE;
#endif
};

class CBitmapDC: public CCacheDC
{
public:
	explicit CBitmapDC(HBITMAP hBitmap, bool bAutoSaveDC = false) : CCacheDC(bAutoSaveDC)
	{
		m_hOldBitmap = SelectBitmap(hBitmap);
	}

	~CBitmapDC()
	{
		if (!IsDCSaved())
			SelectBitmap(m_hOldBitmap);
	}

private:
	HBITMAP m_hOldBitmap = NULL;
};

class CFontDC: public CCacheDC
{
public:
	explicit CFontDC(HFONT hFont, bool bAutoSaveDC = false) : CCacheDC(bAutoSaveDC)
	{
		m_hOldFont = SelectFont(hFont);
	}

	~CFontDC()
	{
		if (!IsDCSaved())
			SelectFont(m_hOldFont);
	}

private:
	HFONT m_hOldFont = NULL;
};

class CSelectFont
{
public:
	CSelectFont(CDCHandle dc, HFONT hFont) : m_dc(dc)
	{
		m_hOldFont = m_dc.SelectFont(hFont);
	}

	~CSelectFont()
	{
		m_dc.SelectFont(m_hOldFont);
	}

private:
	CSelectFont& operator = (const CSelectFont& that) = delete;

private:
	CDCHandle m_dc;
	HFONT m_hOldFont = NULL;
};
