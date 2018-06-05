// Author: ChenJianjun
#pragma once
#include "../../CommonLib/Misc.h"
#include "../../Universal.h"
#include "../Util/ManagedPtr.h"
#include "Painter.h"
#include "Ruler.h"
#include "TooltipText.h"
#include "UICoreDef.h"
#include "UICoreEvent.h"
#include "UIManager.h"
#include "UIProps.h"
#include "UIUtil.h"


//////////////////////////////////////////////////////////////////////////
// public class

struct _UITimer;
struct UIProps;
class IRuler;
class CUIManager;
class CUIHScrollbar;
class CUIVScrollbar;
class CAction;
class CUIEventFilter;
__interface IUIScrollbar; // NOLINT (4.5.2 lint bug, 识别不了 __interface)
class CUIObject: public CUIEventMap
{
public:

	static void CALLBACK _UITimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

public:
	typedef std::function < void(int iEvent, WPARAM wParam, LPARAM lParam) > EventHandler;

	UNIVERSAL_LIB_API CUIObject();
	UNIVERSAL_LIB_API virtual ~CUIObject();

	UNIVERSAL_LIB_API void UIAddEventListener(CUIEventFilter* pFilter);
	UNIVERSAL_LIB_API void UIRemoveEventListener(CUIEventFilter* pFilter);

	// 产生一个同步事件
	UNIVERSAL_LIB_API LRESULT UIFireEvent(int iEvent, WPARAM wParam = NULL, LPARAM lParam = NULL, bool* pProcessed = NULL);

	// 产生一个异步一个事件
	UNIVERSAL_LIB_API bool UIFireAsyncEvent(int iEvent, WPARAM wParam = NULL, LPARAM lParam = NULL);

	UNIVERSAL_LIB_API virtual void OnFinalEvent();

	const DEvent* UIGetCurrentEvent() const { UICHECK_OBJECT_VALID(); return m_pCurrentEvent; }

	// 查询状态
	bool UIIsWantArrow() { return UIFireEvent(UIEvent_WantArrow) != FALSE; }
	bool UIIsWantTab() { return UIFireEvent(UIEvent_WantTab) != FALSE; }
	bool UIIsWantEnter() { return UIFireEvent(UIEvent_WantEnter) != FALSE; }
	bool UIIsWantEscape() { return UIFireEvent(UIEvent_WantEscape) != FALSE; }
	bool UIIsTransparent() const { return UITestStyle(UIStyle_Transparent); }
	bool UIIsWantFoucs() const { return !UITestStyle(UIStyle_NoFocus); }
	bool UIIsTabStop() const { return UITestStyle(UIStyle_TabStop); }
	bool UIIsRoot() const { return UITestStyle(UIStyle_UIRoot); }
	bool UIIsVisible() const { return UIIsCreated() && UITestStyle(UIStyle_Visible); }
	UNIVERSAL_LIB_API bool UIIsVisibleReally() const;
#ifdef __UIA_ADJUST__
	bool UIIsUIAHidden() const { return UITestStyle(UIStyle_UIAHidden); }
#endif
	bool UIIsPressed() const { return UITestStatus(UIStatus_Pressed); }
	UNIVERSAL_LIB_API bool UIIsDisabled() const;
	bool UIIsFocused() const { return UITestStatus(UIStatus_Focus); }
	bool UIIsFocusedChild() const { return UITestStatus(UIStatus_FocusChild); }
	bool UIIsOwnFocus() const { return UITestStatus(UIStatus_FocusChild | UIStatus_Focus); }
	bool UIIsHovered() const { return UITestStatus(UIStatus_Hover); }
	bool UIIsHoveredChild() const { return UITestStatus(UIStatus_HoverChild); }
	UNIVERSAL_LIB_API bool UIIsScrollbarVisible(bool bHorz) const;
	bool UIIsCaptured() const { return UITestStatus(UIStatus_Capture); }
	bool UIIsCapturedChild() const { return UITestStatus(UIStatus_CaptureChild); }
	bool UIIsAnimationEnabled() const;
	bool UIIsCreated() const { return (m_dwStatus & UIStatus_Created) != 0 && (m_dwStatus & UIStatus_Destroying) == 0; }
	bool UIIsNoRedraw() const { return (m_dwStatus & UIStatus_NoRedraw) != 0; }
	bool UIIsDeleteFinally() const { return UITestStatus(UIStatus_DeleteFinally); }

