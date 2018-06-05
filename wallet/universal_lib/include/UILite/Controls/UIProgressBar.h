// Author: liujing
#pragma once
#include "../Core/Action/UIAction.h"
#include "../Core/UICoreDef.h"
#include "../Core/UIObject.h"
#include "../UICrack.h"
#include "UIControlDef.h"
#include "UIControls.h"


class CUIProgressBar: public CUIControl
{
public:

	explicit CUIProgressBar(DWORD dwStyle = UIStyle_Transparent) : m_iMin(0), m_iPositonForPaint(0),
		m_iMax(100), m_iProgress(0), m_iStep(10), m_fMoveOverlayProgress(0.0f)
	{
		UIModifyStyle(dwStyle, 0);
	}

	~CUIProgressBar() {}

	void SetRange(int iMin, int iMax);
	void SetStep(int iStep);
	UNIVERSAL_LIB_API void SetProgress(int iProgress);

	void GetRange(int& iMin, int& iMax) const;
	int GetProgress() const { return m_iProgress; }
	int GetPaintPosition() const { return m_iPositonForPaint; }
	int GetStep() const { return m_iStep; }
	void NextStep();
	float GetBlinkProgress() { return m_fMoveOverlayProgress; }

	UNIVERSAL_LIB_API void CalcBlinkRect(CRect& rcDest, CRect& rcSrc, int iFillWidth, int iImageWidth);

	//////////////////////////////////////////////////////////////////////////
	void OnUIShow(bool bShow);
	void OnUISize();
	void OnUIStyleChanged(DWORD dwOldStyle);
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	void UpdateProgress();
	void UpdateMoveOverlay();

	int m_iMin;
	int m_iMax;
	int m_iProgress;
	int m_iStep;

	int m_iPositonForPaint;

	float m_fMoveOverlayProgress;
};


//////////////////////////////////////////////////////////////////////////

inline void CUIProgressBar::SetRange(int iMin, int iMax)
{
	UIASSERT(iMax > iMin);
	if (m_iMax != iMax || m_iMin != iMin)
	{
		m_iMin = iMin;
		m_iMax = iMax;

		int iOldProgress = m_iProgress;
		m_iProgress = std::max(m_iProgress, m_iMin);
		m_iProgress = std::min(m_iProgress, m_iMax);

		if (iOldProgress != m_iProgress)
		{
			m_iPositonForPaint = m_iProgress * UIGetRect().Width() / (m_iMax - m_iMin);
			UIStopAction(SysAction_Progress);
		}

		UIRedraw();
	}
}

inline void CUIProgressBar::SetStep(int iStep)
{
	m_iStep = iStep;
}

inline void CUIProgressBar::NextStep()
{
	SetProgress(m_iProgress + m_iStep);
}

inline void CUIProgressBar::GetRange(int& iMin, int& iMax) const
{
	iMax = m_iMax;
	iMin = m_iMin;
}
