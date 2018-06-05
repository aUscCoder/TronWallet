// Author: liujing
#pragma once
#include "../../../Universal.h"
#include "../../WTLWrapper.h"
#include "WindowAniHelper.h"


class CSizeToWnd : public CUILiteWindowImpl<CSizeToWnd>
{
public:

	explicit CSizeToWnd(CWindowAniHelper* pAniHelper) : m_pBitmap(NULL), m_bFade(false),
		m_pAniHelper(pAniHelper), m_pfnAniTrack(NULL), m_pfnAniEnd(NULL), m_lParam(NULL) {}
	~CSizeToWnd()
	{
		if (m_pBitmap)
			delete m_pBitmap;
	}

	BEGIN_MSG_MAP_EX(CSizeToWnd)
		MESSAGE_HANDLER_EX(WM_UILITE_LAYERED_PAINT, OnIgnore)
		CHAIN_MSG_MAP(CUILiteWindowImpl<CSizeToWnd>)
		MSG_WM_MOUSEACTIVATE(OnMouseActivate)
	END_MSG_MAP()

	LRESULT OnIgnore(UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }
	int OnMouseActivate(CWindow wndTopLevel, UINT nHitTest, UINT message) { return MA_NOACTIVATE; }
	void OnFinalMessage(HWND hWnd) { delete this; }

	UNIVERSAL_LIB_API void SetAniParam(HWND hAniWnd, const CRect& rcFrom, const CRect& rcTo, const CRect& rcDest, bool bFade, HBITMAP hBmpAni);
	UNIVERSAL_LIB_API void DoSizeToAnimation();

	void SetAniEnd(fnAniEnd pfnEnd, LPARAM lParam) { m_pfnAniEnd = pfnEnd; m_lParam = lParam; }
	void SetAniTrack(fnAniTrack pfnTrack) { m_pfnAniTrack = pfnTrack; }

private:

	void DuringAnimation(float fProgress, LPARAM lParam);
	void EndAnimation(float fProgress, LPARAM lParam);
	void UpdateThumbnail(int x, int y, int cx, int cy, BYTE iAlpha);

	CWindowAniHelper* m_pAniHelper;

	HWND m_hAniWnd;
	CRect m_rcFrom;
	CRect m_rcTo;
	CRect m_rcDest;
	Gdiplus::Bitmap* m_pBitmap;
	bool m_bFade;

	fnAniTrack m_pfnAniTrack; // 自定义窗口动画路径

	fnAniEnd m_pfnAniEnd;	// 动画结束后回调
	LPARAM	m_lParam;
};