	// 查询一下是不是子窗口或孙窗口...
	UNIVERSAL_LIB_API bool UIIsChild(const CUIObject* pChild) const;

	// Disable窗口
	UNIVERSAL_LIB_API void UIDisable(bool bDisable = true);
	void UIEnable(bool bEnable = true) { UIDisable(!bEnable); }

	void UIHilight(bool bHilight = true) { bHilight ? ModifyStatus(UIStatus_Hilight, 0) : ModifyStatus(0, UIStatus_Hilight); }
	
	void UIDeleteFinally(bool bDelete = true) { bDelete ? ModifyStatus(UIStatus_DeleteFinally, 0) : ModifyStatus(0, UIStatus_DeleteFinally); }

	// 显示窗口
	bool UIShow(bool bShow = true);

	// 窗口在父窗口的位置转为Window坐标
	CRect UIRectToWindow(const CRect& rc) const;

	// 从Client坐标转为Window坐标
	UNIVERSAL_LIB_API CRect UIClientToWindow(const CRect& rc) const;
	UNIVERSAL_LIB_API CPoint UIClientToWindow(const CPoint& pt) const;

	// 从Window坐标转为Client坐标
	UNIVERSAL_LIB_API CRect UIWindowToClient(const CRect& rc) const;
	UNIVERSAL_LIB_API CPoint UIWindowToClient(const CPoint& pt) const;

	// window窗口坐标转换为Screen坐标
	UNIVERSAL_LIB_API CRect UIWindowToScreen(const CRect& rc) const;
	UNIVERSAL_LIB_API CPoint UIWindowToScreen(const CPoint& pt) const;

	// Screen坐标转换为window窗口坐标
	UNIVERSAL_LIB_API CRect UIScreenToWindow(const CRect& rc) const;
	UNIVERSAL_LIB_API CPoint UIScreenToWindow(const CPoint& pt) const;

	// Client坐标转换为Screen坐标
	inline CRect UIClientToScreen(const CRect& rc) const { return UIWindowToScreen(UIClientToWindow(rc)); }
	inline CPoint UIClientToScreen(const CPoint& pt) const { return UIWindowToScreen(UIClientToWindow(pt)); }

	// Screen坐标转换为Client坐标
	inline CRect UIScreenToClient(const CRect& rc) const { return UIWindowToClient(UIScreenToWindow(rc)); }
	inline CPoint UIScreenToClient(const CPoint& pt) const { return UIWindowToClient(UIScreenToWindow(pt)); }

	// 返回窗口的私有数据
	DWORD_PTR UIGetData() const { UICHECK_OBJECT_VALID(); return m_pdwData; }

	// 返回自定义布局的信息
	bool UIHasProps() const { return m_pUIProps != NULL; }
	UIProps *UIGetProps()
	{
		if (m_pUIProps == NULL)
			m_pUIProps = new UIProps;
		return m_pUIProps;
	}

	// 返回客户区域,以(0, 0)开始
	UNIVERSAL_LIB_API CRect UIGetClientRect() const;

	UNIVERSAL_LIB_API CRect UIGetPaintRect() const;

	// 可见的客户区
	UNIVERSAL_LIB_API CRect UIGetVisibleClientRect() const;

	// 返回相对于父窗口的窗口区域坐标
	UNIVERSAL_LIB_API CRect UIGetRectNonClientCliped() const;
	const CRect& UIGetRect() const { UICHECK_OBJECT_VALID(); return m_rc; }
	CPoint UIGetPos() const { UICHECK_OBJECT_VALID(); return m_rc.TopLeft(); }
	CSize UIGetSize() const { UICHECK_OBJECT_VALID(); return CSize(UIGetWidth(), UIGetHeight()); }
	int UIGetWidth() const { UICHECK_OBJECT_VALID(); return m_rc.Width(); }
	int UIGetHeight() const { UICHECK_OBJECT_VALID(); return m_rc.Height(); }

	// 返回相对于Root的坐标
	UNIVERSAL_LIB_API CRect UIGetWindowRect() const;

	// 返回屏幕坐标
	inline CRect UIGetScreenRect() const { return UIWindowToScreen(UIGetWindowRect()); }

