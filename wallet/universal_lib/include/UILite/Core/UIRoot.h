// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "../UICrack.h"
#include "UICoreEvent.h"
#include "UIManager.h"
#include "UIRichObject.h"


class CUIRoot: public CUIRichObject
{
public:

	UNIVERSAL_LIB_API CUIRoot(); // NOLINT (3.4.7 BUG)
	~CUIRoot() {} // NOLINT (3.4.7 BUG)
	
	UNIVERSAL_LIB_API void CenterUI(HWND hWnd = NULL);

#ifdef _DEBUG
	bool _isUIRootDebug_() override { return true; }
#endif

	CUIRichObject* GetDefault() { return (CUIRichObject*)UIFireEvent(UIEvent_RootGetDefault); }
	void SetDefault(CUIRichObject* pDefault) { UIFireEvent(UIEvent_RootSetDefault, (WPARAM)pDefault); }
	bool IsModal() { return (int)UIFireEvent(UIEvent_GetRootType) == UIRootType_Modal; }
	virtual void EndModal(int iEndCode) { (iEndCode); UIASSERT(FALSE); }

	BEGIN_MSG_MAP(CUIRoot)
		CHAIN_MSG_MAP_MEMBER(m_UIMgr)
	END_MSG_MAP()

	BEGIN_UI_EVENT_MAP(CUIRoot)
		UI_DECLARE_ORIGINAL_OBJECT_TYPE(UIObject_Root)
		UI_EVENT_UITEXTCHANGE(OnUITextChange)
		UI_EVENT_HANDLER(UIEvent_GetRootType, OnGetRootType)
	END_UI_EVENT_MAP()

	void OnUITextChange();
	LRESULT OnGetRootType(int, WPARAM, LPARAM, bool&) { return UIRootType_Normal; }

private:
	
	CUIManager m_UIMgr;
};

//////////////////////////////////////////////////////////////////////////
//
inline void CUIRoot::OnUITextChange()
{
	if (UIGetHWND())
		::SetWindowText(UIGetHWND(), UIGetText().c_str());
}
