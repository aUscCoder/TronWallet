// Author: miaojinquan
#pragma once
#include "../UIControlDef.h"


class CUIListView;
enum UIListViewItemTextAlign
{
	UILVIAlign_Left = DT_LEFT,
	UILVIAlign_Center = DT_CENTER,
	UILVIAlign_Right = DT_RIGHT,
};

enum UIListViewItemPart
{
	UILVItemPart_Items = 0x0001,
	UILVItemPart_Image = 0x0002,
	UILVItemPart_CheckBox = 0x0003
};

struct UIListViewItemNotify
{
	UINH	hdr;
	int		iIndex = 0;
	DWORD	dwData = 0;
};

struct UIListViewHeaderClick
{
	UINH hdr;
	int iIndex = 0;
	POINT ptClicked = POINT{ -1, -1 };
	bool bResizableArea = false;	// 是否点击在Resizable区域
};

struct UIListViewCheck
{
	UINH hdr;
	UINT iRow = 0;
	bool bChecked = false;
};

struct UIListViewEditItem
{
	UINH	hdr;
	int		iRow = 0;
	int		iCol = 0;
	LPCWSTR	lpszText = 0;
};

struct UIListViewKey
{
	UINH hdr;
	UINT nChar = 0;
	UINT nRepCnt = 0;
	UINT nFlags = 0;
};

struct UIListViewColumn
{
	int iWidth = 0;								// 宽度
	int iMiniWidth = 0;								// 宽度最小值
	int iMaxWidth = -1;								// 宽度最大值
	UIListViewItemTextAlign lvcTextPos = UILVIAlign_Left;	// 文字的位置
	LPCWSTR lpszText = nullptr;							// 显示的文字
};

struct LVCompareContext
{
	CUIListView *pListView = nullptr;
	int iItem1 = 0;
	int iItem2 = 0;
	int iSubItem = 0;
	DWORD_PTR pdwData = 0;
	bool bAscending = false;
};

struct UIListViewItem
{
	HICON		hIcon = nullptr;
	DWORD_PTR	pdwData = 0;
	bool		m_bShowIcon = false;
	bool		m_bChecked = false;
};

#define DEFAULT_HEADER_HEIGHT	UI_SCALE(24)
#define LVSPACING				UI_SCALE(3)
typedef DWORD_PTR HLISTVIEWITEM;
