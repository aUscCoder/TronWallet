// Author: ChenJianjun
#pragma once
#include "../../CommonLib/UI/Bitmap.h"
#include "../../Universal.h"
#include "../Core/UIObject.h"
#include "../Core/UIUtil.h"
#include "../UICrack.h"
#include "UIControlDef.h"
#include "UIControlEvent.h"
#include "UIControls.h"
#include "UIEdit.h"
#include "UIWinControls.h"


typedef UINT_PTR HTREENODE;
#define TN_ROOT		((HTREENODE)-1)
#define TN_FIRST	((HTREENODE)-2)
#define TN_LAST		((HTREENODE)-3)

// 定义每一项的状态, 是不是被选中, 有没有Hover什么的
enum UITreeViewNodeStates
{
	UITVNodeStates_None			= 0,
	UITVNodeStates_Hover		= 0x00000001,
	UITVNodeStates_Selected		= 0x00000002,
	UITVNodeStates_Expanded		= 0x00000004,
	UITVNodeStates_Expandonce	= 0x00000008,	// 已经展开过

	UITVNodeStates_Editing		= 0x80000000,	// 正在编辑
};

// 节点定义
enum UITreeViewNodePart
{
	UITVNodePart_None			= 0,
	UITVNodePart_LeftBlank		= 0x00000001,		// 左边的空白区域
	UITVNodePart_Button			= 0x00000002,		// 按钮
	UITVNodePart_CheckBox		= 0x00000004,		// 复选框
	UITVNodePart_Image			= 0x00000008,		// 图标
	UITVNodePart_Text			= 0x00000010,		// 文字
	UITVNodePart_RightBlank		= 0x00000020,		// 右边空白区域
	UITVNodePart_ElementGap		= 0x00000040,		// 素之间的空隙
};

enum UITreeViewNodeCheckState
{
	UITVNodeCheckState_Mixed		= -1,		// 混合状态
	UITVNodeCheckState_Unchecked	= 0,		// 未选中
	UITVNodeCheckState_Checked		= 1,		// 选中
};

enum UITreeViewNodeExpand
{
	UITVNodeExpand_Collapse		= 0x1,	// TVE_COLLAPSE
	UITVNodeExpand_Expand		= 0x2,	// TVE_EXPAND
	UITVNodeExpand_Toggle		= 0x4,
	UITVNodeExpand_EventMask	= 0xF,

	UITVNodeExpand_CollapseReset = 0x80000000,	// 查看MSDN TVE_COLLAPSERESET
};

enum UITreeViewDraw
{
	UITVDraw_Line			= 0x00000001,
	UITVDraw_Button			= 0x00000002,
	UITVDraw_Image			= 0x00000004,
	UITVDraw_Check			= 0x00000008,
	UITVDraw_Background		= 0x00000010,
	UITVDraw_Text			= 0x00000020,

	UITVDraw_All			= UITVDraw_Line | UITVDraw_Button | UITVDraw_Image
								| UITVDraw_Check | UITVDraw_Background | UITVDraw_Text,
};

struct TreeViewNode
{
	HTREENODE hNode;
	DWORD dwStatus;
	LPCWSTR lpszText;
	int iImage;
	int iHeight;
	int iChild;
	int iCheck;
	DWORD_PTR pdwData;
};

struct UITVKey
{
	UINH hdr;
	UINT nChar;
	UINT nRepCnt;
	UINT nFlags;
};

struct UITVNode
{
	UINH hdr;
	TreeViewNode node;
};

struct UITVNodePart
{
	UINH hdr;
	TreeViewNode node;
	UITreeViewNodePart part;
};

struct UITVNodeClick
{
	UINH hdr;
	TreeViewNode node;
	UITreeViewNodePart clickPart;
	POINT point;
};

struct UITVNodeExpanding
{
	UINH hdr;
	DWORD dwAction;
	TreeViewNode node;
};

typedef UITVNodeExpanding UITVNodeExpanded;

struct UITVEditNode
{
	UINH hdr;
	TreeViewNode node;
	CUIEdit* pEdit;
};

