// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "../Controls/UIControlDef.h"
#include "../UICrack.h"
#include "UIApp.h"
#include "UICoreEvent.h"
#include "UIRoot.h"


struct IModalCallback
{
	virtual void OnModalEnd(int iEndCode, LPARAM lParam) = 0;

	// 保证Callback的合法性
	IModalCallback() { g_UIAppModule.AddModalUICallback(this); }
	~IModalCallback() { g_UIAppModule.RemoveModalUICallback(this); }
};

class CUIManager;
class CUIModalRoot: public CUIRoot
{
public:

	UNIVERSAL_LIB_API CUIModalRoot();
	~CUIModalRoot() {}

	UNIVERSAL_LIB_API int ModalUI(HWND hParent = ::GetActiveWindow());
	UNIVERSAL_LIB_API bool ModalCallbackUI(IModalCallback* pCallback, HWND hParent = ::GetActiveWindow());
	UNIVERSAL_LIB_API virtual void EndModal(int iEndCode) override;
	HWND GetParentHWND() { return m_hParent; }

	BEGIN_MSG_MAP(CUIModalRoot)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		CHAIN_MSG_MAP(CUIRoot)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()
	UNIVERSAL_LIB_API LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	UNIVERSAL_LIB_API LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_UI_EVENT_MAP(CUIModalRoot)
		UI_EVENT_HANDLER(UIEvent_GetRootType, OnGetRootType)
		UI_EVENT_HANDLER(UIEvent_RootGetDefault, OnGetDefault)
		UI_EVENT_HANDLER(UIEvent_RootSetDefault, OnSetDefault)
		UI_EVENT_HANDLER(UIEvent_RootFocusChange, OnRootFocusChanged)
	END_UI_EVENT_MAP()

	LRESULT OnGetRootType(int, WPARAM, LPARAM, bool&) { return UIRootType_Modal; }
	LRESULT OnGetDefault(int, WPARAM, LPARAM, bool&) { return (LRESULT)m_pDefault; }
	LRESULT OnSetDefault(int, WPARAM wp, LPARAM, bool&);
	LRESULT OnRootFocusChanged(int, WPARAM wp, LPARAM, bool&);

private:

	void _OnFocusChange();
	bool _Modal(HWND hWndParent);
	void _CheckParent();
	void _CheckDefaultStatus();

private:

	friend class CUIManager;
	IModalCallback* m_pModalCallback;
	CUIRichObject* m_pDefault;
	bool m_bModalCallbackProcessed;
	bool m_bModalEnded;
	bool m_bEnableParent;
	bool m_bDestroying;
	HWND m_hParent;
	int m_iModalRetCode;
};

//////////////////////////////////////////////////////////////////////////
//
inline LRESULT CUIModalRoot::OnSetDefault(int, WPARAM wp, LPARAM, bool&)
{
	if (m_pDefault)
	{
		UIAddOrRemoveStatus(false, (DWORD)UIControlStatus_Default);
		m_pDefault->UIFireEvent(UIEvent_DefaultChanged, (WPARAM)FALSE);
	}

	m_pDefault = (CUIRichObject*)wp;
	if (m_pDefault)
	{
		_CheckDefaultStatus();
		m_pDefault->UIFireEvent(UIEvent_DefaultChanged, (WPARAM)TRUE);
	}

	return 0;
}

inline LRESULT CUIModalRoot::OnRootFocusChanged(int, WPARAM wp, LPARAM, bool&)
{
	if (m_pDefault)
		m_pDefault->UIRedraw();

	_CheckDefaultStatus();
	return 0;
}

inline void CUIModalRoot::_CheckDefaultStatus()
{
	if (m_pDefault)
	{
		bool bDefault = false;
		if (!m_pDefault->UIIsTransparent())
		{
			CUIObject* pFocus = UIGetMgr()->GetFocus();
			bDefault = pFocus == m_pDefault || !UIGetMgr()->GetFocus() || !UIGetMgr()->GetFocus()->UIIsWantEnter();
		}

		m_pDefault->UIAddOrRemoveStatus(bDefault, (DWORD)UIControlStatus_Default);
	}
}
