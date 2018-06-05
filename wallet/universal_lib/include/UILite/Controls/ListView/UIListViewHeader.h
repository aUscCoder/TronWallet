// Author: miaojinquan
#pragma once
#include "../../Core/UIObject.h"
#include "../../Core/UIRichObject.h"
#include "UIListViewDef.h"


class CUIListView;
class CUIListViewHeaderSection : public CUIObject
{
public:

	explicit CUIListViewHeaderSection(CUIListView* pListView) : m_pListView(pListView) {}
	~CUIListViewHeaderSection()
	{
		if (m_lvColumn.lpszText)
		{
			delete m_lvColumn.lpszText;
			m_lvColumn.lpszText = nullptr;
		}
	}

	void SetInfo(const UIListViewColumn &lvc);
	UIListViewColumn* GetInfo() { return &m_lvColumn; }
	bool PtIsResizable(const POINT &point);			// 判断一个点是否在可以移动Section的区域

	int GetWidth() { return m_lvColumn.iWidth; }
	CRect GetValidClientRect() const;
	CRect GetValidRect() const;						// 得到Section的可视区域

	//////////////////////////////////////////////////////////////////////////
	// 消息响应函数
	BEGIN_UI_EVENT_MAP(CUIListViewHeaderSection)
		UI_EVENT_GETTEXT(OnUIGetText)
		UI_EVENT_GETTEXTLENGTH(OnUIGetTextLength)
		UI_EVENT_PAINT(OnUIPaint)
		UI_EVENT_PAINTBACKGROUND(OnUIPaintBackground)
		UI_EVENT_SETCURSOR(OnUISetCursor)
		UI_EVENT_MOUSEMOVE(OnUIMouseMove)
		UI_EVENT_LBUTTONDOWN(OnUILButtonDown)
		UI_EVENT_LBUTTONUP(OnUILButtonUp)
		UI_EVENT_KEYDOWN(OnUIKeyDown)
		UI_EVENT_CAPTURECHANGE(OnUICaptureChanged)
	END_UI_EVENT_MAP()

	void OnUIGetText(std::wstring& strTextOut) { strTextOut = m_lvColumn.lpszText ? m_lvColumn.lpszText : L""; }
	int  OnUIGetTextLength() { UIASSERT(FALSE && L"shit!!!"); return 0; }
	void OnUIPaint(CDCHandle dc, DWORD dwStatus);
	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	bool OnUISetCursor(UINCHitTestCode tc, int iParam);
	void OnUIMouseMove(UINT nFlags, POINT point);
	void OnUILButtonDown(UINT nFlags, POINT point);
	void OnUILButtonUp(UINT nFlags, POINT point);
	void OnUIKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnUICaptureChanged(CUIObject* pCapture);

	void OnFinalEvent() override { delete this; }

private:

	void	Reset();

	int		m_iDelta			= 0;				// 用于Keydown后，在MouseMove里修改位置
	bool	m_bLButtonPressed	= false;
	POINT	m_ptPressed			= POINT{ -1, -1 };
	CUIListView* m_pListView	= nullptr;
	UIListViewColumn m_lvColumn;

	friend class CUIListViewHeader;
};

class CUIListViewHeader : public CUIRichObject
{
public:

	UNIVERSAL_LIB_API explicit CUIListViewHeader(CUIListView* pListView);
	~CUIListViewHeader() { }

	int GetValidClientWidth();

	void InsertColumn(const int iIndex, const UIListViewColumn &lvc);
	void RemoveColumn(const int iIndex);
	void SetColumn(const int iIndex, const UIListViewColumn &lvc);
	void ReLayout();

	void GetVisibleRange(int &iBegin, int &iEnd);
	int GetCount() const { return m_Sections.size(); }

	CUIListViewHeaderSection* GetSection(const int iIndex) { TuoAssert(TestIndexValid(iIndex)); return m_Sections.at(iIndex); }

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnUIPrePaint();
	void OnUISize();
	void OnUIDestory();
	bool OnUIGetKidsZOrder(std::vector<CUIObject*>* pResultVec);


	LRESULT OnHeaderSectionResized(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnHeaderClicked(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIScrollChanged(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:

	void HideVisible();
	UNIVERSAL_LIB_API bool TestIndexValid(const int iIndex);
	UNIVERSAL_LIB_API void UpdateVisibleIndex();

	int		m_iVisibleBegin = 0;
	int		m_iVisibleEnd = 0;
	bool	m_bNeedLayout = true;
	bool	m_bValidWidthChanged = true;
	bool	m_bVisibleChanged = true;
	int		m_iHeight = 0;
	int		m_iValidWidth = 0;
	CRect	m_rcOldVisible;

	CUIListView* m_pListView = nullptr;
	std::vector<CUIListViewHeaderSection*> m_Sections;
};