struct UITVNodeEditPos
{
	UINH hdr;
	TreeViewNode hNode;
	POINT pos;
};

class CUITreeViewNodeUI;
struct _TreeViewNode
{
	DWORD dwStatus;
	DWORD_PTR pdwData;
	wchar_t* pszText;
	wchar_t* pszTagText;
	int iImage;
	int iHeight;
	int iTextWidth;	// 文本的像素宽度
	int iTagTextWidth; // Tag文本的像素宽度

	// -1 - 表示未定义, 以实际的个数为准.
	//  0 - 表示没有, 即使有子节点也不显示+
	//  1 - 表示有, 即使没有子节点也显示+
	int cChildren;

	// <0 - mix
	//  0 - unchecked
	// >0 - checked
	int iCheck;

	CUITreeViewNodeUI* pUI;
	_TreeViewNode* pParent;
	_TreeViewNode* pPrev;
	_TreeViewNode* pNext;
	_TreeViewNode* pKids;
	_TreeViewNode* pKidsEnd;

	// 公用方法
	_TreeViewNode();
	~_TreeViewNode();
	int GetTextWidth();
	int GetTagTextWidth();
	void SetText(LPCWSTR lpszText);
	void SetTagText(LPCWSTR lpszText);
	void UpdateLayoutInfo(int nHeight);
};

class CUITreeViewCheckbox: public CUIObject
{
public:

	explicit CUITreeViewCheckbox(_TreeViewNode* pNodeData) : m_pNodeData(pNodeData) { UIModifyStyle(UIStyle_Transparent, 0); }
	~CUITreeViewCheckbox() {}

	BEGIN_UI_EVENT_MAP(CUITreeViewCheckbox)
		UI_EVENT_PAINT(OnPaint)
	END_UI_EVENT_MAP()

	void OnPaint(CDCHandle dc, DWORD dwStatus);
	void OnFinalEvent() override { delete this; }

private:

	_TreeViewNode* m_pNodeData;
};

// TreeNode
class CUITreeView;
class CUITreeViewNodeUI: public CUIObject
{
public:

	CUITreeViewNodeUI(CUITreeView* pTree, _TreeViewNode* pNode);
	~CUITreeViewNodeUI();

	UNIVERSAL_LIB_API CRect GetElementRect(UITreeViewNodePart tnn);

	int GetElementWidth(UITreeViewNodePart tnn);
	int GetNodeLevel();
	const _TreeViewNode* GetNodeData() const { return m_pNodeData; }
	CUITreeView* GetTree() const { return m_pTree; }

	HFONT OnUIGetFont();
	void OnPrePaint();
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnLButtonDown(UINT nFlags, POINT point);
	void OnLButtonUp(UINT nFlags, POINT point);
	void OnLButtonDblClk(UINT nFlags, POINT point);
	void OnMouseMove(UINT nFlags, POINT point);
	void OnRButtonDown(UINT nFlags, POINT point);
	void OnRButtonUp(UINT nFlags, POINT point);
	void OnContextMenu(CUIObject *pSender, CPoint ptScreen);
	UINT_PTR OnHitTest(POINT point);
	bool OnSetCursor(UINCHitTestCode ht, int iMouseEvent);
	void OnSize();
	void OnMouseEnter();
	void OnMouseLeave();
	void OnCaptureChanged(CUIObject* pCapture);
	LRESULT OnKeyDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

	void OnFinalEvent() override { delete this; }

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

protected:

	friend class CUITreeView;
	friend struct _TreeViewNode;

private:

	_TreeViewNode* m_pNodeData;
	CUITreeView* m_pTree;
	CUITreeViewCheckbox* m_pCheckbox;
	HBITMAP m_hBitmapButton;
	CPoint m_ptDragLbuttonDown;
	bool m_bAutoDelete;
	bool m_bLButtonDown;
};

class CUITreeView: public CUIControl
{
public:

	UNIVERSAL_LIB_API CUITreeView();
	UNIVERSAL_LIB_API ~CUITreeView();

	UNIVERSAL_LIB_API void SetUsingWebScale(bool bUsingWebScale);

