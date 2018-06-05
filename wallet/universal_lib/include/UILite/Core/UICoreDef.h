// Author: ChenJianjun
#pragma once
#include "../UIMsg.h"
#include "UICoreEvent.h"


#ifndef UNIVERSAL_LIB_API
#define UNIVERSAL_LIB_API
#endif

struct IUIManager;
struct UINH;
class CUINonClient;
class CUIRoot;
class IPainter;
class CUIObject;


#define MAKE_CALL(func)	((ActionFuncCall)(&func))
typedef void (CUIObject::*ActionFuncCall)(float fProgress, LPARAM lParam);
typedef POINT(*FNPathControl)(const POINT& ptStart, const POINT& ptEnd, float fProgress, LPARAM lParam);

union FNActionFuncCall
{
	ActionFuncCall func;
	struct
	{
		void* ptr;
		size_t adjustor;
	} func_ptr_struct;

	FNActionFuncCall(ActionFuncCall f) : func(f) {}
};


#define _DECLARE_INLINE

#define UI_DEFAULT_BORDER_WIDTH		2
#define UI_MAKE_FRAME(normal, hover, focus, pressed, Disabled) { (normal), (hover), (focus), (pressed), (Disabled) }
#define UITEST_HSCROLL(pDUIWnd) (pDUIWnd && pDUIWnd->m_pNC && pDUIWnd->UITestStyle(UIStyle_HScroll) && pDUIWnd->m_pNC->IsScrollbarVisible(true))
#define UITEST_VSCROLL(pDUIWnd) (pDUIWnd && pDUIWnd->m_pNC && pDUIWnd->UITestStyle(UIStyle_VScroll) && pDUIWnd->m_pNC->IsScrollbarVisible(false))
#define UITEST_BORDER(pDUIWnd) (pDUIWnd && pDUIWnd->m_pNC && pDUIWnd->UITestStyle(UIStyle_Border))
#define UI_IMPLEMENT_PAINTER(painter, pThis, dc, rc, stats)				\
	if ((painter))														\
	{																	\
		(painter)->DoPaint(pThis, (dc), (rc), (stats));					\
		return;															\
	}

#define UI_IMPLEMENT_RULER(ruler, sz)									\
	if (ruler)															\
	{																	\
		sz = (ruler)->DoMeasure(this);									\
		return true;													\
	}


// UILite系统内置的动画组
enum UISysAction
{
	SysAction_HScroll = -1000,
	SysAction_VScroll,
	SysAction_CommonPaintStatus,
	SysAction_CBStatusChanged,
	SysAction_CBDropdown,
	SysAction_DropdownList,
	SysAction_ProgressMoveOverlay,
	SysAction_Progress,
	SysAction_ToolbarStatusChanged,
};

enum UIScrollMode
{
	UIScrollMode_Now = 0,
	UIScrollMode_Quick,
	UIScrollMode_Smooth,
};

enum UIStateImageIndex
{
	UIIndex_Normal = 0,
	UIIndex_Hover,
	UIIndex_Focus,
	UIIndex_Pressed,
	UIIndex_Disabled,
	UIIndex_Active,
};

enum UIObjectType
{
	UIObject_Window,
	UIObject_Root,
	UIObject_Control,
	UIObject_Button,
	UIObject_Edit,
	UIObject_Checkbox,
	UIObject_Radio,
	UIObject_ListView,
	UIObject_TreeView,
	UIObject_Toolbar,
	UIObject_ToolbarButton,
	UIObject_HScrollbar,
	UIObject_VScrollbar,
	UIObject_ComboBox,
	UIObject_Slider,
	UIObject_Static,
	UIObject_HorzLayout,
	UIObject_VertLayout,
	UIObject_ControlEnd,

	// 本地窗口
	UIObject_NativeWindowBegin,

	UIObject_WinControls,
	UIObject_WinEdit,

#ifdef __UIA_ADJUST__
	UIObject_UILiteEnd,
#endif

	UIObject_Thor = 0x100,	//

	UIObject_User = 0x200,
};

// Valid CoreStatus Mask 0xFFFF
enum UICoreStatus
{
	UIStatus_Hover			= 0x0001,
	UIStatus_Pressed		= 0x0002,
	UIStatus_Focus			= 0x0004,
	UIStatus_Disabled		= 0x0008,
	UIStatus_HoverChild		= 0x0010,
	UIStatus_Capture		= 0x0020,
	UIStatus_FocusChild		= 0x0040,
	UIStatus_CaptureChild	= 0x0080,
	UIStatus_NoRedraw		= 0x0100,
	UIStatus_OwnIME			= 0x0200,
	UIStatus_MouseOver		= 0x0400,
	UIStatus_Hilight		= 0x0800,
//	UIStatus_NotUsed		= 0x1000,
	UIStatus_DeleteFinally	= 0x2000,
	UIStatus_Created		= 0x4000,
	UIStatus_Destroying		= 0x8000,
};

