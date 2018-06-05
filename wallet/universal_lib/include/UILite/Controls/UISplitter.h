// Author: Miaojinquan
#pragma once
#include "../../Universal.h"
#include "../UICrack.h"
#include "UIControlEvent.h"
#include "UIControls.h"
#define MAX_SPLITBAR_WIDTH	50


enum SplitterDirection
{
	Splitter_Horz,	// 水平
	Splitter_Vert,	// 垂直
};

enum SplitterPanePos
{
	PanPos_Pane1,	// left or top
	PanPos_Pane2	// right or bottom
};

// CUISplitterWindow的子窗口
struct SplitterPaneInfo
{
	CUIObject*	pWnd;		// 窗口
	int			iPos;	// 位置
	int			iMinWidth;	// 最小值
	int			iMaxWidth;	// 最大值
	bool		bFixed;		// 窗口拉伸时，窗口大小是否固定
};

struct UISplitBarSizeNotify
{
	UINH hdr;
	int* iNewPos;	// 用户可以修改这个值
};

class CFixedPaneSize
{
public:

	explicit CFixedPaneSize(int iInitialWidth, int iMinWidth = 0, int iMaxWidth = -1)
		: m_iInitialWidth(iInitialWidth), m_iMinWidth(iMinWidth), m_iMaxWidth(iMaxWidth)
	{}

	~CFixedPaneSize() {}

protected:
	friend class CUISplitter;
	int m_iInitialWidth;
	int m_iMinWidth;
	int m_iMaxWidth;
};

class CAutoPaneSize
{
public:

	explicit CAutoPaneSize(int iIntialPercent/* 0 - 100*/, int iMinWidth = 0, int iMaxWidth = -1)
		: m_iInitialPercent(iIntialPercent), m_iMinWidth(iMinWidth), m_iMaxWidth(iMaxWidth)
	{ }
	~CAutoPaneSize() {}

protected:
	friend class CUISplitter;
	int m_iInitialPercent;	// 初始百分比
	int m_iMinWidth;		// 最小像素值
	int m_iMaxWidth;		// 最大像素值
};

// 分隔条
class CUISplitterBar: public CUIControl
{
public:

	explicit CUISplitterBar(const SplitterDirection eDirection);
	~CUISplitterBar() { }

	void SetDirection(const SplitterDirection eDirection) { m_eDirection = eDirection; UIRedraw(); }
	SplitterDirection GetDirection() const { return m_eDirection; }

	void SetMovable(const bool bMovable) { UIDisable(!bMovable); }	// 设置是否可以移动SplitterBar
	bool IsMovable() const { return !UIIsDisabled(); }

	int GetWidth() const { return m_iWidth; }	// SplitterBar宽度 setter/getter
	void SetWidth(const int iWidth) { m_iWidth = std::min(std::max(iWidth, 0), MAX_SPLITBAR_WIDTH); }

	void OnUICreate();
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	bool OnUISetCursor(UINCHitTestCode tc, int iParam);
	void OnUIMouseMove(UINT nFlags, POINT point);
	void OnUICaptureChanged(CUIObject* pCapture);
	void OnUILButtonDown(UINT nFlags, POINT point);
	void OnUILButtonUp(UINT nFlags, POINT point);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	SplitterDirection		m_eDirection;		// 方向
	int						m_iWidth;			// 分隔条宽度
	bool					m_bPressed;			// 是否鼠标按下
	POINT					m_ptPressed;		// 鼠标按下的位置
};

class CUISplitter : public CUIControl
{
public:

	UNIVERSAL_LIB_API explicit CUISplitter(const SplitterDirection eDirection = Splitter_Horz);
	~CUISplitter() { }

	UNIVERSAL_LIB_API bool SetPane(const SplitterPanePos PanePos, CUIObject *pWnd, const CFixedPaneSize& PaneSize);
	UNIVERSAL_LIB_API bool SetPane(const SplitterPanePos PanePos, CUIObject *pWnd, const CAutoPaneSize& PaneSize);

	UNIVERSAL_LIB_API void SetBarPos(int iPos);
	int GetBarPos() const { return m_eDirection == Splitter_Horz ? m_SplitterBar.UIGetPos().x : m_SplitterBar.UIGetPos().y; }
	UNIVERSAL_LIB_API void SetDirection(const SplitterDirection eDirection); // 设置分割方向
	SplitterDirection GetDirection() const { return m_eDirection; }

	void SetBarMovable(const bool bMovable) { m_SplitterBar.SetMovable(bMovable); }
	bool IsBarMovable() const { return m_SplitterBar.IsMovable(); }

	void SetBarWidth(const int iWidth) { m_SplitterBar.SetWidth(iWidth); ReLayout(); }
	int GetBarWidth() const { return m_SplitterBar.GetWidth(); }

	void SetBarPainter(IPainter* pPainter, bool bAutoDelete = false) { m_SplitterBar.UISetPainter(pPainter, bAutoDelete); }

	//////////////////////////////////////////////////////////////////////////
	void OnUICreate();
	void OnUISize();
	void OnUIDestroy();
	LRESULT OnUINotifyBarMoving(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);


	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

protected:

	UNIVERSAL_LIB_API void ReLayout();

	int GetCount() const;
	SplitterPaneInfo GetFirstValidInfo();

	SplitterDirection					m_eDirection;
	CUISplitterBar						m_SplitterBar;
	SplitterPaneInfo					m_Panes[2];
};