	UNIVERSAL_LIB_API HTREENODE InsertNode(LPCWSTR lpszText, int iImage, HTREENODE hParent, HTREENODE hInsertAfter, DWORD_PTR pdwData = NULL,
											int cChildren = -1);
	HTREENODE InsertNode(IPainter* pBkPainter, bool bAutoDelete, LPCWSTR lpszText, int iImage, HTREENODE hParent,
						HTREENODE hInsertAfter, DWORD_PTR pdwData = NULL, int cChildren = -1);

	UNIVERSAL_LIB_API HTREENODE InsertNode(IPainter* pBkPainter, bool bAutoDelete, int iImage, HTREENODE hParent, HTREENODE hInsertAfter,
											DWORD_PTR pdwData = NULL, int cChildren = -1);

	UNIVERSAL_LIB_API bool RemoveNode(HTREENODE hNode, bool bAutoChangeSelection = false);

	UNIVERSAL_LIB_API void RemoveAllNode();

	UNIVERSAL_LIB_API bool EditNode(HTREENODE hNode);

	UNIVERSAL_LIB_API void EndEditNode(bool bCancel = false);

	UNIVERSAL_LIB_API void EnsureNodeVisible(HTREENODE hNode, bool bScollAnimation = true);

	UNIVERSAL_LIB_API bool ExpandNode(HTREENODE hNode, int nCode = UITVNodeExpand_Expand);

	UNIVERSAL_LIB_API void LayoutIfNeed();

	UITreeViewNodeCheckState GetNodeCheck(HTREENODE hNode);

	HTREENODE GetSelectNode() const { return (HTREENODE)m_pCurrentSel; }

	HIMAGELIST GetImageList() const { return m_hImageList; }

	HTREENODE GetRootNode() const { return m_RootNodes.empty() ? NULL : (HTREENODE)*m_RootNodes.begin(); }

	HTREENODE GetPrevNode(HTREENODE hNode) const { return TestNodeValid(hNode) ? (HTREENODE)((_TreeViewNode*)hNode)->pPrev : NULL; }

	HTREENODE GetNextNode(HTREENODE hNode) const { return TestNodeValid(hNode) ? (HTREENODE)((_TreeViewNode*)hNode)->pNext : NULL; }

	HTREENODE GetParentNode(HTREENODE hNode) const { return TestNodeValid(hNode) ? (HTREENODE)((_TreeViewNode*)hNode)->pParent : NULL; }

	HTREENODE GetChildNode(HTREENODE hNode)  const { return TestNodeValid(hNode) ? (HTREENODE)((_TreeViewNode*)hNode)->pKids : NULL; }

	int GetNodeChildren(HTREENODE hNode) const { return TestNodeValid(hNode) ? ((_TreeViewNode*)hNode)->cChildren : 0; }

	DWORD GetNodeStats(HTREENODE hNode) const { return TestNodeValid(hNode) ? ((_TreeViewNode*)hNode)->dwStatus : 0; }

	DWORD_PTR GetNodeData(HTREENODE hNode) const { return TestNodeValid(hNode) ? ((_TreeViewNode*)hNode)->pdwData : NULL; }

	CUIEdit* GetEditBox() { return m_pEdit; }

	void SetEdit(CUIEdit* pEdit, bool bAuotDelete = false);

	UNIVERSAL_LIB_API std::wstring GetNodeText(HTREENODE hNode);

	std::wstring GetNodeTagText(HTREENODE hNode) const;

	UNIVERSAL_LIB_API CRect GetNodeRect(HTREENODE hNode, bool bCalcIndent = true) const;

	int GetNodeHeight() const { return m_iNodeHeight; }

	int GetNodeLeftPadding() const { return m_iPaddingLeft; }

	int GetNodeRightPadding() const { return m_iPaddingRight; }

	int GetNodeIndent() const { return m_iIndent; }

	UNIVERSAL_LIB_API bool SetNodeText(HTREENODE hNode, LPCWSTR lpszText);

	UNIVERSAL_LIB_API bool SetNodeTagText(HTREENODE hNode, LPCWSTR lpszText);

	bool SetNodeBkPainter(HTREENODE hNode, IPainter* pPainter, bool bAutoDelete);

