// Author: miaojinquan
#pragma once
#include "../Core/EventFilter.h"
#include "../Core/UIProps.h"
#include "../UICrack.h"
#include "UIControls.h"


/**
	UIScrollArea是一个具有外部滚动条的控件，根据Content的UIMeasure的返回值来决定是否显示滚动条；
	|----------------------------------------------------------------|
	|		            TopMargin    		            	         |
	|          ---------------------------------------               |
	|LeftMagin|                                       |rightmargin   |
	|         |										  |				 |
	|		  |			Content                       |       		 |
	|		  |										  |				 |
	|		  |										  |				 |
	|		  |										  |				 |
	|		  -----------------------------------------				 |
	|                         bottommargin                           |
	|________________________________________________________________|
	详细用法见cpp中的例子
*/

struct IUIScrollbar;
class CUIScrollArea : public CUIControl, public CUIEventFilter
{
public:

	UNIVERSAL_LIB_API explicit CUIScrollArea(IUIScrollbar* pHScrollBar = nullptr, IUIScrollbar* pVScrollBar = nullptr, CUIObject* pViewport = nullptr);
	~CUIScrollArea() {}

	UNIVERSAL_LIB_API void SetContentMargin(const int iLeft, const int iTop, const int iRight, const int iBottom);
	void SetContentObject(CUIObject* pObject);
	IUIScrollbar* GetScrollBar(bool bHorz) { return bHorz ? m_pHScrollBar : m_pVScrollBar; }
	UNIVERSAL_LIB_API void SetScrollBar(IUIScrollbar* pScrollBar);
	CUIObject* GetViewport() { return m_pViewport.get(); }
	void ScrollTo(bool bHorz, const int iPos);
	int  GetScrollPos(bool bHorz);

	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	void OnUICreate();
	void OnUISize();
	void OnUIStyleChanged(DWORD dwOldStyle);

protected:

	UNIVERSAL_LIB_API bool OnEventFilter(CUIObject* pObj, const DEvent& event) override;
	UNIVERSAL_LIB_API virtual void Layout();
	UNIVERSAL_LIB_API virtual void ScrollContentTo(bool bHorz, const int iPos);
	virtual void OnViewportScrollChanged(const DEvent& event) {}

	void UpdateViewportGeometry();
	void InitScrollbar(IUIScrollbar* pBar);
	CRect CalcViewportRect();


private:

	se::managed_ptr<CUIObject> m_pViewport;
	CUIObject*			m_pContent = nullptr;
	IUIScrollbar*		m_pVScrollBar = nullptr;
	IUIScrollbar*		m_pHScrollBar = nullptr;
	UIPropMargin		m_propMarginContent;
};
