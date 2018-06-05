// Author: ChenJianjun
#pragma once
#include "UIControls.h"


#define BTN_ID_INVALID	-1
#define BTN_ID_CHEVRON	-2

enum ToolButtonType
{
	ToolButton_Normal,
	ToolButton_Dropdown,
	ToolButton_WithDropdown,
	ToolButton_Toggle,
	ToolButton_Null,
	ToolButton_Sep,
	ToolButton_Custom,
};

enum ToolButtonHoverPart
{
	TBHoverPart_Null,
	TBHoverPart_Button,
	TBHoverPart_Dropdown,
};

struct AddToolButton
{
	ToolButtonType eType;
	DWORD_PTR	dwData;
	bool		bChecked;
	bool		bEnabled;
	int			iTextCount;
	int			iWidth;
	int			iDropdownPartWidth;	// 当 eType == ToolButton_WithDropdown 时有意义

	CUIObject*	pObject;		// 当 eType == ToolButton_Custom 时有意义
	bool		bObjectManaged;	// 为 true 时, 自动删除pObject
};

struct UITBButton
{
	UINH hdr;
	int iIndex;
	CPoint point;
};

struct UITBStatusChange
{
	UINH hdr;
	CUIObject* pButton;
	int iIndex;
	DWORD dwOldStatus;
	DWORD dwNewStatus;
};

struct UITBTextCallback
{
	UINH hdr;
	int iIndex;
	std::wstring strText;
};

struct _ToolButton
{
	CUIObject*	pUIObject;
	bool		bManaged;
	int			iIndex;

	ToolButtonType eType;
	int			iWidth;
	int			iDropdownPartWidth;
	int			iMaxTextCount;
	bool		bChecked;
	bool		bEnabled;
	bool		bTextCallback;
	DWORD_PTR	pData;
	RECT		rc;
};

class CUIToolbar;

class CUIToolbarButton : public CUIRichObject
{
public:

	explicit CUIToolbarButton(_ToolButton* pButton);
	~CUIToolbarButton();

	int GetIndex() const { return m_pButton->iIndex; }

	BEGIN_UI_EVENT_MAP(CUIToolbarButton)
		UI_REDRAW_ON_OWN_MOUSE_OR_FOCUS()
		UI_DECLARE_ORIGINAL_OBJECT_TYPE(UIObject_ToolbarButton)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
		UI_EVENT_RANGE(UIEvent_MouseFirst, UIEvent_MouseLast, OnUIMouseEvent)
	END_UI_EVENT_MAP()

	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	LRESULT OnUIMouseEvent(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:

	_ToolButton* m_pButton;
};

//////////////////////////////////////////////////////////////////////////
class CUIToolbarChevron : public CUIToolbarButton
{
public:

	explicit CUIToolbarChevron(CUIToolbar* p);
	~CUIToolbarChevron() {}

	BEGIN_UI_EVENT_MAP(CUIToolbarChevron)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
	END_UI_EVENT_MAP()

	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);

private:

	CUIToolbar* m_pToolbar;
	_ToolButton m_tb;
};

//////////////////////////////////////////////////////////////////////////

class CUIToolbar : public CUIControl
{
public:
	UNIVERSAL_LIB_API explicit CUIToolbar(DWORD_PTR dwID = 0, DWORD dwStyle = 0);
	UNIVERSAL_LIB_API ~CUIToolbar(void);

	UNIVERSAL_LIB_API bool AddButton(AddToolButton* pButton, int iButtons);
	int GetButtonCount() const { return m_Items.size(); }
	int GetDisplayCount() const { return m_iDisplayCount; }
	RECT GetChevronRect() const { return m_Chevron.UIGetRect(); }
	_DECLARE_INLINE RECT GetButtonRect(int iIndex);
	int GetButtonWidth(int iIndex) const;
	ToolButtonType GetButtonType(int iIndex) const;
	bool GetChecked(int iIndex) const;
	int GetDropdownPartWidth(int iIndex) const;
	bool GetEnable(int iIndex) const;
	DWORD_PTR GetData(int iIndex) const;
	int GetTextLimit(int iIndex) const;
	std::wstring GetText(int iIndex);
	int GetDropdownIndex() const { return m_iDropdown; }
	bool IsChevronActivate() const { return m_bUseChevron && m_iDisplayCount < (int)m_Items.size(); }
	int GetButtonFromPoint(const CPoint& pt);
	int GetHotItem() const { return m_iHover; }
	int GetPressed() const { return m_iPressed; }

