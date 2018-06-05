// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "../UICrack.h"
#include "UICoreEvent.h"
#include "UIObject.h"

__interface IUIScrollbar; // NOLINT (4.5.2 lint bug, 识别不了 __interface)
class CUIObject;
class CUIManager;
class CUINonClient
{
public:

	static void ChangeUIMgrRecursively(CUIObject* p, CUIManager* pMgr);

	explicit CUINonClient(CUIObject* pOwner);
	~CUINonClient();

	void SetUIMgr(CUIManager* pUIMgr);
	void ChangeUIManger(CUIManager* pMgr);

	int GetBorderThick() const;

	// 窗口区域裁剪掉非客户区, 变成客户区 (相对于父窗口客户区的坐标)
	CRect ClipRect(const CRect& rc);

	// 对window rect 进行裁剪
	CRect ClipWindowRect(const CRect& rc);

	// 把 rc 四周切掉非客户区
	CRect DeflateNC(const CRect& rc);

	// 画非客户区
	void DoPaint(CDCHandle dc, const CRect& rc, const CPoint &ptOwner);

	// 滚动条是不是可见
	bool IsScrollbarVisible(bool bHorz) const;

	// 客户区坐标系转换为非客户区坐标系
	CPoint ClientToNCCoordinate(const CPoint& pt) const;

	int GetScrollWidth(bool bHorz) const;
	int CalcScrollWidth(bool bHorz) const;

	// 测试一下在什么上面
	UINCHitTestCode NCHitTest(CPoint pt);

	// 重新计算滚动条
	void UpdateScrollbar();

	void OnStyleChanged(DWORD dwOldStyle, DWORD dwNewStyle);
	void OnScrollChanged();
	void OnCreate();
	void OnDestroy();

	IUIScrollbar* GetScrollbar(UIScrollbarType eType);
	void SetScrollbar(IUIScrollbar* pScroll, bool bManaged = true);

private:

	void ReleaseScrollBar(bool bHorz);
	void ShowScrollbar(IUIScrollbar* p, bool bShow);

	CUIObject* m_pOwner;
	IUIScrollbar* m_pHScrollbar;
	IUIScrollbar* m_pVScrollbar;
	bool m_bHScrollbarManaged;
	bool m_bVScrollbarManaged;

	friend class CUIManager;
	friend class CUIObject;
};