	// 返回窗口的状态
	DWORD UIGetStatus() const { UICHECK_OBJECT_VALID(); return m_dwStatus; }

	// 返回窗口的样式
	DWORD UIGetStyle() const { UICHECK_OBJECT_VALID(); return m_dwStyle; }

	// 返回窗口文字,如果没有设置过,则返回空字符串
	std::wstring UIGetText() const;

	// 返回窗口文字的长度
	UINT UIGetTextLength() { return UIFireEvent(UIEvent_GetTextLength); }

	std::wstring UIGetTooltipText();

	UNIVERSAL_LIB_API IUIScrollbar* UIGetScrollbar(UIScrollbarType eType);

	// 这个 bManaged 参数真的有必要么？ yangqi, 20160108
	UNIVERSAL_LIB_API virtual void UISetScrollbar(IUIScrollbar* pScroll, bool bManaged);

	// 返回窗口ID
	virtual LPCWSTR UIGetObjectID() const { UICHECK_OBJECT_VALID(); return L""; }

	// 往上找，直到找到一个ObjectID非空的parent，然后返回这个parent的object id
	LPCWSTR UIGetReferID() const;

	// 返回所属的windows窗口
	HWND UIGetHWND() const;

	// 返回窗口的类型
	UIObjectType UIGetObjectType() { UICHECK_OBJECT_VALID(); return (UIObjectType)UIFireEvent(UIEvent_GetObjectType); }
	std::wstring UIGetObjectTypeStr();
	UIObjectType UIGetOrigObjectType() { UICHECK_OBJECT_VALID(); return (UIObjectType)UIFireEvent(UIEvent_GetOrigObjectType); }
	std::wstring UIGetOrigObjectTypeStr();

	// 返回窗口的默认字体
	HFONT UIGetFont() { UICHECK_OBJECT_VALID(); return (HFONT)UIFireEvent(UIEvent_GetFont); }

	// 获取文字颜色
	COLORREF UIGetTextColor() { UICHECK_OBJECT_VALID(); return (COLORREF)UIFireEvent(UIEvent_GetTextColor); }

	// 获取窗口disable状态下的字体颜色
	COLORREF UIGetDisabledTextColor() { UICHECK_OBJECT_VALID();	return (COLORREF)UIFireEvent(UIEvent_GetDisabledTextColor); }

	// 返回窗口的子窗口个数
	int UIGetChildCount() const { UICHECK_OBJECT_VALID(); return m_pvecKids ? (int)m_pvecKids->size() : 0; }

	// 返回自己在父窗口中的顺序位置, 也就是addChild的顺序
	int UIGetIndex();

	// 返回子窗口的顺序位置, 这个函数和getPos()一样
	UNIVERSAL_LIB_API int UIGetChildIndex(CUIObject* pChild) const;

	// 获取滚动条的位置
	int UIGetScrollPos(bool bHorz) const;

	// 当滚动条在进行动画的时候通过UIGetScrollPos获取到的是动画过程中的一个
	// 中间位置，通过这个api可以获得动画完成后滚动条会在什么位置。
	int UIGetScrollPosWhenAniFinish(bool bHorz) const;

	// 获取边框的大小
	UNIVERSAL_LIB_API int UIGetBorderThick() const;

	// 获取滚动条信息
	ScrollbarInfo UIGetScrollInfo(bool bHorz) const;

	// 返回第idx个子窗口,如果没有的话就返回空
	CUIObject* UIGetChild(int idx) const;

	// 返回所有窗口所在的窗口
	CRect UIGetChildrenRect();

	// 返回被Capture的窗口,
	CUIObject* UIGetCapture();

	// 返回父窗口
	CUIObject* UIGetParent() const { UICHECK_OBJECT_VALID(); return m_pParent; }

	// 返回最顶层窗口
	CUIRoot* UIGetRoot() const;

	IPainter* UIGetPainter() const { UICHECK_OBJECT_VALID(); return m_pUIInfo ? m_pUIInfo->pPainter.get() : NULL; }

	IPainter* UIGetBkPainter() const { UICHECK_OBJECT_VALID(); return m_pUIInfo ? m_pUIInfo->pBkPainter.get() : NULL; }

