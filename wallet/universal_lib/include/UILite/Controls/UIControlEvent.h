// Author: ChenJianjun
#pragma once
#include "../Core/UICoreDef.h"


enum UIControlEvent
{
	UIControlEvent_Begin = UIEvent_Control,

	// TreeView
	UITVEvent_GetNodeElementWidth,	// 取某个Element的Width, wParam 为UITreeViewNodePart
	UITVEvent_GetNodeHeight,
	UITVEvent_GetButtonImage,
	UITVEvent_GetNodeWidth, // 取节点的宽度, 节点的宽度是以最宽的那个节点来定的.
	UITVEvent_GetTextColor,
	UITVEvent_GetTagTextColor,
	UITVEvent_LeftButtonDown,
	UITVEvent_RightButtonDown,
	UITVEvent_Click,
	UITVEvent_RClick,
	UITVEvent_DblClick,
	UITVEvent_CheckNodeVisible, // 检查节点当前是否可见
	UITVEvent_MouseOver,
	UITVEvent_MouseLeave,
	UITVEvent_CustomDrawLButtonDown,
	UITVEvent_CustomDrawLButtonUp,
	UITVEvent_CustomDrawLButtonDblClk,
	UITVEvent_CustomDrawMouseMove,
	UITVEvent_CustomDrawRButtonDown,
	UITVEvent_CustomDrawRButtonUp,
	UITVEvent_CustomDrawRButtonDblClk,
	UITVEvent_CustomDrawMouseOver,
	UITVEvent_CustomDrawMouseLeave,
	UITVEvent_BeginDrag,
	UITVEvent_ContextMenu,

	// ComboBox
	UICBEvent_AsyncSelAll,	// 全选
	UICBEvent_DropListItemMouseOver,

	// Edit
	UIEEvent_BeginEdit,
	UIEEvent_EndEdit,	// wparam: bool - is cancel		lparam: wchar_t*
	UIEEvent_TextChange,
	UIEEvent_EditBoxKillFocus,
	UIEEvent_EditBoxKeyDown,
	UIEEvent_SetFocus,
	UIEEvent_KillFocus,
	UIEEvent_LeftButtonDown,
	UIEEvent_LeftButtonUp,
	UIEEvent_KeyDown,
	UIEEvent_KeyUp,
	UIEEvent_SysKeyDown,
	UIEEvent_SysKeyUp,
	UIEEvent_Char,
	UIEEvent_NumberOnly,
	UIEEvent_ContextMenu,
	UIEEvent_IMENotify,
	UIEEvent_AsyncScroll,

	// Toolbar
	UITBEvent_ButtonMouseMove,
	UITBEvent_ButtonLDown,
	UITBEvent_ButtonLUp,
	UITBEvent_ButtonLDblClk,
	UITBEvent_ButtonRDown,
	UITBEvent_ButtonRUp,
	UITBEvent_ButtonRDblClk,
	UITBEvent_ButtonMDown,
	UITBEvent_ButtonMUp,
	UITBEvent_ButtonMDblClk,
	UITBEvent_ButtonMouseEnter,
	UITBEvent_ButtonMouseLeave,
	UITBEvent_ShowDropdown,

	// splitter
	UISPLITEvent_TrackMove,

	// listview
	UILVEvent_HeaderSectionResized,
	UILVEvent_HeaderLayoutChanged,
	UILVEvent_HeaderClicked,
	UILVEvent_ItemCreated,
	UILVEvent_ItemLayout,
	UILVEvent_ItemMouseEnter,
	UILVEvent_ItemMouseLeave,
	UILVEvent_ItemLButtonDown,
	UILVEvent_ItemLButtonUp,
	UILVEvent_ItemLButtonDblClk,
	UILVEvent_ItemRButtonDown,
	UILVEvent_ItemRButtonUp,
	UILVEvent_ItemRButtonDblClk,
	UILVEvent_ItemStatusChanged,
	UILVEvent_ItemHeightChanged,
	UILVEvent_ContentAreaChanged,
	UILVEvent_SelectionChanged,

	// slider
	UISLIDEEvent_ThumbPosChanged,		// thumb -> slider

	// button
	UIBNEvent_ResetDropdownTemporarilyStatus,
};

#define UITBEvent_ButtonMouseFirst UITBEvent_ButtonMouseMove
#define UITBEvent_ButtonMouseLast UITBEvent_ButtonMouseLeave

enum UINotifyCommandCode
{
	// Common Notify
	UINM_LBUTTONDOWN = 1,	// Notify by UIEvent_Notify	param: UINH*
	UINM_RBUTTONDOWN,		// Notify by UIEvent_Notify	param: UINH*
	UINM_CLICK,				// Notify by UIEvent_Notify	param: UINH*
	UINM_RCLICK,			// Notify by UIEvent_Notify	param: UINH*
	UINM_DBLCLK,			// Notify by UIEvent_Notify	param: UINH*
	UINM_RDBLCLK,			// Notify by UIEvent_Notify	param: UINH*
	UINM_KEYDOWN,			// Notify by UIEvent_Notify	param: UINH*
	UINM_KEYUP,				// Notify by UIEvent_Notify	param: UINH*
	UINM_SYSKEYDOWN,		// Notify by UIEvent_Notify	param: UINH*
	UINM_SYSKEYUP,			// Notify by UIEvent_Notify	param: UINH*
	UINM_CHAR,				// Notify by UIEvent_Notify	param: UINH*
	UINM_SETCURSOR,			// Notify by UIEvent_Notify	param: UINH*
	UINM_SCROLLCHANGE,		// Notify by UIEvent_Notify	param: UINH*
	UINM_GETTOOLTIP,		// Notify by UIEvent_Notify	param: UINH*
	UINM_GETDISPINFO,
	UINM_MOUSEENTER,
	UINM_MOUSELEAVE,
	UINM_FOCUSENTER,
	UINM_FOCUSLEAVE,