	// CTuoToolBarCtrl::HitTest 要调用，不想拷贝一份代码，所以还是导出吧
	UNIVERSAL_LIB_API int HitTest(const CPoint& pt);

	UNIVERSAL_LIB_API void RemoveButton(const int iIndex);
	UNIVERSAL_LIB_API void RemoveAllButton();

	void RedrawButton(int iIndex);

	UNIVERSAL_LIB_API void SetButtonPainter(IPainter* pPainter, bool bManaged = false);
	UNIVERSAL_LIB_API void SetChevronPainter(IPainter* pPainter, bool bManaged = false);
	void SetButtonWidth(int iIndex, int iWidth);
	void SetChevronWidth(int iWidth);
	void SetButtonSpace(int iSpace);
	void SetCheck(int iIndex, bool bCheck);
	void SetData(int iIndex, DWORD_PTR dwData);
	void SetEnable(int iIndex, bool bEnable);
	void SetTextLimit(int iIndex, int iTextLen);
	void SetDropdownPartWidth(int iIndex, int iWidth);
	void SetUseChevron(bool bUse);
	UNIVERSAL_LIB_API void SetDropdownItem(int iDropdown);
	void SetHotItem(int iHotItem) { SetHoverButton(iHotItem); }
	void SetMinDisplayWidth(int iWidth) { m_iLastDisplayButtonMinWidth = iWidth; DeferLayout(); }
	CUIObject *GetButtonObject(int iIndex) const { return m_Items[iIndex]->pUIObject; }	// 没办法了，临时用着，以后想办法去掉
	void ShowDropdown(int iIndex) { UIFireAsyncEvent(UITBEvent_ShowDropdown, (WPARAM)iIndex, NULL); }

	void LayoutNowIfNeed();
	
	//////////////////////////////////////////////////////////////////////////
	// callback
	virtual void OnDrawButton(int iIndex, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha);
	virtual void OnDrawChevron(int iIndex, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha);
	virtual void OnReLayout() {}