	IRuler* UIGetRuler() const { UICHECK_OBJECT_VALID(); return m_pUIInfo ? m_pUIInfo->pRuler.get() : NULL; }

	CAction* UIGetPainterAction() const { UICHECK_OBJECT_VALID(); return m_pUIInfo ? m_pUIInfo->pPaintAction : NULL; }

	// 自测量窗口需要的窗户区大小
	UNIVERSAL_LIB_API SIZE UIMeasure();

	bool UIShowScrollbar(bool bHorz, bool bShow);

	UNIVERSAL_LIB_API void UISetParent(CUIObject* pParent);

	// 设置窗口私有数据, 这会触发一个onDataChange的回调
	void UISetData(DWORD_PTR pdwData) { UICHECK_OBJECT_VALID(); m_pdwData = pdwData; }

	// 设置窗口的区域, 这会触发onSize回调
	UNIVERSAL_LIB_API void UISetPos(const CRect& rc);

	void UISetPos(const CPoint& pt) { UISetPos(pt.x, pt.y); }
	
	void UISetPos(int x, int y, int cx, int cy) { UISetPos(CRect(x, y, x + cx, y + cy)); }
	
	void UISetPos(int x, int y) { UISetPos(x, y, UIGetWidth(), UIGetHeight()); }
	
	void UISetSize(int cx, int cy) { UISetPos(m_rc.left, m_rc.top, cx, cy); }
	void UISetSize(const CSize& size) { UISetPos(m_rc.left, m_rc.top, size.cx, size.cy); }

	void UISetTooltipText(const std::wstring& strText) { UISetTooltip(new CFixedTooltipText(strText), true); }

	UNIVERSAL_LIB_API void UISetTooltip(CTooltipText* pTooltip, bool bManaged = false);

	// 设置Painter
	UNIVERSAL_LIB_API void UISetPainter(IPainter* pPainter, bool bAutoDelete = false);

	// 设置BkPainter
	UNIVERSAL_LIB_API void UISetBkPainter(IPainter* pBkPainter, bool bAutoDelete = false);

	UNIVERSAL_LIB_API void UISetRuler(IRuler* pRuler, bool bManaged);

	UNIVERSAL_LIB_API void UISetPainterAction(CAction* pAction);

	// 使窗口获得焦点, 但如果窗口没有UIStyle_WantInput属性的话, 它就会调用父窗口的UISetFocus, 直到找到为止
	// 一旦窗口获得焦点, 窗口就拥有了DWS_FOCUS属性
	CUIObject* UISetFocus();

	// 是否绘制窗口
	void UISetRedraw(bool bRedraw);

	// 使窗口失去焦点
	void UIKillFocus();

	// 启动一个Timer
	UNIVERSAL_LIB_API void UISetTimer(UINT_PTR nIDEvent, UINT uElapse, LPARAM lParam = NULL);

	// 停止一个Timer
	UNIVERSAL_LIB_API void UIKillTimer(UINT_PTR nIDEvent);

	// 使窗口获取Capture, 这个函数会直接设置顶层窗口的m_pCapture, 把所有的鼠标/键盘事件都导向这个窗口
	CUIObject* UISetCapture();

	// 释放Capture
	void UIReleaseCapture();

	// 创建窗口
	UNIVERSAL_LIB_API bool UICreate(CUIObject* pParent, DWORD dwStyle = 0);
	UNIVERSAL_LIB_API bool UICreate(CUIObject* pParent, int x, int y, int cx, int cy, DWORD dwStyle = 0);
	bool UICreate(CUIObject* pParent, const RECT& rc, DWORD dwStyle = 0);
	UNIVERSAL_LIB_API void UIDestroy();

	// 移除所有子窗口
	UNIVERSAL_LIB_API void UIRemoveAllChildren();

	UNIVERSAL_LIB_API void UIModifyStyle(DWORD dwStyleAdd, DWORD dwStyleRemove, bool bNotify = true);
	bool UIAddOrRemoveStyle(bool bAdd, DWORD dwStyle, bool bNotify = true);

	// 重绘窗口区域
	UNIVERSAL_LIB_API void UIRedraw();

	// print Object的区域
	UNIVERSAL_LIB_API void UIPrint(HDC hDC);

	// 测试一下是在客户区还是在非客户区
	UINCHitTestCode UINCHitTest(const CPoint& pt);

