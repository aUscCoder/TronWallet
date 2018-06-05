// Author: 津津
#pragma once
#include "../TextLayout.h"
#include "UIControlDef.h"
#include "UIControls.h"


struct UIRichLinkClick
{
	UINH hdr;
	int iLinkId;
};

class CUIRichLink : public CUIControl
{

public:

	CUIRichLink() : m_pBlockHover(NULL), m_TextLayout(this) {}
	~CUIRichLink() {}

	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	CUITextLayout &GetTextLayoutor()	{ return m_TextLayout; }

private:

	void OnUICreate();
	void OnUIPaint(CDCHandle dc, DWORD dwStatus);
	void OnUIMouseLeave();
	void OnUIMouseMove(UINT nFlags, POINT point);
	void OnUILButtonDown(UINT nFlags, POINT point);
	void OnUILButtonUp(UINT nFlags, POINT point);
	bool OnUISetCursor(UINCHitTestCode ht, int iMouseEvent);
	void OnUICaptureChanged(CUIObject* pCapture);
	LRESULT OnReflectClick(DWORD dwCommandCode, UINH* pNotifyHeader);

	CUITextLayout m_TextLayout;
	CUITextBlock* m_pBlockHover;
};
