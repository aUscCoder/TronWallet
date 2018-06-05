// Author: ChenJianjun
#pragma once

struct _UIPostEvent
{
	int iEvent;
	WPARAM wParam;
	LPARAM lParam;
};

struct DEvent
{
	int iEvent;
	WPARAM wParam;
	LPARAM lParam;
	void* lpReserved;
};

struct PaintTransitionWParam
{
	DWORD dwOldStatus;
	DWORD dwNewStatus;
	float fProgress;
};

// UIEvent
enum UIEvent
{
	UIEvent_None = 0,

	UIEvent_Create,
	UIEvent_Destroy,

	//
	// modalUI相关Event
	//
	UIEvent_ModalInit,	// 初始化
	UIEvent_ModalUninit, // 反初始化
	UIEvent_ModalAniInit, //动画初始化
	UIEvent_ModalAniUninit,	//动画反初始化

	//
	// Paint
	//
	UIEvent_PrePaint,
	UIEvent_PaintBackgound,		// wParam: Status, lParam: HDC
	UIEvent_Paint,				// wParam: Status, lParam: HDC
	UIEvent_PaintTransition,	// wParam: PaintTransitionWParam, lParam: HDC

	//
	// Status
	//
	UIEvent_WantArrow,
	UIEvent_WantTab,
	UIEvent_WantEnter,
	UIEvent_WantEscape,

	//
	// Message
	//
	UIEvent_GetFont,
	UIEvent_SetFont,
	UIEvent_GetObjectType,
	UIEvent_GetObjectTypeStr,
	UIEvent_GetOrigObjectType,
	UIEvent_GetOrigObjectTypeStr,
	UIEvent_GetTextColor,
	UIEvent_SetTextColor,
	UIEvent_SetDisabledTextColor,
	UIEvent_GetDisabledTextColor,
	UIEvent_SetText,
	UIEvent_GetText,
	UIEvent_GetTextLength,
	UIEvent_LeftButtonDown,	// Equal to WM_LBUTTONDOWN
	UIEvent_LeftButtonUp,	// Equal to WM_LBUTTONUP
	UIEvent_LeftButtonDblClk,// Equal to WM_LBUTTONDBLCLK
	UIEvent_MiddleButtonDown,// Equal to WM_MBUTTONDOWN
	UIEvent_MiddleButtonUp,	// Equal to WM_MBUTTONUP
	UIEvent_MiddleButtonDblClk,	// Equal to WM_MBUTTONDBLCLK
	UIEvent_RightButtonDown,	// Equal to WM_RBUTTONDWON
	UIEvent_RightButtonUp,	// Equal to WM_RBUTTONUP
	UIEvent_RightButtonDblClk,// Equal to WM_RBUTTONDBLCLK
	UIEvent_XButtonDown,// Equal to WM_XBUTTONDOWN
	UIEvent_XButtonUp,	// Equal to WM_XBUTTONUP
	UIEvent_XButtonDblClk,	// Equal to WM_XBUTTONDBLCLK
	UIEvent_MouseMove,		// Equal to WM_MOUSEMOVE
	UIEvent_MouseWheel,		// Equal to WM_MOUSEWHEEL
	UIEvent_MouseEnter,		// Equal to WM_MOUSEENTER
	UIEvent_MouseLeave,		// Equal to WM_MOUSELEAVE
	UIEvent_KeyDown,			// Equal to WM_KEYDOWN - 键盘按下, VK_TAB不会进这里面来
	UIEvent_KeyUp,			// Equal to WM_KEYUP
	UIEvent_Char,			// Equal to WM_CHAR
	UIEvent_MenuChar,		// Equal to WM_MENUCHAR
	UIEvent_SysKeyDown,			// Equal to WM_KEYDOWN - 键盘按下, VK_TAB不会进这里面来
	UIEvent_SysKeyUp,			// Equal to WM_KEYUP
	UIEvent_SysChar,			// Equal to WM_CHAR
	UIEvent_SetCursor,		// Equal to WM_SETCURSOR
	UIEvent_Active,			//
	UIEvent_ActiveApp,		//
	UIEvent_GetDlgCode,
	UIEvent_ContextMenu,

	//
	// Touch
	//

	UIEvent_TouchDown,
	UIEvent_TouchUp,
	UIEvent_TouchMove,
	UIEvent_TouchEnter,
	UIEvent_TouchLeave,