	// 测试一下鼠标在窗口的什么上面
	UINT_PTR UIHitTest(const CPoint &pt) { UICHECK_OBJECT_VALID(); return (UINT_PTR)UIFireEvent(UIEvent_HitTest, pt.x, pt.y); }

	// 动态调整一下Scrollbar
	UNIVERSAL_LIB_API void UIUpdateScrollbar();

	CUIManager* UIGetMgr() { UICHECK_OBJECT_VALID(); return m_pUIMgr; }

	// 设置滚动条的位置, 返回值是新的滚动条位置
	int UISetScrollPos(bool bHorz, int iNewPos) { return DoSetScrollPos(bHorz, iNewPos, UIScrollMode_Smooth); }
	int UISetScrollPosQuick(bool bHorz, int iNewPos) { return DoSetScrollPos(bHorz, iNewPos, UIScrollMode_Quick); }
	int UISetScrollPosNow(bool bHorz, int iNewPos) { return DoSetScrollPos(bHorz, iNewPos, UIScrollMode_Now); }
	UNIVERSAL_LIB_API void UISetScrollInfo(bool bHorz, const ScrollbarInfo& sbi);

	// 垂直滚动
	void UIScrollTop();
	void UIScrollBottom();
	void UIScrollUp();
	void UIScrollDown();
	void UIScrollPageUp();
	void UIScrollPageDown();

	// 水平滚动
	void UIScrollLeftEdge();
	void UIScrollRightEdge();
	void UIScrollLeft();
	void UIScrollRight();
	void UIScrollPageLeft();
	void UIScrollPageRight();

	// 测试一下是否存在这个窗口属性
	bool UITestStatus(DWORD dwState) const
	{
		UICHECK_OBJECT_VALID();
		UIASSERT((m_pCurrentEvent == NULL || (m_pCurrentEvent->iEvent != UIEvent_PaintBackgound && m_pCurrentEvent->iEvent != UIEvent_PaintTransition))
				&& L"绘制时请使用参数的状态值，不要使用 UITestStatus 以及 UIIsXXX。");
		UIASSERT((m_pCurrentEvent == NULL || m_pCurrentEvent->iEvent != UIEvent_Paint)
				&& L"绘制时请使用参数的状态值，不要使用 UITestStatus 以及 UIIsXXX。若方便，请将需要状态值的绘制代码放到 UIEvent_PaintBackgound 中去。");
		return (m_dwStatus & dwState) != 0;
	}
	void UIModifyStatus(DWORD dwAdd, DWORD dwRemove, bool bNotify = true);
	bool UIAddOrRemoveStatus(bool bAdd, DWORD dwStatus, bool bNotify = true);
	bool UITestStyle(DWORD dwStyle) const { UICHECK_OBJECT_VALID(); return (m_dwStyle & dwStyle) != 0; }

	// 测试一下窗口和这个区域有没有重叠(相对于parent的坐标)
	bool UITestOverlap(const CUIObject* pChild, const CRect& rc) const;

	bool UITestKeyDown(int vKey) { UICHECK_OBJECT_VALID(); return IS_KEY_DOWN(vKey) != 0; }

	UNIVERSAL_LIB_API void UIRunAction(CDecorator* pAction, int iGroupID = 0, IContextCallback* pContextDestroyCallback = NULL, bool bRestart = true);
	UNIVERSAL_LIB_API void UIStopAction(int iGroupID = 0);

	void UIAddObjectRef(int iDelta) { UIGetMgr()->AddObjectRef(this, iDelta); }
	bool UITestObjectRef(int iDelta) { return UIGetMgr()->TestObjectRef(this, iDelta); }

	BYTE UIGetAlpha() const { return m_cAlpha; }
	void UISetAlpha(BYTE cAlpha) { m_cAlpha = cAlpha; }
	bool UIIsUsingAlpha() const { return m_cAlpha != 0xff; }
	bool UIHasPaintAction() const { return m_pUIInfo != NULL && m_pUIInfo->pPaintAction != NULL; }

protected:

	DWORD_PTR GetRealTimerIDByTimerID(DWORD_PTR nIDTimer);

private:

	struct _UIScrollbarInfo;

