// Author: chenjianjun
#pragma once
#include "../../Universal.h"


class CShadow
{
public:

	UNIVERSAL_LIB_API CShadow();
	UNIVERSAL_LIB_API ~CShadow();

	UNIVERSAL_LIB_API void SetShadow(HBITMAP hbm);
	UNIVERSAL_LIB_API void SetShadowActivateStatus(bool bActivated);
	UNIVERSAL_LIB_API void UpdateShadowAlpha(int iAlpha);
	HBITMAP GetShadow() { return m_bmCustomShadow; }

	BEGIN_MSG_MAP(CShadow)
		m_hWndHost = hWnd;
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_INITDIALOG, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY, OnNCDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_NCACTIVATE, OnNcActivate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
//		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanged)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, OnWindowPosChanged)
	END_MSG_MAP()

	UNIVERSAL_LIB_API LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	UNIVERSAL_LIB_API LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	UNIVERSAL_LIB_API LRESULT OnNCDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	UNIVERSAL_LIB_API LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	UNIVERSAL_LIB_API LRESULT OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	UNIVERSAL_LIB_API LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	UNIVERSAL_LIB_API LRESULT OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	UNIVERSAL_LIB_API LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:

	bool IsShadowEnabled();
	bool CanShowShadow();
	void CreateShadow();
	void DestroyShadow();
	void UpdateShadow();
	bool IsDelayShowShadow();

	enum ShadowDirection
	{
		SD_Left = 0,
		SD_Top,
		SD_Right,
		SD_Bottom,
		SD_Count,
	};

	bool m_bDelay;
	bool m_bShadowEnable;
	bool m_bCreated;

	bool m_bForceActivated;
	bool m_bActivated;
	bool m_bNcActivated;

	HWND m_hWndHost;
	HWND m_hWndShadow[SD_Count];
	int	 m_iShadowAlpha;
	int  m_iUpdatedAlpha;

	CBitmapHandle m_bmCustomShadow;
};