	//////////////////////////////////////////////////////////////////////////
	void OnUICreate();
	bool OnUIGetKidsZOrder(std::vector<CUIObject*>* pResultVec);
	void OnUISize();
	void OnUIPrePaint();
	void OnUILButtonDown(UINT nFlags, const CPoint& point);
	void OnUILButtonUp(UINT nFlags, const CPoint& point);
	void OnUILButtonDblClk(UINT nFlags, const CPoint& point);
	void OnUIRButtonDown(UINT nFlags, const CPoint& point);
	void OnUIRButtonUp(UINT nFlags, const CPoint& point);
	void OnUIRButtonDblClk(UINT nFlags, const CPoint& point);
	void OnUIMButtonDown(UINT nFlags, const CPoint& point);
	void OnUIMButtonUp(UINT nFlags, const CPoint& point);
	void OnUIMButtonDblClk(UINT nFlags, const CPoint& point);
	void OnUIMouseMove(UINT nFlags, const CPoint& point);
	void OnUIMouseLeave();
	void OnUICaptureChanged(CUIObject* pCapture);
	LRESULT OnUIButtonMouseMove(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonLDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonLUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonLDblClk(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonRDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonRUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonRDblClk(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonMDown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonMUp(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonMDblClk(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonMouseEnter(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIButtonMouseLeave(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIShowDropdown(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

protected:

	friend class CUIToolbarButton;
	friend class CToolbarStatusSnapshot;
	
	void SetCaptureButton(int iIndex);
	void ReleaseCaptureButton();
	void SetHoverButton(int iIndex);
	void SetPressedButton(int iIndex);
	void OnTBStatusChanged(int iIndex, DWORD dwOldStatus);
	void PrepareDrag(const CPoint& ptDown);
	void ProcessDrag(const CPoint& ptCursor);
	void ShowDropdownInternal(int iIndex);

	UNIVERSAL_LIB_API void AddOrRemoveButtonStatus(int iButton, bool bAdd, DWORD dwStatus);
	UNIVERSAL_LIB_API void DeferLayout(bool bDelay = true);
	void ReLayout();
	bool ForwardMouseEventWhenCapture(int iEvent, const CPoint point);

	IPainter* m_pPaintChevron;
	IPainter* m_pPaintButton;
	bool m_bManageChevronPainter;
	bool m_bManagePainter;

	CUIToolbarChevron m_Chevron;

	std::vector<_ToolButton*> m_Items;
	bool m_bNeedLayout;
	bool m_bUseChevron;
	int m_iChevronWidth;
	int m_iLastDisplayButtonMinWidth;
	int m_iButtonSpace;
	int m_iDisplayCount;

	CPoint m_ptDownPos;
	int m_iDraging;
	int m_iCapture;
	int m_iDropdown;
	int m_iHover;
	int m_iPressed;
};

//////////////////////////////////////////////////////////////////////////

inline void CUIToolbarChevron::OnUIPaintBackground(CDCHandle dc, DWORD dwStatus)
{
	m_pToolbar->OnDrawChevron(BTN_ID_CHEVRON, dc, UIGetClientRect(), UIGetStatus(), 255);
}

inline void CUIToolbar::OnDrawButton(int iIndex, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha)
{
	if (m_pPaintButton)
	{
		m_pPaintButton->DoPaint(m_Items[iIndex]->pUIObject, dc, rc, dwStatus, cAlpha);
	}
	else
	{
		if (dwStatus & UIStatus_Hover)
			dc.FillSolidRect(&rc, 0xFF0000);
	}
}

inline void CUIToolbar::OnDrawChevron(int iIndex, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha)
{
	if (m_pPaintChevron)
	{
		m_pPaintChevron->DoPaint(&m_Chevron, dc, rc, dwStatus, cAlpha);
	}
	else
	{
		if (dwStatus & UIStatus_Hover)
			dc.FillSolidRect(&rc, 0xFF0000);
	}
}

inline RECT CUIToolbar::GetButtonRect(int iIndex)
{
	if (iIndex == BTN_ID_CHEVRON)
		return m_Chevron.UIGetRect();

	if (iIndex >= GetButtonCount())
		return CRect();

	LayoutNowIfNeed();
	return m_Items[iIndex]->rc;
}

inline int CUIToolbar::GetButtonFromPoint(const CPoint& pt)
{
	LayoutNowIfNeed();

	if (IsChevronActivate() && m_Chevron.UIGetRect().PtInRect(pt))
		return BTN_ID_CHEVRON;

	// 这个地方有空可以优化
	for (size_t n = 0; n < m_Items.size(); n++)
	{
		if (::PtInRect(&m_Items[n]->rc, pt))
			return n;
	}

	return BTN_ID_INVALID;
}

inline void CUIToolbar::SetCheck(int iIndex, bool bCheck)
{
	UIASSERT(m_Items[iIndex]->eType == ToolButton_Toggle);
	if (m_Items[iIndex]->bChecked != bCheck)
	{
		m_Items[iIndex]->bChecked = bCheck;
		TuoAssert(m_Items[iIndex]->eType != ToolButton_Null && "对null设置check无意义");
		m_Items[iIndex]->pUIObject->UIRedraw();
	}
}

inline void CUIToolbar::SetEnable(int iIndex, bool bEnable)
{
	if (m_Items[iIndex]->bEnabled != bEnable)
	{
		m_Items[iIndex]->bEnabled = bEnable;
		TuoAssert(m_Items[iIndex]->eType != ToolButton_Null && "对null设置enable无意义");
		m_Items[iIndex]->pUIObject->UIDisable(!bEnable);
	}
}

inline void CUIToolbar::SetButtonSpace(int iSpace)
{
	if (m_iButtonSpace != iSpace)
	{
		m_iButtonSpace = iSpace;
		m_bNeedLayout = true;
		UIRedraw();
	}
}

inline void CUIToolbar::SetButtonWidth(int iIndex, int iWidth)
{
	if (iIndex < 0 || iIndex >= (int)GetButtonCount() || GetButtonWidth(iIndex) == iWidth)			// 若宽度未变，则不刷新
		return;

	if (m_Items[iIndex]->iWidth != iWidth)
	{
		m_Items[iIndex]->iWidth = iWidth;
		m_bNeedLayout = true;
		UIRedraw();
	}
}

inline void CUIToolbar::SetData(int iIndex, DWORD_PTR dwData)
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		m_Items[iIndex]->pData = dwData;
}

inline void CUIToolbar::SetTextLimit(int iIndex, int iTextLen)
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		m_Items[iIndex]->iMaxTextCount = (char)iTextLen;
}

inline void CUIToolbar::SetDropdownPartWidth(int iIndex, int iWidth)
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		m_Items[iIndex]->iDropdownPartWidth = iWidth;
}

inline int CUIToolbar::GetButtonWidth(int iIndex) const
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		return m_Items[iIndex]->iWidth;
	return 0;
}

inline ToolButtonType CUIToolbar::GetButtonType(int iIndex) const
{
	TuoAssert(iIndex == BTN_ID_CHEVRON || (iIndex < (int)m_Items.size() && iIndex >= 0));
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		return m_Items[iIndex]->eType;

	if (iIndex == BTN_ID_CHEVRON)
		return ToolButton_Dropdown;

	return ToolButton_Null;
}

inline bool CUIToolbar::GetChecked(int iIndex) const
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		return m_Items[iIndex]->bChecked;
	return false;
}

inline int CUIToolbar::GetDropdownPartWidth(int iIndex) const
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		return m_Items[iIndex]->iDropdownPartWidth;
	return 0;
}

inline bool CUIToolbar::GetEnable(int iIndex) const
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		return m_Items[iIndex]->bEnabled;
	return false;
}

inline DWORD_PTR CUIToolbar::GetData(int iIndex) const
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		return m_Items[iIndex]->pData;
	return NULL;
}