	UNIVERSAL_LIB_API int SetNodeCheck(HTREENODE hNode, UITreeViewNodeCheckState eCheck);

	UNIVERSAL_LIB_API bool SetNodeImage(HTREENODE hNode, int iImage);

	HIMAGELIST SetImageList(HIMAGELIST imgList);

	UNIVERSAL_LIB_API void SetNodeHeight(int iHeight);

	void SetNodeLeftPadding(int iLeftPadding);

	void SetNodeRightPadding(int iRightPadding);

	void SetNodeIndent(int iIndent);

	void SetNodeChildren(HTREENODE hNode, int cChildren);

	void SetNodeData(HTREENODE hNode, DWORD_PTR dwNodeData);

	void SetNodeStats(HTREENODE hNode, DWORD dwStates);

	UNIVERSAL_LIB_API bool SelectNode(HTREENODE hNode);

	UNIVERSAL_LIB_API void SetOriginalSelectedNode(HTREENODE hNode);

	void SetBkColor(bool bDrawBkColor, COLORREF clr) { m_bDrawBackColor = bDrawBkColor; m_clrBkColor = clr; }

	void SetTagTextColor(COLORREF clr) { m_clrTagTextColor = clr; }

	bool TestNodeValid(HTREENODE hNode) const { return m_setHandle.end() != m_setHandle.find(hNode); }

	//////////////////////////////////////////////////////////////////////////
	void OnDestroy();
	void OnSize();
	void OnPrePaint();
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	bool OnGetChildrenForPaint(std::vector<CUIObject*>* pResultVec, CRect* pRect);
	LRESULT OnGetKidsZOrder(std::vector<CUIObject*>* pResultVec);
	UINT_PTR OnHitTest(POINT point);
	void OnVScroll(UIVScrollEvent eEvent, int iDelta);
	void OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnMouseEnter();
	void OnUIFontChange();

