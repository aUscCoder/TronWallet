// Author: ChenJianjun
#pragma once
#include "../Util/PaintStatusHelper.h"
#include "UIControls.h"


#define ALIGNLEFT	0
#define ALIGNMIDDLE	1
#define ALIGNRIGHT 2

// 按钮
class CUIButton: public CUIControl, public CPaintStatusHelper
{
public:

	UNIVERSAL_LIB_API explicit CUIButton(DWORD_PTR dwControlID = 0, LPCWSTR lpszText = NULL, int iAlign = ALIGNMIDDLE);
	virtual ~CUIButton() {}

	void SetPadding(RECT rcPadding) { m_rcPadding = rcPadding; }
	bool GetOffsetOnPress() const { return m_bOffsetTextOnPress; }
	void SetOffsetOnPress(bool bEnable) { m_bOffsetTextOnPress = bEnable; }

	int  GetAlign() const { return m_iAlign; }
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	bool OnMeasureClient(SIZE& sz);
	void OnEnableWindow(bool bEnable) { UIRedraw(); }
	LRESULT OnWantEnter(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled) { return TRUE; }
	LRESULT OnReflectClick(DWORD dwCommandCode, UINH* pNotifyHeader);
	LRESULT OnReflectKeyDown(DWORD dwCommandCode, UINH* pNotifyHeader);
	LRESULT OnReflectKeyUp(DWORD dwCommandCode, UINH* pNotifyHeader);

	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

protected:
	
	CRect	GetTextPartRect(CDCHandle dc, DWORD dwStatus);
	RECT	m_rcPadding;

private:

	int		m_iAlign;
	bool	m_bSpaceKeyDown;
	bool	m_bOffsetTextOnPress;
};

class CUICheckBox: public CUIButton
{
public:

	UNIVERSAL_LIB_API CUICheckBox();
	UNIVERSAL_LIB_API ~CUICheckBox();

	//	0 - unchecked
	//	> 0 - checked
	//	< 0 - mix
	UNIVERSAL_LIB_API int GetCheck();
	UNIVERSAL_LIB_API void SetCheck(int iCheck);
	void SetIconTextSpacing(int iSpacing) { m_iIconTextSpacing = iSpacing; }
	
	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	void OnUIStyleChanged(DWORD dwOldStyle);
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	bool OnMeasureClient(SIZE& sz);
	LRESULT OnWantEnter(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled) { return FALSE; }
	LRESULT OnReflectClick(DWORD dwCommandCode, UINH* pNotifyHeader);

private:
	void _SetCheck(int iCheck, bool bNotify);

	int m_iIconTextSpacing;

};

class CUIRadio: public CUIButton
{
public:

	UNIVERSAL_LIB_API CUIRadio();
	~CUIRadio() {}

	UNIVERSAL_LIB_API bool GetCheck();
	UNIVERSAL_LIB_API void SetCheck(bool bCheck);
	DWORD GetGroupID() { return m_dwGroupID; }
	void SetGroupID(DWORD dwGID) { m_dwGroupID = dwGID; }
	void SetIconTextSpacing(int iSpacing) { m_iIconTextSpacing = iSpacing; }

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	void OnUIStyleChanged(DWORD dwOldStyle);
	void OnPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	bool OnMeasureClient(SIZE& sz);
	LRESULT OnWantEnter(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled) { return FALSE; }
	LRESULT OnWantArrow(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled) { return TRUE; }
	LRESULT OnReflectClick(DWORD dwCommandCode, UINH* pNotifyHeader);
	LRESULT OnReflectKeyDown(DWORD dwCommandCode, UINH* pNotifyHeader);
	LRESULT OnReflectKeyUp(DWORD dwCommandCode, UINH* pNotifyHeader);

private:

	void _SetCheck(bool bCheck, bool bNotify);
	void _SetSelfCheck(bool bCheck, bool bNotify);

	int m_iIconTextSpacing;

	// 组ID, 在同一级子窗口中组ID相同的单选框同时只有一个能被选中
	DWORD m_dwGroupID;
};

//////////////////////////////////////////////////////////////////////////

class CUIImageButton: public CUIButton
{
public:

	explicit CUIImageButton(DWORD_PTR dwControlID = 0) : CUIButton(dwControlID) {}
	~CUIImageButton() {}

	void SetBitmap(HBITMAP hBitmap, const CUIStatsImageFrameInfo& sifi)
	{
		m_hBitmap = hBitmap;
		m_sifi = sifi;
	}

	BEGIN_UI_EVENT_MAP(CUIImageButton)
		UI_EVENT_PAINT(OnPaint)
		UI_EVENT_PAINTBACKGROUND(OnPaintBackground)
		UI_EVENT_MEASURECLIENT(OnMeasureClient)
	END_UI_EVENT_MAP()

	void OnPaintBackground(CDCHandle dc, DWORD dwStatus)
	{
		UI_IMPLEMENT_PAINTER(UIGetBkPainter(), this, dc, UIGetClientRect(), UIGetStatus());

		Universal::UI::Paint::DrawImage(dc, UIGetClientRect(), m_hBitmap, m_sifi.ToFraw9Frame(UIGetStatus()));
	}

	void OnPaint(CDCHandle dc, DWORD dwStatus) {}

	bool OnMeasureClient(SIZE& sz)
	{
		m_hBitmap.GetSize(sz);
		sz = CSize(sz.cx / std::max(m_sifi.count(), 1), sz.cy);
		return true;
	}

private:

	CBitmapHandle m_hBitmap;
	CUIStatsImageFrameInfo m_sifi;
};