inline int CUIToolbar::GetTextLimit(int iIndex) const
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0)
		return m_Items[iIndex]->iMaxTextCount;
	return 0;
}

inline std::wstring CUIToolbar::GetText(int iIndex)
{
	if (iIndex < 0 || iIndex >= (int)m_Items.size())
		return std::wstring();

	if (m_Items[iIndex]->bTextCallback)
	{
		UITBTextCallback tc = { { this, GetControlID(), UITBN_TEXTCALLBACK }, iIndex };
		if (FireCommand(UIGetParent(), (UINH*)&tc))
			return tc.strText;
	}

	return std::wstring();
}

inline void CUIToolbar::SetChevronWidth(int iWidth)
{
	if (m_iChevronWidth != iWidth)
	{
		m_iChevronWidth = iWidth;
		m_bNeedLayout = true;
		UIRedraw();
	}
}

inline void CUIToolbar::RedrawButton(int iIndex)
{
	TuoAssert(iIndex < (int)m_Items.size() && iIndex >= 0);
	if (iIndex < (int)m_Items.size() && iIndex >= 0 && m_Items[iIndex]->pUIObject)
		m_Items[iIndex]->pUIObject->UIRedraw();
}

inline void CUIToolbar::LayoutNowIfNeed()
{
	if (m_bNeedLayout)
		DeferLayout(false);
}

inline void CUIToolbar::SetUseChevron(bool bUse)
{
	if (m_bUseChevron != bUse)
	{
		m_bUseChevron = bUse;
		DeferLayout();
	}
}

inline void CUIToolbar::SetHoverButton(int iIndex)
{
	if (m_iHover != iIndex)
	{
		AddOrRemoveButtonStatus(m_iHover, false, UITBStatus_Hover | UITBStatus_DropdownPartHover);
		m_iHover = iIndex;
		AddOrRemoveButtonStatus(m_iHover, true, UITBStatus_Hover);
	}
}

inline void CUIToolbar::SetPressedButton(int iIndex)
{
	if (m_iPressed != iIndex)
	{
		AddOrRemoveButtonStatus(m_iPressed, false, UITBStatus_Pressed);
		m_iPressed = iIndex;
		AddOrRemoveButtonStatus(m_iPressed, true, UITBStatus_Pressed);
	}

	if (m_iPressed != m_iHover && m_iPressed != BTN_ID_INVALID && m_iHover != BTN_ID_INVALID)
		SetHoverButton(BTN_ID_INVALID);
}