	UNIVERSAL_LIB_API LRESULT DefEventProc(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

	void RemoveChild(int nIndex);
	bool AddChild(CUIObject* pControl);
	bool DoCreate(CUIObject* pParent, bool bAddKids);
	void DoPaint(CDCHandle dc, const CRect &rcInvalidate, const CPoint &ptObject);
	UNIVERSAL_LIB_API int DoSetScrollPos(bool bHorz, int iNewPos, UIScrollMode eMode);
	void InitScrollbar();
	void InitUIInfo();
	void KillAllTimers();
	UNIVERSAL_LIB_API void ModifyStatus(DWORD dwAdd, DWORD dwRemove, bool bNotify = true);
	void SetUIMgr(CUIManager* pUIMgr);

	LRESULT OnGetChildrenForPaint(int iEvent, WPARAM wParam, LPARAM lParam);
	LRESULT OnCalcChildrenRect(int iEvent, WPARAM wParam, LPARAM lParam);
	LRESULT OnGetTooltipText(int iEvent, WPARAM wParam, LPARAM lParam);
	UINCHitTestCode OnNCHitTest(const CPoint& pt);
	void OnVScroll(WPARAM wParam, LPARAM lParam);
	void OnHScroll(WPARAM wParam, LPARAM lParam);
	bool OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnSmoothScroll(float fProgress, LPARAM lParam);

	void LayoutChildren();
	void LayoutChild(CUIObject *pChild);

#ifdef _DEBUG
	virtual bool _isUIRootDebug_() { return false; }
protected:
	DWORD m_dwValidSignature___ = 0x19860403;
#endif // #ifdef _DEBUG

private:

	friend const void* PropsGetter(const CUIObject*);
	friend void PropsSetter(CUIObject*, void*);

	friend class CUIRoot;
	friend class CUIControl;
	friend class CUINonClient;
	friend class CUIManager;
	friend class CUIHostRoot;
	friend class CTrackFinalEvent;

	struct _UIInfo
	{
		se::managed_ptr<IPainter> pPainter;
		se::managed_ptr<IPainter> pBkPainter;
		se::managed_ptr<IRuler> pRuler;
		se::managed_ptr<CTooltipText> pTooltip;
		CAction* pPaintAction;

		_UIInfo() { pPaintAction = NULL; }
	};

	std::list<CUIEventFilter*> *m_pEventListenerList = nullptr;

	UIProps *m_pUIProps = nullptr;

	struct _UIScrollbarInfo
	{
		ScrollbarInfo sbiV;
		ScrollbarInfo sbiH;
	};

	// 窗口所在的位置,这个是以m_hWndOwner的ClientRect的左上角为(0,0)
	CRect m_rc;

	// 窗口可以存储一个DWORD*的私有数据
	DWORD_PTR m_pdwData = 0;

	// 窗口的状态,
	DWORD m_dwStatus = 0;

	// 窗口样式
	DWORD m_dwStyle = UIStyle_Visible;

	// 存储一个DUI窗口实例所必需的数据
	CUIManager* m_pUIMgr = nullptr;

	// 父窗口
	CUIObject* m_pParent = nullptr;

	// 非客户区(Non-Client)信息
	CUINonClient* m_pNC = nullptr;

	// 所有一级子窗口
	std::vector<CUIObject*>* m_pvecKids = nullptr;

	// 属于这个窗口的Timer列表, 在对象消毁的时候一定要都Kill,不然会崩溃的
	std::vector<_UITimer*>* m_pvecTimer = nullptr;

	// 自定义绘制界面
	_UIInfo* m_pUIInfo = nullptr;

	// 滚动条
	_UIScrollbarInfo* m_pSBI = nullptr;

	DEvent* m_pCurrentEvent = nullptr;

	bool m_bHoldFinalEvent = false;

	BYTE m_cAlpha = 0xff;
};

//////////////////////////////////////////////////////////////////////////
// 下面这些函数很小, 导出会比较亏, 直接inline吧

inline int CUIObject::UIGetScrollPos(bool bHorz) const
{
	UICHECK_OBJECT_VALID();
	if (m_pSBI)
		return bHorz ? m_pSBI->sbiH.iPos : m_pSBI->sbiV.iPos;

	return 0;
}

inline int CUIObject::UIGetScrollPosWhenAniFinish(bool bHorz) const
{
	UICHECK_OBJECT_VALID();
	if (m_pSBI)
		return bHorz ? m_pSBI->sbiH.iDestPos : m_pSBI->sbiV.iDestPos;

	return 0;
}

inline ScrollbarInfo CUIObject::UIGetScrollInfo(bool bHorz) const
{
	UICHECK_OBJECT_VALID();
	ScrollbarInfo sbi = { 0 };
	if (!m_pSBI)
		return sbi;

	return bHorz ? m_pSBI->sbiH : m_pSBI->sbiV;
}

inline CUIObject* CUIObject::UIGetCapture()
{
	UICHECK_OBJECT_VALID();
	if (!m_pUIMgr)
		return NULL;
	return m_pUIMgr->GetCapture();
}

inline CUIRoot* CUIObject::UIGetRoot() const
{
	return m_pUIMgr ? m_pUIMgr->GetUIRoot() : NULL;
}

inline bool CUIObject::UIAddOrRemoveStyle(bool bAdd, DWORD dwStyle, bool bNotify/* = true*/)
{
	DWORD dwOldStyle = m_dwStyle;
	if (bAdd)
		UIModifyStyle(dwStyle, 0, bNotify);
	else
		UIModifyStyle(0, dwStyle, bNotify);

	return m_dwStyle != dwOldStyle;
}

inline bool CUIObject::UIShowScrollbar(bool bHorz, bool bShow)
{
	UICHECK_OBJECT_VALID();
	if (bHorz)
		UIAddOrRemoveStyle(bShow, UIStyle_HScroll);
	else if (!bHorz)
		UIAddOrRemoveStyle(bShow, UIStyle_VScroll);

	return true;
}

inline HWND CUIObject::UIGetHWND() const
{
	UICHECK_OBJECT_VALID();
	if (m_pUIMgr)
		return m_pUIMgr->GetHWND();

	return NULL;
}

inline std::wstring CUIObject::UIGetObjectTypeStr()
{
	UICHECK_OBJECT_VALID();
	std::wstring str;
	UIFireEvent(UIEvent_GetObjectTypeStr, (WPARAM)&str, NULL);
	return str;
}

inline std::wstring CUIObject::UIGetOrigObjectTypeStr()
{
	UICHECK_OBJECT_VALID();
	std::wstring str;
	UIFireEvent(UIEvent_GetOrigObjectTypeStr, (WPARAM)&str, NULL);
	return str;
}

inline CUIObject* CUIObject::UIGetChild(int idx) const
{
	UICHECK_OBJECT_VALID();
	if (NULL == m_pvecKids || idx >= UIGetChildCount())
		return NULL;

	return (*m_pvecKids)[idx];
}

inline CRect CUIObject::UIGetChildrenRect()
{
	UICHECK_OBJECT_VALID();

	CRect rc;
	UIFireEvent(UIEvent_CalcChildrenRect, (WPARAM)&rc);
	return rc;
}

inline int CUIObject::UIGetIndex()
{
	UICHECK_OBJECT_VALID();
	if (!UIGetParent())
		return -1;

	return UIGetParent()->UIGetChildIndex(this);
}

inline CUIObject* CUIObject::UISetCapture()
{
	UICHECK_OBJECT_VALID();
	UIASSERT(m_pUIMgr);
	if (!m_pUIMgr)
		return NULL;

	return m_pUIMgr->SetCapture(this);
}

inline void CUIObject::UIReleaseCapture()
{
	UICHECK_OBJECT_VALID();
	UIASSERT(m_pUIMgr);
	if (m_pUIMgr)
		m_pUIMgr->SetCapture(NULL);
}

inline bool CUIObject::UICreate(CUIObject* pParent, const RECT& rc, DWORD dwStyle)
{
	return UICreate(pParent, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, dwStyle);
}

inline CUIObject* CUIObject::UISetFocus()
{
	UICHECK_OBJECT_VALID();
	return UIGetMgr()->SetFocus(this);
}

inline void CUIObject::UIKillFocus()
{
	UICHECK_OBJECT_VALID();
	if (m_pUIMgr && this == m_pUIMgr->GetFocus())
		m_pUIMgr->  SetFocus(NULL);
}

inline bool CUIObject::UIIsAnimationEnabled() const
{
	UICHECK_OBJECT_VALID();
	ANIMATIONINFO ani;
	::SystemParametersInfo(SPI_GETANIMATION, 0, &ani, 0);
	return ani.iMinAnimate != 0;
}

inline bool CUIObject::UIShow(bool bShow/* = true */)
{
	UICHECK_OBJECT_VALID();
	if (UITestStyle(UIStyle_Visible) != bShow)
	{
		UIAddOrRemoveStyle(bShow, UIStyle_Visible);
		return !UIIsVisible();
	}

	return UIIsVisible();
}

inline void CUIObject::UISetRedraw(bool bRedraw)
{
	UICHECK_OBJECT_VALID();
	if (bRedraw)
	{
		ModifyStatus(0, UIStatus_NoRedraw);
		UIRedraw();
	}
	else
	{
		ModifyStatus(UIStatus_NoRedraw, 0);
	}
}

inline std::wstring CUIObject::UIGetText() const
{
	std::wstring strText;
	((CUIObject*)this)->UIFireEvent(UIEvent_GetText, (WPARAM)&strText);
	return strText;
}

inline LPCWSTR CUIObject::UIGetReferID() const
{
	CUIObject* pParent = UIGetParent();
	while (pParent)
	{
		LPCWSTR lpszReferID = pParent->UIGetObjectID();
		if (lpszReferID && lpszReferID[0])
			return lpszReferID;

		pParent = pParent->UIGetParent();
	}

	return L"";
}

inline void CUIObject::UIModifyStatus(DWORD dwAdd, DWORD dwRemove, bool bNotify/* = true*/)
{
	UIASSERT((dwAdd & 0xFFFF) == 0 && (dwRemove & 0xFFFF) == 0);
	ModifyStatus(dwAdd & 0xFFFF0000, dwRemove & 0xFFFF0000, bNotify);
}

inline bool CUIObject::UIAddOrRemoveStatus(bool bAdd, DWORD dwStatus, bool bNotify/* = true*/)
{
	DWORD dwOldStatus = m_dwStatus;
	if (bAdd)
		UIModifyStatus(dwStatus, 0, bNotify);
	else
		UIModifyStatus(0, dwStatus, bNotify);

	return m_dwStatus != dwOldStatus;
}

inline bool CUIObject::UITestOverlap(const CUIObject* pChild, const CRect& rc) const
{
	UICHECK_OBJECT_VALID();
	if (rc.left >= pChild->m_rc.right || rc.top >= pChild->m_rc.bottom || rc.right <= pChild->m_rc.left || rc.bottom <= pChild->m_rc.top)
		return false;
	return true;
}

inline void CUIObject::UIScrollTop()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_VScroll, VScroll_Top, m_pSBI->sbiV.iPos - m_pSBI->sbiV.iMin);
}