	LRESULT OnSetCursor(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnGetButtonImage(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnGetNodeWidth(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnGetTextColor(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnGetTagTextColor(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnCalcChildrenRect(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnNodeLButtonDown(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnNodeRButtonDown(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnNodeClick(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnNodeRClick(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnNodeDblClick(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnNodeMouseOver(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnNodeMouseLeave(int iEvent, WPARAM wParam, LPARAM lparam, bool& bHandled);
	LRESULT OnNodeBeginDrag(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnNodeContextMenu(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

	LRESULT OnUIEditKeyDown(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
	LRESULT OnUIEditKillFocus(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
	LRESULT OnUIEditTextChange(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	void _Layout();
	void _InternalRemove(_TreeViewNode* pNode);
	CRect _GetNodeTreeRect(_TreeViewNode* pNode);

	// 检查一下节点是否可见
	bool _CheckNodeVisible(_TreeViewNode* pNode);
	// Tree是否拥有焦点
	bool _IsTreeTakingFocus();
	bool _IsChild(_TreeViewNode* pParent, _TreeViewNode* pKid);

	void _ToggleCheckState(_TreeViewNode* pNode);
	void _ResetParentCheck(_TreeViewNode* pNode);
	void _ResetChildrenCheck(_TreeViewNode* pNode);

	void UpdateLayoutInfo();
	Universal::UI::DPI_SCALE_TYPE GetCurrentScale();
	friend class CUITreeViewNodeUI;

	std::list<_TreeViewNode*> m_RootNodes;
	std::vector<_TreeViewNode*> m_vecVisible;
	std::set<HTREENODE> m_setHandle;

	HIMAGELIST m_hImageList;
	CBitmap m_BitmapButton;
	COLORREF m_clrBkColor;
	COLORREF m_clrTagTextColor;

	int m_iNodeHeight;
	int m_iNodeWidth;		// 节点的宽度要以最宽的节点为准
	int m_iPaddingLeft;		// 元素左空白
	int m_iPaddingRight;	// 元素右空白
	int m_iIndent;			// 元素缩进

	_TreeViewNode* m_pCurrentSel;
	CUIEdit*	m_pEdit;

	bool m_bEditAutoDelete;
	bool m_bNeedCalcNodeWidth;
	bool m_bNeedLayout;
	bool m_bDrawBackColor;
	bool m_bFontChanged;							// 文字发生变化
	bool m_bUseWebScale;							// 是否使用WebScale
	Universal::UI::DPI_SCALE_TYPE	m_eScaleType;	// 目前只针对TreeView做处理，其他的Object都先不做处理了，量太大了
};

//////////////////////////////////////////////////////////////////////////
// 下面这些函数很小, 导出会比较亏, 直接inline吧

inline HTREENODE CUITreeView::InsertNode(IPainter* pBkPainter, bool bAutoDelete, LPCWSTR lpszText, int iImage, HTREENODE hParent,
	HTREENODE hInsertAfter, DWORD_PTR pdwData, int cChildren)
{
	HTREENODE hNode = InsertNode(lpszText, iImage, hParent, hInsertAfter, pdwData, cChildren);
	SetNodeBkPainter(hNode, pBkPainter, bAutoDelete);
	return hNode;
}

inline void CUITreeView::SetEdit(CUIEdit* pEdit, bool bAuotDelete)
{
	TuoAssert(!m_pEdit && !UIIsCreated() && L"不要在tree创建后用");
	m_pEdit = pEdit;
	m_bEditAutoDelete = bAuotDelete;
}

inline void CUITreeView::SetNodeData(HTREENODE hNode, DWORD_PTR dwNodeData)
{
	if (!TestNodeValid(hNode))
		return;

	_TreeViewNode* p = (_TreeViewNode*)hNode;
	p->pdwData = dwNodeData;
}

inline void CUITreeView::SetNodeStats(HTREENODE hNode, DWORD dwStates)
{
	if (TestNodeValid(hNode))
		((_TreeViewNode*)hNode)->dwStatus = dwStates;
}

inline bool CUITreeView::SetNodeBkPainter(HTREENODE hNode, IPainter* pBkPainter, bool bAutoDelete)
{
	if (!TestNodeValid(hNode))
		return false;

	_TreeViewNode* p = (_TreeViewNode*)hNode;
	p->pUI->UISetBkPainter(pBkPainter, bAutoDelete);
	return true;
}

inline std::wstring CUITreeView::GetNodeTagText(HTREENODE hNode) const
{
	if (!TestNodeValid(hNode))
		return L"";

	_TreeViewNode* p = (_TreeViewNode*)hNode;
	return p->pszTagText ? p->pszTagText : L"";
}

inline HIMAGELIST CUITreeView::SetImageList(HIMAGELIST imgList)
{
	HIMAGELIST hOldImageList = m_hImageList;
	m_hImageList = imgList;
	return hOldImageList;
}

inline void CUITreeView::SetNodeLeftPadding(int iLeftPadding)
{
	m_iPaddingLeft = iLeftPadding;
	m_bNeedCalcNodeWidth = true;
	m_bNeedLayout = true;

}

inline void CUITreeView::SetNodeRightPadding(int iRightPadding)
{
	m_iPaddingRight = iRightPadding;
	m_bNeedCalcNodeWidth = true;
	m_bNeedLayout = true;

}

inline void CUITreeView::SetNodeIndent(int iIndent)
{
	m_iIndent = iIndent;
	m_bNeedCalcNodeWidth = true;
	m_bNeedLayout = true;
}

inline void CUITreeView::SetNodeChildren(HTREENODE hNode, int cChildren)
{
	if (TestNodeValid(hNode))
	{
		_TreeViewNode* p = (_TreeViewNode*)hNode;
		p->cChildren = cChildren;
		p->pUI->UIRedraw();
	}
}

inline UITreeViewNodeCheckState CUITreeView::GetNodeCheck(HTREENODE hNode)
{
	if (!TestNodeValid(hNode))
		return UITVNodeCheckState_Unchecked;

	if (UITestStyle(UITVStyle_HasCheck))
	{
		_TreeViewNode* p = (_TreeViewNode*)hNode;
		return (UITreeViewNodeCheckState)p->iCheck;
	}

	return UITVNodeCheckState_Unchecked;
}