	// Button Notify
	// UIBN_XXX
	UIBN_CLICK,				// Notify by UIEvent_Notify	param: UINH*
	UIBN_CHECKCHANGE,		// Notify by UIEvent_Notify	param: UINH*
	UIBN_COMBOCLICK,		// Notify by UIEvent_Notify	param: UINH*
	UIBN_COMBODROPDOWN,		// 显示下拉列表，如果返回TRUE，则会阻止下拉列表的显示
	UIBN_COMBOCLOSEUP,		// 关闭下拉列表

	// Edit Notify
	// UIEN_XXX
	UIEN_KEYDOWN,
	UIEN_RETURN,
	UIEN_TEXTCHANGE,			// Notify by UIEvent_Notify	param: UINH*
	UIEN_SETFOCUS,
	UIEN_KILLFOCUS,
	UIEN_NUMBERONLY,

	// SysLink Notify
	// UISLN_XXX
	UISLN_CLICK,				// Notify by UIEvent_Command	param: UINH*
	UISLN_RETURN,				// Notify by UIEvent_Command	param: UINH*
	UISLN_MOUSEENTER,
	UISLN_MOUSELEAVE,

	// TreeView Notify
	// UITVN_XXX
	UITVN_EXPANDING,	// 返回值: FALSE - 继续		TRUE - 取消本次操作
	UITVN_EXPANDED,
	UITVN_SELCHANGED,
	UITVN_SELCHANGING,	// 返回值: FALSE - 继续		TRUE - 取消本次操作
	UITVN_CHECKCHANGED,
	UITVN_KEYDOWN,		// 返回值: FALSE - 继续		TRUE - 取消本次操作
	UITVN_CLICK,		// 返回值: FALSE - 继续		TRUE - 取消本次操作
	UITVN_RCLICK,		// 返回值: FALSE - 继续		TRUE - 取消本次操作
	UITVN_DBLCLICK,		// 返回值: FALSE - 继续		TRUE - 取消本次操作
	UITVN_BEGINEDIT,	// 返回值: FALSE - 继续		TRUE -  取消编辑
	UITVN_ENDEDIT,
	UITVN_GETEDITPOS,	// 返回值: TRUE - 使用提供的 POS，FALSE - 使用默认 POS
	UITVN_MOUSEOVER,
	UITVN_MOUSELEAVE,
	UITVN_SETCURSOR,
	UITVN_BEGINDRAG,
	UITVN_CONTEXTMENU,

	// ComboBox Notify
	// UICBN_XXX
	UICBN_DROPDOWN,
	UICBN_CLOSEUP,
	UICBN_SELENDOK,
	UICBN_SELENDCANCEL,
	UICBN_SETFOCUS,
	UICBN_KILLFOCUS,
	UICBN_SELCHANGING,
	UICBN_SELCHANGED,
	UICBN_EDITTEXTCHANGE,

	// TabCtrl Notify
	// UITABN_XXX

	// Toolbar Notify
	// UITBN_XXX
	UITBN_STATUSCHANGED,
	UITBN_DROPDOWN,
	UITBN_CHEVRONDROPDOWN,
	UITBN_LBUTTONDOWN,
	UITBN_LBUTTONUP,
	UITBN_LBUTTONDBLCLK,
	UITBN_CLICK,
	UITBN_RBUTTONDOWN,
	UITBN_RBUTTONUP,
	UITBN_RBUTTONDBLCLK,
	UITBN_RCLICK,
	UITBN_MBUTTONDOWN,
	UITBN_MBUTTONUP,
	UITBN_MBUTTONDBLCLK,
	UITBN_MCLICK,
	UITBN_MOUSEMOVE,
	UITBN_BEGINDRAG,
	UITBN_TEXTCALLBACK,

	// Splitter Notify
	// UISPLTN_XXX
	UISPLTN_SIZING,

	// ListView Notify
	// UILVN_XXX
	UILVN_HEADCLICKED,
	UILVN_ITEMDBCLICK,
	UILVN_ITEMLBUTTONDOWN,
	UILVN_ITEMLBUTTONUP,
	UILVN_ITEMLDBCLICK,
	UILVN_ITEMRBUTTONDOWN,
	UILVN_ITEMRBUTTONUP,
	UILVN_GETEDITPOS,
	UILVN_BEGINEDIT,
	UILVN_ENDEDIT,
	UILVN_GETEDITCOLUMN,	// 返回值: Column - 使用返回值作为要编辑的 Column
	UILVN_KEYDOWN,
	UILVN_SCROLLCHANGED,	// 滚动条发生滚动
	UILVN_SELECTIONCHANGED,
	UILVN_CHECKSTATECHANGED,

	// Slider Notify
	// UISLDN_XXX
	// 注意： 如果想要在界面初始化完成时，就能得到回调值：进度、坐标等，请在优先保证Slider的属性正确设置后，主动调用SetValue。
	UISLDN_THUMBPOSITION,
	UISLDN_THUMBTRACK,
	UISLDN_TOP,
	UISLDN_BOTTOM,
	UISLDN_LINEDOWN,
	UISLDN_LINEUP,
	UISLDN_PAGEDOWN,
	UISLDN_PAGEUP,

	// DropdownList Notify
	// UIDLN_XXX
	UIDLN_CLOSE,
	UIDLN_CLICK,

	// TAB Notify
	UITABN_SELCHANGE,

	// RichLinkNotify
	UIRLN_CLICK,
	
	// TuoImagePlayerNotify
	UITIP_DONE,

	UINotify_User = 0x400
};

