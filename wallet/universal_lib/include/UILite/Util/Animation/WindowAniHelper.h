// Author: liujing
#pragma once
#include "../../../UILite/UIMsg.h"
#include "../../Core/UIRoot.h"


typedef CRect(*fnAniTrack)(float, LPARAM);
typedef void(*fnAniEnd)(LPARAM);

class CSizeToWnd;
class CWindowAniHelper
{
public:

	UNIVERSAL_LIB_API explicit CWindowAniHelper(CUIRoot* pRoot);
	virtual ~CWindowAniHelper() { }

	BEGIN_MSG_MAP_EX(CWindowAniHelper)
		m_hRegisterWnd = hWnd;
		MESSAGE_HANDLER_EX(WM_UILITE_ROOT_INITIALIZED, OnCreate)
		MSG_WM_CLOSE(OnClose)
	END_MSG_MAP()

	BEGIN_UI_EVENT_MAP(CWindowAniHelper)
		UI_EVENT_HANDLER(UIEvent_ModalAniInit, OnUIModalAniInit)
		UI_EVENT_HANDLER(UIEvent_ModalAniUninit, OnUIModalAniUninit)
	END_UI_EVENT_MAP()

	UNIVERSAL_LIB_API LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);
	UNIVERSAL_LIB_API void OnClose();
	LRESULT OnUIModalAniInit(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled)
	{
		m_bDialog = true;
		return 1;
	}
	UNIVERSAL_LIB_API LRESULT OnUIModalAniUninit(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	// bFade=true:关闭窗口动画 =false:创建窗口动画
	UNIVERSAL_LIB_API void SetAniTrack(fnAniTrack pfnTrack, bool bFade);
	UNIVERSAL_LIB_API void SetAniEnd(fnAniEnd pfnEnd, LPARAM lParam, bool bFade);

	virtual bool IsWindowAniAvailable() { return true; }
	virtual HBITMAP GetShadowBitmap() { return NULL; }
	UNIVERSAL_LIB_API virtual HBITMAP PrintAniWindow();

	UNIVERSAL_LIB_API void DoShowAnimation(fnAniEnd pfnEnd, LPARAM lParam);
	UNIVERSAL_LIB_API void DoHideAnimation(fnAniEnd pfnEnd, LPARAM lParam);

private:

	void DoCreateAnimation();
	void DoCloseAnimation();
	int GetShadowBorder();

	HWND m_hRegisterWnd;
	CUIRoot* m_pRoot;
	bool m_bDialog;

	CSizeToWnd* m_pSizeToWnd;
	CSizeToWnd* m_pSizeToWndClose;

	fnAniEnd m_pfnAniClose;	// Aero下窗口关闭无动画回调
	fnAniEnd m_pfnAniCreate;	// Aero下窗口关闭无动画回调
	LPARAM	m_lParamClose;
	LPARAM	m_lParamCreate;
};
