// Author: ChenJianjun
#pragma once
#include "../../CommonLib/UI/Bitmap.h"
#include "../../CommonLib/UI/DC.h"
#include "../../CommonLib/UI/UI.h"
#include "../Util/FPSCounter.h"
#include "Action/ActionManager.h"
#include "UICoreDef.h"
#include "UIUtil.h"


struct ObjHandler
{
	CUIObject* pObj;
	CTrackFinalEvent track;
};

class CUIObject;
class CUIModalHost;
class CPaintContext;
class CUIManager: public CUIEventMap
{
public:

	explicit CUIManager(CUIRoot* pUIRoot);
	UNIVERSAL_LIB_API ~CUIManager();

	UNIVERSAL_LIB_API void Print(CDCHandle dc, const CRect& rc);
	UNIVERSAL_LIB_API void Print(CDCHandle dc, CUIObject* pObj);

	UNIVERSAL_LIB_API BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0);
	UNIVERSAL_LIB_API bool AddMessageHook(IHookMessage* pMsgFilter);
	UNIVERSAL_LIB_API bool RemoveMessageHook(IHookMessage* pMsgFilter);
	UNIVERSAL_LIB_API void AddObjectRef(const CUIObject* p, int iDelta);
	UNIVERSAL_LIB_API bool TestObjectRef(const CUIObject* p, int iDelta);
	UNIVERSAL_LIB_API void RemoveObject(const CUIObject* p);
	UNIVERSAL_LIB_API void RemoveStatusChangeNotification(CUIObject* pObject);
	UNIVERSAL_LIB_API void AddRootChangeNotifyObj(CUIObject* pObj);
	UNIVERSAL_LIB_API void RemoveRootChangeNotifyObj(CUIObject* pObj);

	void AddRootChangeNotifyObj(std::vector<CUIObject*>& vecObj);
	void RemoveRootChangeNotifyObj(std::vector<CUIObject*>& vecObj);

	bool IsUIValid() { return m_hWnd && ::IsWindow(m_hWnd); }
	bool IsUIVisible() { return IsUIValid() && ::IsWindowVisible(m_hWnd); }
	bool IsFoucsRectActivate() { return m_bFoucsRectActivate; }

	bool IsCreated() const { return m_bCreated; }
	bool IsLayeredWindow() { return (::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) != 0; }
	UNIVERSAL_LIB_API void SetLayeredWindowAlpha(BYTE iAlpha);
	UNIVERSAL_LIB_API void NotifyWhenWindowPosChanged(CUIObject* p, bool bRemove = false); // 添加窗口大小, 位置, 滚动条等变化的通知
	UNIVERSAL_LIB_API CUIObject* FindObject(POINT pt, DWORD dwStatus = 0);
	UNIVERSAL_LIB_API CUIObject* FindObject(LPCWSTR lpszObjectID);

	DWORD_PTR GetData() const { return m_pdwData; }
	CActionManager* GetActionManager() { return &m_ActionManager; }
	ICommandHandler* GetCommandHandler() { return m_pCommandHandler; }
	HFONT GetDefaultFont();
	COLORREF GetDefaultColor() { return m_clrDefault; }
	COLORREF GetDefaultDisabledColor() const { return m_clrDefaultGray; }
	HWND GetHWND() const { TuoAssert((!IsCreated() && m_hWnd == NULL) || (IsCreated() && m_hWnd != NULL)); return m_hWnd; }
	CUIRoot* GetUIRoot() { return m_pUIRoot; }
	CUIObject* GetPressed() const { return m_pCurrentPressed; }
	CUIObject* GetHover() const { return m_pCurrentHover; }
	CUIObject* GetMouseOver() const { return m_pCurrentMouseOver; }
	CUIObject* GetFocus() const { return m_pCurrentFocus; }
	CUIObject* GetCapture() const { return m_pCurrentCapture; }
	UNIVERSAL_LIB_API CUIObject* GetPrevTabStop(CUIObject* pCurrent);
	UNIVERSAL_LIB_API CUIObject* GetNextTabStop(CUIObject* pCurrent);

	void SetData(DWORD_PTR pData) { m_pdwData = pData; }
	void SetCommandHandler(ICommandHandler* pHandler) { m_pCommandHandler = pHandler; }
	void SetFocusRectActivate() { m_bFoucsRectActivate = true; }
	UNIVERSAL_LIB_API void SetDefaultFont(HFONT hFont);
	UNIVERSAL_LIB_API void SetDefaultColor(COLORREF clr);
	UNIVERSAL_LIB_API void SetDefaultDisabledColor(COLORREF clr);
	UNIVERSAL_LIB_API CUIObject* SetPressed(CUIObject* pActive);
	UNIVERSAL_LIB_API CUIObject* SetHover(CUIObject* pHover);
	UNIVERSAL_LIB_API CUIObject* SetMouseOver(CUIObject* pMouseOver);
	UNIVERSAL_LIB_API CUIObject* SetFocus(CUIObject* pFocus);
	UNIVERSAL_LIB_API CUIObject* SetCapture(CUIObject* pCapture);
	void UpdateIMEContext();

	void OnWindowChanged(CUIObject* pChangedWindow);	 // 窗口大小, 位置, 滚动条等发生了变化
	UNIVERSAL_LIB_API void UpdateUIRect(CRect& rc);

	UNIVERSAL_LIB_API bool Create(HWND hWnd, bool bVisible, const CRect& rc);
	UNIVERSAL_LIB_API void Destroy();

	void StartActionTimer();
	void StopActionTimer();
	void NotifyRootChange(std::vector<CUIObject*>& vecObj);
	std::vector<CUIObject*> GetNotifyRootChangeObj(CUIObject* pObj); // @ret: the objects need the notification