inline void CUIObject::UIScrollBottom()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_VScroll, VScroll_Bottom, m_pSBI->sbiV.iMax - m_pSBI->sbiV.iPos);
}

inline void CUIObject::UIScrollUp()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_VScroll, VScroll_Up, 10);
}

inline void CUIObject::UIScrollDown()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_VScroll, VScroll_Down, 10);
}

inline void CUIObject::UIScrollPageUp()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_VScroll, VScroll_PageUp, m_pSBI->sbiV.iPage);
}

inline void CUIObject::UIScrollPageDown()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_VScroll, VScroll_PageDown, m_pSBI->sbiV.iPage);
}

inline void CUIObject::UIScrollLeftEdge()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_HScroll, HScroll_LeftEdge, m_pSBI->sbiH.iPos - m_pSBI->sbiH.iMin);
}

inline void CUIObject::UIScrollRightEdge()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_HScroll, HScroll_RightEdge, m_pSBI->sbiH.iMax - m_pSBI->sbiH.iPos);
}

inline void CUIObject::UIScrollLeft()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_HScroll, HScroll_Left, 10);
}

inline void CUIObject::UIScrollRight()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_HScroll, HScroll_Right, 10);
}

inline void CUIObject::UIScrollPageLeft()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_HScroll, HScroll_PageLeft, m_pSBI->sbiH.iPage);
}

inline void CUIObject::UIScrollPageRight()
{
	if (m_pSBI)
		UIFireEvent(UIEvent_HScroll, HScroll_PageRight, m_pSBI->sbiH.iPage);
}