enum UIStyle
{
	UIStyle_Visible					= 0x0001,	// 可见
	UIStyle_WantIME					= 0x0002,
	UIStyle_Transparent				= 0x0004,	// 是否接受用户的输入（鼠标，键盘，焦点。。。）
	UIStyle_NoFocus					= 0x0008,	// 窗口不拿焦点
	UIStyle_TabStop					= 0x0010,
	UIStyle_Border					= 0x0020,	// 有边框
	UIStyle_HScroll					= 0x0040,	// 在必要时显示水平滚动条
	UIStyle_VScroll					= 0x0080,	// 在必要时显示垂直滚动条
	UIStyle_AutoScroll				= 0x0100,
	UIStyle_CombineStatusChange		= 0x0200,
	UIStyle_CommandTransparent		= 0x0400,
// 	UIStyle_NotUsed					= 0x0800,
// 	UIStyle_NotUsed					= 0x1000,
#ifdef __UIA_ADJUST__
	UIStyle_UIAHidden				= 0x2000,	// UIA 下不可见
#endif
	UIStyle_NonClient				= 0x4000,	// 非客户区窗口
	UIStyle_UIRoot					= 0x8000,	// 是UIRoot
};

enum UIScrollStyle
{
	UIScrollStyle_NoTriangle		= 0x04000000,
	UIScrollStyle_Float				= 0x08000000,
	UIScrollStyle_Outside			= 0x10000000,
};

enum UINCHitTestCode
{
	NCHitTest_NoWhere,		// 不在任何地方
	NCHitTest_Transparent,	// 消息透明
	NCHitTest_Client,		// 在客户区
	NCHitTest_HScroll,		// 在水平滚动条上
	NCHitTest_VScroll,

	NCHitTest_LeftBorder,
	NCHitTest_TopBorder,
	NCHitTest_RightBorder,
	NCHitTest_BottomBorder,
};

enum UIVScrollEvent
{
	VScroll_Top = 1,
	VScroll_Bottom,
	VScroll_Up,
	VScroll_Down,
	VScroll_PageUp,
	VScroll_PageDown,
	VScroll_TrackThumb,
	VScroll_WheelUp,	// 在客户区用滚轮
	VScroll_WheelDown,	// 在客户区用滚轮
};

enum UIHScrollEvent
{
	HScroll_LeftEdge = 1,
	HScroll_RightEdge,
	HScroll_Left,
	HScroll_Right,
	HScroll_PageLeft,
	HScroll_PageRight,
	HScroll_TrackThumb,
	HScroll_WheelLeft,	// 在客户区用滚轮
	HScroll_WheelRight,	// 在客户区用滚轮
};

enum class UIScrollbarType
{
	H, // horizontal
	V, // vertical
};

//////////////////////////////////////////////////////////////////////////
// public struct

struct ScrollbarInfo
{
	int iMin;
	int iMax;
	int iPage;
	int iPos;

	// 如果有平滑滚动，那么这里记录了平滑滚动完成后滚动条的最终位置
	int iDestPos;
};

struct ScrollbarPainterInfo
{
	IPainter* pTriangle1;
	bool bTriangle1Managed;

	IPainter* pTriangle2;
	bool bTriangle2Managed;

	IPainter* pNormalBk;
	bool bNormalBkManaged;

	IPainter* pTrackBk;
	bool bTrackBkManaged;

	IPainter* pThumb;
	bool bThumbManaged;
};

struct UIGetTooltip
{
	std::wstring strText;
};

//////////////////////////////////////////////////////////////////////////
struct IHookMessage
{
	virtual bool OnHookMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) = 0;
};

struct IPreTranslateMessageHook
{
	virtual bool OnPreTranslateMessage(MSG* lpMsg) = 0;
};

// 处理DUI Command
struct ICommandHandler
{
	virtual bool OnUICommand(LPCWSTR pszWindowID, DWORD dwControlID, DWORD dwCommand, UINH* pHDR, LRESULT& lResult) = 0;
};

class CUIEventMap
{
public:

	CUIEventMap() {}
	virtual ~CUIEventMap() {}

	virtual bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) { return false; }
};

class CTrackFinalEvent
{
public:
	explicit CTrackFinalEvent(CUIObject* p) : m_pUIObject(NULL), m_bLastHoldFinalEvent(false)
	{
		Attach(p);
	}

	CTrackFinalEvent(const CTrackFinalEvent& that)
	{
		m_bLastHoldFinalEvent = that.m_bLastHoldFinalEvent;
		m_pUIObject = that.m_pUIObject;

		((CTrackFinalEvent*)&that)->m_pUIObject = NULL;
	}

	virtual ~CTrackFinalEvent()
	{
		Detach();
	}

	operator CUIObject * () const // NOLINT (3.3.2 需要)
	{
		return m_pUIObject;
	}

protected:

	UNIVERSAL_LIB_API void Attach(CUIObject* p);
	UNIVERSAL_LIB_API void Detach();

protected:

	CTrackFinalEvent& operator = (const CTrackFinalEvent& that) { return *this; }

	bool m_bLastHoldFinalEvent;
	CUIObject* m_pUIObject;
};

#define UIDCANCEL IDCANCEL
#define UIDOK IDOK
#define UIDYES IDYES
#define UIDNO IDNO

#define UIASSERT	TuoAssert