protected:

	CUIObject* GetNextArrowObject(CUIObject* pObject);
	CUIObject* GetPrevArrowObject(CUIObject* pObject);

	void OnPostEvent(WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnMouseAction(CUIObject* pWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnLButtonDown(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnLButtonUp(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnLButtonDblClk(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnRButtonDown(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnRButtonUp(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnRButtonDblClk(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnMButtonDown(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnMButtonUp(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnMButtonDblClk(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnXButtonDown(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnXButtonUp(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnXButtonDblClk(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnMouseMove(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnMouseWheel(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnMouseLeave(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	BOOL OnSetCursor(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnKeyDown(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnKeyUp(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnChar(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnMenuChar(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIRootKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnUIRootKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnUIRootChar(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyDown(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnSysKeyUp(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnSysChar(CUIObject* pWnd, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnCaptureChanged(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnActivate(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnActivateApp(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnIMEMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnShowWindow(CUIObject* pWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnTouchEnter(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnTouchDown(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnTouchUp(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnTouchUpdate(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnTouchLeave(WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnTouchCaptureChanged(WPARAM wParam, LPARAM lParam, bool& bHandled);

	void OnEraseBkgnd(WPARAM wParam, LPARAM lParam);
	void OnPaint(WPARAM wParam, LPARAM lParam);
	void OnLayeredPaint();
	void OnActionTimer();
	void OnStatusChanged();

private:

	class CCanvas
	{
	public:
		operator HBITMAP() { return m_bmBuffer; }
		const SIZE& GetSize() const { return m_szBuffer; }
		void AcquireSize(const SIZE& sz)
		{
			if (m_bmBuffer.IsNull() || m_szBuffer != sz)
			{
				m_szBuffer = sz;
				m_bmBuffer.Attach(::CreateCompatibleBitmap(Universal::UI::Cache::GetDC32(), m_szBuffer.cx, m_szBuffer.cy));
				CBitmapDC(m_bmBuffer).FillSolidRect(0, 0, sz.cx, sz.cy, 0xFFFFFF);
			}
		}

	private:

		CBitmap m_bmBuffer;
		CSize m_szBuffer;
	};

	CUIObject* FindCrossPos(CUIObject* pHead1, CUIObject* pHead2);
	CUIObject* FindFocusWindow(CUIObject* pControl, bool bPrev);
	CUIObject* DoFindObject(CUIObject* pWnd, POINT pt, DWORD dwStatus = 0);
	CUIObject* DoFindObject(CUIObject* pParent, LPCWSTR lpszObjectID);
	void NotifyDefaultFontChange(CUIObject* p);
	void NotifyDefaultColorChange(CUIObject* p);
	void NotifyDefaultGrayColorChange(CUIObject* p);
	void InvalidateRect(const RECT& rc);
	void CheckCursor(CUIObject* pObj, DWORD dwOldStatus);
	void PushStatusChange(CUIObject* pObject, DWORD dwOldStatus, DWORD dwNewStatus);
	void DrawFPS(CDCHandle dc);
	void FindObjectChainFromPoint(CUIObject* pWnd, const CPoint& pt, std::stack<ObjHandler>& stkOut, bool bWantDisabled = false);

private:

	friend class CUIObject;
	friend class CUINonClient;
	friend class CPaintContext;
	typedef std::map<CUIObject*, std::pair<DWORD/* OldStatus */, DWORD/* NewStatus */>> Object2OldAndNewStatus;
	CActionManager m_ActionManager;
	std::map<const CUIObject*, int/* count */> m_mapReferredObject;
	std::vector<CUIObject*> m_vecPosChange;	// 滚动条位置发生变化时需要通知的窗口, 作为root窗口时这个指针才有效, 这个主要是针到WinControls的
	std::set<CUIObject*> m_setRootChangeNotify; // UIObject的Root发生变化时，通知到UIObject
	std::vector<IHookMessage*> m_vecMsgHook;
	Object2OldAndNewStatus* m_pStatusChangeMap;
	CFPSCounter m_fps;

	CCanvas m_Canvas;

	HIMC m_hImc;

	HWND m_hWnd;
	HFONT m_hDefaultFont;
	DWORD m_pdwData;
	COLORREF m_clrDefault;
	COLORREF m_clrDefaultGray;
	CRect m_rcInvalidateRect;
	BYTE m_iLayeredAlpha;

	ICommandHandler* m_pCommandHandler;
	CPaintContext* m_pCurrentPaintContext;

	CUIRoot* m_pUIRoot;
	CUIObject* m_pCurrentPressed;
	CUIObject* m_pCurrentHover;
	CUIObject* m_pCurrentMouseOver;
	CUIObject* m_pCurrentFocus;
	CUIObject* m_pCurrentCapture;
	CUIObject* m_pCurrentIMEOwner;

	bool m_bDelayUpdate;
	bool m_bCreated;
	bool m_bFoucsRectActivate;
};

//////////////////////////////////////////////////////////////////////////
// 下面这些函数很小, 导出会比较亏, 直接inline吧

inline HFONT GetSystemFont()
{
	static HFONT hSystemFont = NULL;
	if (hSystemFont == NULL)
	{
		HFONT hFont = (HFONT)::GetStockObject(SYSTEM_FONT);

		// 根据当前 DPI 设置，重新创建字体
		if (IS_UI_SCALED())
		{
			LOGFONT LogFont;
			if (::GetObject(hFont, sizeof(LOGFONT), &LogFont) == sizeof(LOGFONT))
			{
				DeleteObject(hFont);
				
				LogFont.lfHeight = UI_SCALE(LogFont.lfHeight);

				// lfWidth
				// If lfWidth is zero, the aspect ratio of the device is matched
				// against the digitization aspect ratio of the available fonts to find the closest match,
				// determined by the absolute value of the difference.
				LogFont.lfWidth = 0;

				hFont = ::CreateFontIndirect(&LogFont);
			}
		}

		hSystemFont = hFont;
	}

	return hSystemFont;
}

inline HFONT CUIManager::GetDefaultFont()
{
	if (NULL == m_hDefaultFont)
		m_hDefaultFont = GetSystemFont();
	return m_hDefaultFont;
}

inline void CUIManager::StartActionTimer()
{
	m_ActionManager.StartActionTimer();
}

inline void CUIManager::StopActionTimer()
{
	m_ActionManager.StopActionTimer();
}
