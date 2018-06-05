// Author: ChenJianjun
#pragma once
#include "../Core/UICoreDef.h"


enum UIControlStatus
{
	UIControlStatus_Default = 0x80000000,
};

enum UIListViewStatus
{
	UILVStatus_ItemEditing	= 0x10000,
	UILVStatus_ItemSelected = 0x20000,
	UILVStatus_ItemChecked	= 0x40000,
};

enum UIToolbarStatus
{
	UITBStatus_Hover				= 0x010000,
	UITBStatus_Pressed				= 0x020000,
	UITBStatus_Dropdown				= 0x040000,
	UITBStatus_DropdownPartHover	= 0x080000,
	UITBStatus_ToolbarHover			= 0x100000,
	UITBStatus_ToolbarFocus			= 0x200000,
};

enum UIButtonStatus
{
	UIBTNStatus_DropdownPartHover	= 0x10000,
	UIBTNStatus_Dropdown			= 0x20000,

	UIBTNStatus_Mask_Check			= 0xc0000,
	UIBTNStatus_Unchecked			= 0x00000,
	UIBTNStatus_Checked				= 0x40000,
	UIBTNStatus_Mixed				= 0x80000,
	UIBTNStatus_Unused				= 0xc0000,
};

enum UIDropdownListStatus
{
	UIDLStatus_Selected			= 0x00010000,
};

enum UIPBStatus
{
};

enum UIButtonStyle
{
	UIButtonStyle_LeftText			= 0x00010000,
};

enum UIStaticStyle
{
	UIStaticStyle_HidePrefix		= 0x00010000,	// &&->&
	UIStaticStyle_NoPrefix			= 0x00020000,	// &-->&
	UIStaticStyle_End_Ellipsis		= 0x00040000,
	UIStaticStyle_VCenter			= 0x00080000,
	UIStaticStyle_Bottom			= 0x00100000,
	UIStaticStyle_Center			= 0x00200000,
	UIStaticStyle_Right				= 0x00400000,
	UIStaticStyle_MultiLine			= 0x00800000,
};

enum UIEditStyle
{
	UIEditStyle_VCenter			= 0x00010000,	// 竖直居中，不能与 UIEditStyle_MultiLine/UIStyle_HScroll/UIStyle_VScroll  一起使用
	UIEditStyle_Password		= 0x00020000,
	UIEditStyle_ReadOnly		= 0x00040000,
	UIEditStyle_MultiLine		= 0x00080000,
	UIEditStyle_ClientEdge		= 0x00100000,
	UIEditStyle_EndEllipsis		= 0x00200000,
	UIEditStyle_Number			= 0x00400000,
	UIEditStyle_AutoHScroll		= 0x00800000,
	UIEditStyle_AutoVScroll		= 0x01000000,
	UIEditStyle_WordWrap		= 0x02000000,
	UIEditStyle_RichEdit		= 0x04000000,
	UIEditStyle_NoHideSel		= 0x08000000,
	UIEditStyle_PatchDraw		= 0x10000000,
};

enum UIComboBoxStyle
{
	UICBStyle_AutoHScroll	= 0x00100000,
	UICBStyle_ReadOnly		= 0x00200000,
	UICBStyle_DropDown		= 0x00400000,
	UICBStyle_DropDownList	= 0x00800000,
	UICBStyle_Sort			= 0x01000000,
	UICBStyle_Image			= 0x02000000,
	UICBStyle_Extension		= 0x04000000, // 特殊风格的combox，像浏览器的地址栏那样
};

enum UIDropdownListStyle
{
	UIDLStyle_Image		= 0x00010000,
	UIDLStyle_Sort		= 0x00020000,
};

enum UITreeViewStyle
{
	UITVStyle_LineAtRoot			= 0x00040000,
	UITVStyle_HasButton				= 0x00080000,
	UITVStyle_HasImage				= 0x00100000,
	UITVStyle_HasLine				= 0x00200000,
	UITVStyle_HasCheck				= 0x00400000,
	UITVStyle_SingleSelection		= 0x00800000,
	UITVStyle_EditLabel				= 0x01000000,
	UITVStyle_AutoCheck				= 0x02000000,	// 如果设置了这个状态, 那么tree会自动维护checkbox
	UITVStyle_ButtonTripleStyle		= 0x04000000,
	UITVStyle_NoStatusBackground	= 0x08000000,	// 当不画选中或鼠标Hover的状态背景
	UITVStyle_ClickSelection		= 0x10000000,	// 鼠标左键点下的时候就选中
};

enum UIListViewStyle
{
	UILVStyle_ShowSelectionAlways	= 0x00020000,
	UILVStyle_CheckBox				= 0x00040000,
	UILVStyle_ShowGrid				= 0x00080000,
	UILVStyle_SingleSelection		= 0x00100000,
	UILVStyle_FullRowSelect			= 0x00200000,
	UILVStyle_EditLabel				= 0x00400000,
	UILVStyle_ShowSelectedItem		= 0x00800000,
	UILVStyle_TrackingMouse			= 0x01000000,
	UILVStyle_SimpleList			= 0x10000000,
};

enum UIProgressBarStyle
{
	UIPBStyle_MoveOverlay			= 0x10000,
};

enum UISysLinkStyle
{
	UISLStyle_UnderLine		= 0x10000000,
};

enum UIHorzLayoutStyle
{
	UIHLStyle_Center	= 0x00010000,
	UIHLStyle_Bottom	= 0x00020000,
};

enum UIVertLayoutStyle
{
	UIVLStyle_Center = 0x00010000,
	UIVLStyle_Right = 0x00020000,
};

struct UINH	// UINotifyHead
{
	CUIObject* pControl;
	DWORD_PTR pdwControlID;
	DWORD dwCode;
	DWORD dwParam;
};

struct UITTN
{
	UINH hdr;
	std::wstring strText;
};

#define COMBO_DROPDOWNLIST_CLASS_NAME L"__UI_COMBOBOX_DROP_LIST__"

#define DROPDOWNLIST_CLASS_NAME L"__UI_DROPDOWN_LIST__"

#define INVALID_ITEM		((UINT)-1)
