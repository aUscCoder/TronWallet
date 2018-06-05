// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "UIControls.h"
#include "UILayout.h"


class CUITabCtrl;

class CUITabTitle: public CUIControl
{
public:

	explicit CUITabTitle(bool bManaged) : m_bCheck(false), m_bAutoDelete(bManaged) {}
	~CUITabTitle() {}

	bool IsManaged() const { return m_bAutoDelete; }

	UNIVERSAL_LIB_API void SetCheck(bool b);
	bool GetCheck() { return m_bCheck; }

	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnPaint(CDCHandle dc, DWORD dwStatus);
	bool OnMeasureClient(SIZE& sz);

	bool m_bAutoDelete;
	bool m_bCheck;

};

enum TabHeadLayoutMode
{
	THLM_AlignLeft,
	THLM_FullFill,
};

class CUITabHead: public CUIRichObject
{
public:

	UNIVERSAL_LIB_API CUITabHead();
	UNIVERSAL_LIB_API ~CUITabHead();

	void Add(CUITabTitle* pTitle);
	UNIVERSAL_LIB_API void SetCheck(int iItem, bool bUserClick);
	void SetLayoutMode(TabHeadLayoutMode eMode, CRect rcPadding = CRect(), int iHSpace = 0, int iVSpace = 0, CRect rcCheckInflate = CRect())
	{
		m_eLayoutMode = eMode;
		m_rcPadding = rcPadding;
		m_iHSpace = iHSpace;
		m_iVSpace = iVSpace;
		m_rcCheckInflate = rcCheckInflate;
	}
	void SetMaxWidth(int iMaxWidth) { m_iMaxWidth = iMaxWidth; }
	void SetTitleTextPadding(CRect rcTextPadding)
	{
		m_rcTextPadding = rcTextPadding;
	}
	void SetTitleBkPainter(IPainter *pPainter, bool bAutoDelete = false)
	{
		if (m_pTitleBkPainter != NULL && m_bAutoDeleteTitleBkPainter)
			delete m_pTitleBkPainter;

		m_pTitleBkPainter = pPainter;
		m_bAutoDeleteTitleBkPainter = bAutoDelete;
	}

	void SetTitlePainter(int iItem, IPainter *pPainter, bool bAutoDelete = false)
	{
		UIASSERT(iItem >= 0 && iItem < (int)m_vecTab.size());
		m_vecTab[iItem]->UISetPainter(pPainter, bAutoDelete);
	}

	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	void OnCreate();
	void OnSize();
	bool OnMeasureClient(SIZE& sz);
	LRESULT OnCommand(CUIObject* pSender, DWORD dwControlID, DWORD dwCode, UINH* pHDR);
	bool OnGetKidsZOrder(std::vector<CUIObject*>* pResultVec);

	CRect Layout(bool bMeasure);

	friend class CUITabTitle;

	std::vector<CUITabTitle*> m_vecTab;

	CRect m_rcTextPadding;

	IPainter *m_pTitleBkPainter;
	bool m_bAutoDeleteTitleBkPainter;

	TabHeadLayoutMode m_eLayoutMode;	// 布局模式
	CRect m_rcPadding;					// Head 里减去这个 Padding 后才是 Title 们的布局空间
	CRect m_rcCheckInflate;				// Title 被 Check 后的放大差值
	int m_iHSpace;						// Title 之间的横向间距
	int m_iVSpace;						// Title 之间的纵向间距
	int m_iMaxWidth;					// 布局计算所使用的最大宽度，需要在布局之前设定好
};

class CUITabBody: public CUIObject
{
public:

	CUITabBody() : m_iCurrentItem(-1)
	{
		m_rcBorder.SetRect(2, 2, 2, 2);
	}

	~CUITabBody() {}

	void Add(CUIObject* p);
	void SetDisplay(int iItem);
	int GetDisplay() { return m_iCurrentItem; }
	void SetBorder(CRect rcBorder) { m_rcBorder = rcBorder; }
	int GetCurrentItem() { return m_iCurrentItem; }
	int GetItemsCount() { return (int)m_vecBody.size(); }

	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	void OnUISize();
	void OnUIShowUI(bool bShow);
	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);

	void HideAllItems();
	void LayoutItem(int iItem);

	std::vector<CUIObject*> m_vecBody;
	int m_iCurrentItem;
	CRect m_rcBorder;
};

struct UITabSelChange
{
	UINH hdr;
	int iOldSel;
	int iNewSel;
	bool bUserClick;
};


class CUITabCtrl: public CUIControl
{
public:

	CUITabCtrl() {}
	~CUITabCtrl() {}

	UNIVERSAL_LIB_API void AddTab(const std::wstring& strTitle, CUIObject* pBody);

	void SetHeadLayoutMode(TabHeadLayoutMode eMode, CRect rcPadding = CRect(), int iHSpace = 0, int iVSpace = 0, CRect rcCheckInflate = CRect())
	{
		m_duiHead.SetLayoutMode(eMode, rcPadding, iHSpace, iVSpace, rcCheckInflate);
	}

	void SetHeadBkPainter(IPainter *pPainter, bool bAutoDelete = false)
	{
		m_duiHead.UISetBkPainter(pPainter, bAutoDelete);
	}

	void SetTitleTextPadding(CRect rcTextPadding)
	{
		m_duiHead.SetTitleTextPadding(rcTextPadding);
	}

	void SetTitleTextPadding(int iLeft, int iTop, int iRight, int iBottom)
	{
		SetTitleTextPadding(CRect(iLeft, iTop, iRight, iBottom));
	}

	void SetTitleBkPainter(IPainter *pPainter, bool bAutoDelete = false)
	{
		m_duiHead.SetTitleBkPainter(pPainter, bAutoDelete);
	}

	void SetTitlePainter(int iItem, IPainter *pPainter, bool bAutoDelete = false)
	{
		m_duiHead.SetTitlePainter(iItem, pPainter, bAutoDelete);
	}

	void SetBodyBorder(CRect rcBorder)
	{
		m_duiBody.SetBorder(rcBorder);
	}

	void SetBodyBkPainter(IPainter *pPainter, bool bAutoDelete = false)
	{
		m_duiBody.UISetBkPainter(pPainter, bAutoDelete);
	}

	void SelectTab(int iItem)
	{
		m_duiHead.SetCheck(iItem, false);
	}

	int GetSelectedIndex()
	{
		return m_duiBody.GetDisplay();
	}

	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

private:

	void OnCreate();
	void OnSize();
	void OnUIShowUI(bool bShow);
	void OnPaint(CDCHandle dc, DWORD dwStatus);

	void _AddTab(CUITabTitle* pTitle, CUIObject* pBody);
	void _SetDisplay(int iItem, bool bUserClick);
	void _Adjust();

private:

	CUITabHead m_duiHead;
	CUITabBody m_duiBody;

	friend class CUITabHead;

};
