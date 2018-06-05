// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "../Core/UIRichObject.h"
#include "../UICrack.h"
#include "UIControlEvent.h"


class CUISep: public CUIObject
{
public:

	explicit CUISep(bool bHorz = true, COLORREF clr = ::GetSysColor(COLOR_GRAYTEXT)) : m_clr(clr),
		m_bHorz(bHorz)
	{
		UIModifyStyle(UIStyle_Transparent, 0);
	}

	virtual ~CUISep() {};

	void SetColor(COLORREF clr) { m_clr = clr; }

	BEGIN_UI_EVENT_MAP(CUISep)
		UI_EVENT_PAINT(OnPaint)
		UI_EVENT_MEASURECLIENT(OnMeasureClient)
	END_UI_EVENT_MAP()

	void OnPaint(CDCHandle dc, DWORD dwStatus)
	{
		CRect rc = UIGetClientRect();
		Universal::UI::Paint::FillSolidRect(dc, rc, m_clr);
	}

	bool OnMeasureClient(SIZE& sz)
	{
		sz.cx = sz.cy = 1;
		return true;
	}

private:

	COLORREF m_clr;
	bool m_bHorz;
};

// 图标
class CUIImage: public CUIObject
{
public:

	CUIImage() : m_hIcon(NULL) { UIModifyStyle(UIStyle_Transparent, 0); }
	~CUIImage() {}

	void SetIcon(HICON hIcon, SIZE sz = CSize(16, 16))
	{
		m_bImageTypeIcon = true;
		m_hIcon = hIcon;
		m_size = sz;
	}

	void SetBitmap(HBITMAP hBmp)
	{
		m_bImageTypeIcon = false;
		m_hBitmap = hBmp;
		m_hBitmap.GetSize(m_size);
	}

	SIZE GetSize() const { return m_size; }

	//////////////////////////////////////////////////////////////////////////
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	bool OnMeasureClient(SIZE& sz);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	HICON m_hIcon;
	CBitmapHandle m_hBitmap;
	CSize m_size;
	bool m_bImageTypeIcon: 1;
};

//////////////////////////////////////////////////////////////////////////
//
class CUIImageListPlayer: public CUIObject
{
public:

	UNIVERSAL_LIB_API explicit CUIImageListPlayer();
	UNIVERSAL_LIB_API ~CUIImageListPlayer();

	UNIVERSAL_LIB_API void SetBitmap(HBITMAP bmp, bool bManaged = false);
	void SetFrameCount(int iFrameCount) { m_iFrameCount = std::max(iFrameCount, 1); Play(); }
	int GetFrameCount() const { return m_iFrameCount; }
	void SetFrameDelay(int iFrameDelay) { m_iFrameDelay = iFrameDelay; Play(); }
	int GetFrameDelay() const { return m_iFrameDelay; }

	UNIVERSAL_LIB_API void Play();
	UNIVERSAL_LIB_API void Stop();

private:

	void OnUIShowUI(bool bShow);
	void OnUIPaint(CDCHandle dc, DWORD dwStatus);
	bool OnUIMeasureClient(SIZE& sz);
	void OnUIAction(float fProgress, LPARAM lParam);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	void _Release();

private:

	CBitmapHandle m_bmp;
	bool m_bManaged;
	int m_iFrameCount;
	int m_iFrameDelay;
	int m_iCurrentFrame;

};

//////////////////////////////////////////////////////////////////////////
// 静态文本
class CUIStatic: public CUIRichObject
{
public:

	UNIVERSAL_LIB_API explicit CUIStatic(const std::wstring& strText = L"", bool bHCenter = false, bool bMultiline = false, bool bVCenter = false,
										 int iMaxWidth = -1, int iLineDistance = 0);
	~CUIStatic() {}

	void SetFormat(int iMaxWidth = -1, int iLineDistance = 0);

	//////////////////////////////////////////////////////////////////////////
	void OnUITextChange();
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	bool OnMeasureClient(SIZE&);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	int m_iMaxWidth;
	int m_iLineDistance;
};

//////////////////////////////////////////////////////////////////////////
// 下面这些函数很小, 导出会比较亏, 直接inline吧

inline void CUIStatic::SetFormat(int iMaxWidth/* = -1 */, int iLineDistance/* = 0 */)
{
	m_iMaxWidth = iMaxWidth;
	m_iLineDistance = iLineDistance;
}