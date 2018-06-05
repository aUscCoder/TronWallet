// Author:	Yanghao
#pragma once
#include "../../Universal.h"
#include "../Core/UIRichObject.h"
#include "../UICrack.h"
#include "UIButton.h"
#include "UIControlEvent.h"
#include "UIControls.h"

struct UISliderPosContext
{
	UINH hdr;
	int iValue;
	CPoint point;
};

enum SliderDirection
{
	Slider_Horz,	// 水平
	Slider_Vert,	// 垂直
};

enum SliderTickPosition
{
	Slider_NoTicks = 0,			// 木有
	Slider_TicksAbove = 1,		// 单边上
	Slider_TicksLeft = Slider_TicksAbove,
	Slider_TicksBelow = 2,		// 单边下
	Slider_TicksRight = Slider_TicksBelow,
	Slider_TicksBothSides = 3	// 双边
};

class IPainterSlider;
class CUISliderBase;

class CUISliderThumb : public CUIButton
{
public:

	explicit CUISliderThumb() {}
	~CUISliderThumb() {}

	BEGIN_UI_EVENT_MAP(CUISliderThumb)
		UI_EVENT_MOUSEMOVE(OnUIMouseMove)
		UI_EVENT_MOUSEWHEEL(OnMouseWheel)
		UI_EVENT_KEYDOWN(OnKeyDown)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
	END_UI_EVENT_MAP()

private:

	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnUIMouseMove(UINT nFlags, POINT point);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnMouseWheel(UINT nFlags, short zDelta, POINT point);
};

class CUISliderProgress : public CUIControl
{
public:

	explicit CUISliderProgress() {}
	~CUISliderProgress() {}

	BEGIN_UI_EVENT_MAP(CUISliderProgress)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
		UI_EVENT_LBUTTONDOWN(OnUILButtonDown)
		UI_EVENT_MOUSEWHEEL(OnMouseWheel)
		UI_EVENT_KEYDOWN(OnKeyDown)
	END_UI_EVENT_MAP()

private:

	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnUILButtonDown(UINT nFlags, POINT point);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	LRESULT OnMouseWheel(UINT nFlags, short zDelta, POINT point);

};

class CUISliderTick : public CUIControl
{
public:
	
	explicit CUISliderTick() {}
	~CUISliderTick() {}

	BEGIN_UI_EVENT_MAP(CUISliderTick)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
		UI_EVENT_LBUTTONDOWN(OnUILButtonDown)
		UI_EVENT_KEYDOWN(OnKeyDown)
	END_UI_EVENT_MAP()

private:

	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnUILButtonDown(UINT nFlags, POINT point);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

};

class CUISliderBase : public CUIControl
{
public:
	
	UNIVERSAL_LIB_API explicit CUISliderBase(const SliderDirection eDirection = Slider_Horz);
	UNIVERSAL_LIB_API virtual ~CUISliderBase(void);

	UNIVERSAL_LIB_API int GetLogicZeroXBegin();
	UNIVERSAL_LIB_API int GetLogicZeroXEnd();
	UNIVERSAL_LIB_API int GetLogicLength();

	// 设置Padding
	void SetPadding(const CRect& rcPadding);
	void SetPadding(int iLeft = 1, int iTop = 1, int iRight =1, int iBottom = 1);
	
	// 滑块大小
	void SetThumbSize(const SIZE& szThumb);
	SIZE GetThumbSize() const;

	// 进度（百分比）
	UNIVERSAL_LIB_API void SetProgress(const int iProgress);
	UNIVERSAL_LIB_API int GetProgress();

	// 指示器对应的当前数值（实刻度值）
	UNIVERSAL_LIB_API void SetValue(const int iValue);
	UNIVERSAL_LIB_API int GetValue() const;

	// 步进
	UNIVERSAL_LIB_API void GoBackwordStep();
	UNIVERSAL_LIB_API void GoForwordStep();

	// 指示器位置
	UNIVERSAL_LIB_API void SetTickPosition(const SliderTickPosition tickPos);
	SliderTickPosition GetTickPosition() const;

	// 指示器最值（刻度极值）
	UNIVERSAL_LIB_API void GetRange(int& iMin, int& iMax) const;
	UNIVERSAL_LIB_API void SetRange(const int iMin, const int iMax);

	// 单步长，-> | <- | mousewheel
	UNIVERSAL_LIB_API int GetSingleStep() const;
	UNIVERSAL_LIB_API void SetSingleStep(const int iStep);

	// 页步长, PageUp | PageDown
	UNIVERSAL_LIB_API int GetPageStep() const;
	UNIVERSAL_LIB_API void SetPageStep(const int iStep);