	//
	// IME
	//
	UIEvent_IMEChar,				// Equal to WM_IME_CHAR
	UIEvent_IMEComposition,			// Equal to WM_IME_COMPOSITION
	UIEvent_IMECompositionFull,		// Equal to WM_IME_COMPOSITIONFULL
	UIEvent_IMEControl,				// Equal to WM_IME_CONTROL
	UIEvent_IMEEndComposition,		// Equal to WM_IME_ENDCOMPOSITION
	UIEvent_IMEKeyDown,				// Equal to WM_IME_KEYDOWN
	UIEvent_IMEKeyUp,				// Equal to WM_IME_KEYUP
	UIEvent_IMENotify,				// Equal to WM_IME_NOTIFY
	UIEvent_IMERequest,				// Equal to WM_IME_REQUEST
	UIEvent_IMESelect,				// Equal to WM_IME_SELECT
	UIEvent_IMESetContext,			// Equal to WM_IME_SETCONTEXT
	UIEvent_IMEStartComposition,	// Equal to WM_IME_STARTCOMPOSITION

	UIEvent_Timer,			// wparam: UINT_PTR - timer ID

	UIEvent_SetFocus,		// wparam: NULL			lparam: NULL
	UIEvent_KillFocus,		// wparam: NULL			lparam: NULL
	UIEvent_FocusEnter,
	UIEvent_FocusLeave,

	// Notify Event
	UIEvent_ScrollChange,
	UIEvent_CaptureChange,	// wparam: NULL			lparam: CUIObject* - window gaining the mouse
	UIEvent_ActionStopped,		// wParam: CDecorator* pAction

	UIEvent_ShowUI,			// wparam: bool - bShow
	UIEvent_EnableUI,	// wparam: bool - bEnable	lparam: NULL
	UIEvent_WindowChange,	// 窗口的大小, 位置, 滚动条等发生了变化
	UIEvent_HScroll,			// wparam: DUIVScrollEvent		lparam: int* - 滚动条变化量
	UIEvent_VScroll,			// wparam: DUIHScrollEvent		lparam: int* - 滚动条变化量
	UIEvent_StyleChange,		// wparam: NULL		lparam: DWORD - dwOldStats
	UIEvent_UITextChange, // wparam: NULL			lparam: NULL
	UIEvent_FontChange,
	UIEvent_DefaultFontChange,
	UIEvent_DefaultColorChange,
	UIEvent_DefaultGrayColorChange,
	UIEvent_TextColorChange,
	UIEvent_DisabledColorChange,
	UIEvent_Size,
	UIEvent_UIPosChanged,
	UIEvent_StatusChanged,
	UIEvent_CombinedStatusChanged,

	UIEvent_GetChildrenForPaint,	// wparam: std::vector<CUIObject*>*	lparam: CRect*
	// 返回在指定区域内的子窗口, 如果重载的话要保证返回的子窗口的Z-Order
	UIEvent_GetKidsZOrder,		// wparam: std::vector<CUIObject*>*
	UIEvent_CalcChildrenRect,	// 计算子窗口所占的区域, 用于滚动条的显示
	UIEvent_GetTooltipText,		// tooltip text

	UIEvent_NCHitTest,
	UIEvent_HitTest,

	UIEvent_MeasureClient,		// wparam: NULL			lparam: CRect* - require rect
	UIEvent_MeasureScrollbar,	// wparam: NULL			lparam: bool - bHorz
	UIEvent_MeasureChild,		// wparam: CUIObject*	lparam: CRect* - require rect

	UIEvent_DefaultChanged,		// wparam: BOOL bDefault

	UIEvent_GetRootType,		// LRESULT: UIRootType
	UIEvent_RootFocusChange,
	UIEvent_RootSetDefault,
	UIEvent_RootGetDefault,
	UIEvent_RootChange,

	UIEvent_Command,		// wparam: NULL			lparam: UINH*
	UIEvent_ReflectCommand,

	UIEvent_Control = 0x100,

	/////////////////////////////////////
	// UIControl_Events
	/////////////////////////////////////

	UIEvent_User = 0x400,	//
};

enum UIRootType
{
	UIRootType_Normal,
	UIRootType_Modal,
};

#define UIEvent_MouseFirst		UIEvent_LeftButtonDown
#define UIEvent_MouseLast		UIEvent_MouseLeave