	// 设置各区域的Painter
	UNIVERSAL_LIB_API void SetSliderThumbPainter(IPainterSlider* pPainter);
	UNIVERSAL_LIB_API void SetSliderTickPainter(IPainterSlider* pPainter);
	UNIVERSAL_LIB_API void SetSliderProgressPainter(IPainterSlider* pPainter);

	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	void OnUICreate();
	bool OnUIGetKidsZOrder(std::vector<CUIObject*>* pResultVec);
	void OnUISize();
	void OnUIPrePaint();
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnUIMouseMove(UINT nFlags, POINT point);
	void OnUICaptureChanged(CUIObject* pCapture);
	void OnUILButtonDown(UINT nFlags, POINT point);
	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);

	LRESULT OnMouseWheel(UINT nFlags, short zDelta, POINT point);
	LRESULT OnSliderThumbPosChange(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	
	friend class CUISliderProgress;
	friend class CUISliderTick;
	friend class CUISliderThumb;

	void OnDrawSliderTick(CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha);
	void OnDrawSliderProgress(CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha);
	void OnDrawSliderThumb(CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha);
	
protected:

	void SetProgressSize(const SIZE& szProgress);
	SIZE GetProgressSize() const;

	// 键盘控制
	void LineUp();
	void LineDown();
	void PageUp();
	void PageDown();
	void HomeUp();
	void EndDown();

	bool IsSliderInitOK() const;

	// 重新调整参数
	virtual void ReCalcSliderArgs() = 0;
	virtual void ReLayout() = 0;
	virtual void AdjustProgressByPoint(CPoint pt) = 0;

	// 计算最近的步进坐标
	int CalcNearestStepPosByPixel(int iPosPixel);

	// Slider方向
	void SetSliderDirection(const SliderDirection direction);
	SliderDirection GetSliderDirection() const;
	
	// 外部通知
	void SendNotfiyCommand(UINotifyCommandCode iCode);
	
protected:

	// Public Property
	int		m_iMinValue;
	int		m_iMaxValue;
	int		m_iCurrentValue;			// 当前Value = m_iMinValue + m_iLogicValue
	int		m_iSingleStepValue;			// 单步移动值
	int		m_iPageStepValue;			// 分页移动值
	CRect	m_rcSliderPadding;			// 默认为{1,1,1,1}
	
	SliderDirection		m_eDirection;
	SliderTickPosition	m_eTickPos;
	
	CSize	m_szSliderProgress;
	CSize	m_szSliderThumb;
	
	// Private Property
	CUISliderThumb		m_SliderThumb;			// 滑块
	CUISliderProgress	m_SliderProgress;		// 指示器Tick
	CUISliderTick		m_SliderTick;			// 进度条

	// 三个Painter，分别描绘对应的区域
	IPainterSlider*	m_pPainterSliderTick;
	IPainterSlider*	m_pPainterSliderProgress;
	IPainterSlider*	m_pPainterSliderThumb;
	
	// 当前Value对应的像素
	int	m_iRealProgressPixel;

	// Slider中的始末像素逻辑坐标位置
	int		m_iLogicPixelBegin;
	int		m_iLogicPixelEnd;
	int		m_iLogicPixelLength;

	bool	m_bNeedLayout;
};

class CUIHSlider : public CUISliderBase
{
public:
	UNIVERSAL_LIB_API explicit CUIHSlider(const SIZE szThumbBlock = CSize(0, 0), const int iProgressHeight = 0, RECT rcPadding = CRect(0, 0, 0, 0));
	~CUIHSlider() {};

	UNIVERSAL_LIB_API void SetProgressHeight(int iHeight);

private:

	virtual void ReCalcSliderArgs();
	virtual void ReLayout();
	virtual void AdjustProgressByPoint(CPoint pt);
};

class CUIVSlider : public CUISliderBase
{
public:

	UNIVERSAL_LIB_API explicit CUIVSlider(const SIZE szThumbBlock = CSize(0, 0), const int iProgressWidth = 0, RECT rcPadding = CRect(0, 0, 0, 0));
	~CUIVSlider() {};

	UNIVERSAL_LIB_API void SetProgressWidth(int iWidth);

private:

	virtual void ReCalcSliderArgs();
	virtual void ReLayout();
	virtual void AdjustProgressByPoint(CPoint pt);
};


//////////////////////////////////////////////////////////////////////////

class IPainterSlider : public IPainter
{

public:

	// 请实现这两个接口以完成Tick的定制描绘
	virtual void PaintRect(CDCHandle dc, CUIObject* pSliderTick, int iMinVal, int iMaxVal, int iCurVal, const RECT& rc, DWORD dwStatus, BYTE cAlpha) = 0;
	virtual bool IsPainterManaged() { return false; }

	// 对外的绘画接口
	LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha /* = 255 */) override
	{
		CUISliderTick* pSliderTick = static_cast<CUISliderTick*>(pUI);
		CUISliderBase* pSlider = static_cast<CUISliderBase*>(pSliderTick->UIGetParent());
		int iMin = 0;
		int iMax = 0;
		pSlider->GetRange(iMin, iMax);
		PaintRect(dc, pUI, iMin, iMax, pSlider->GetValue(), rc, dwStatus, cAlpha);
		return 0;
	}
};

