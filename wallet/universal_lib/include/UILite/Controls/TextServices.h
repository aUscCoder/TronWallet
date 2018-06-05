// Author: yangqi
/**
 \file	Controls\TextServices.h
 \author yangqi
 \brief	Declares the text services class.
 */

#pragma once
#include <textserv.h>
#include <tom.h>

/**
 \class	CTextServices

 \brief	A wrapper of ITextServices.

 \author	yangqi
 \date	2016-03-10
 */

class CTextServices
{
public:
	explicit CTextServices(CComPtr<ITextHost> spTextHost);
	virtual ~CTextServices();

	bool IsWorking() const;

	//@cmember Generic Send Message interface
	HRESULT 	TxSendMessage(
		UINT msg,
		WPARAM wparam,
		LPARAM lparam,
		LRESULT *plresult);

	//@cmember Rendering
	HRESULT		TxDraw(
		DWORD dwDrawAspect,
		LONG  lindex,
		void * pvAspect,
		DVTARGETDEVICE * ptd,
		HDC hdcDraw,
		HDC hicTargetDev,
		LPCRECTL lprcBounds,
		LPCRECTL lprcWBounds,
		LPRECT lprcUpdate,
		BOOL(CALLBACK * pfnContinue) (DWORD),
		DWORD dwContinue,
		LONG lViewId);

	//@cmember Horizontal scrollbar support
	HRESULT		TxGetHScroll(
		LONG *plMin,
		LONG *plMax,
		LONG *plPos,
		LONG *plPage,
		BOOL * pfEnabled);

	//@cmember Horizontal scrollbar support
	HRESULT		TxGetVScroll(
		LONG *plMin,
		LONG *plMax,
		LONG *plPos,
		LONG *plPage,
		BOOL * pfEnabled);

	//@cmember Setcursor
	HRESULT 	OnTxSetCursor(
		DWORD dwDrawAspect,
		LONG  lindex,
		void * pvAspect,
		DVTARGETDEVICE * ptd,
		HDC hdcDraw,
		HDC hicTargetDev,
		LPCRECT lprcClient,
		INT x,
		INT y);

	//@cmember Hit-test
	HRESULT 	TxQueryHitPoint(
		DWORD dwDrawAspect,
		LONG  lindex,
		void * pvAspect,
		DVTARGETDEVICE * ptd,
		HDC hdcDraw,
		HDC hicTargetDev,
		LPCRECT lprcClient,
		INT x,
		INT y,
		DWORD * pHitResult);

	//@cmember Inplace activate notification
	HRESULT		OnTxInPlaceActivate(LPCRECT prcClient);

	//@cmember Inplace deactivate notification
	HRESULT		OnTxInPlaceDeactivate();

	//@cmember UI activate notification
	HRESULT		OnTxUIActivate();

	//@cmember UI deactivate notification
	HRESULT		OnTxUIDeactivate();

	//@cmember Get text in control
	HRESULT		TxGetText(BSTR *pbstrText);

	//@cmember Set text in control
	HRESULT		TxSetText(LPCWSTR pszText);

	//@cmember Get x position of
	HRESULT		TxGetCurTargetX(LONG *);
	//@cmember Get baseline position
	HRESULT		TxGetBaseLinePos(LONG *);

	//@cmember Get Size to fit / Natural size
	HRESULT		TxGetNaturalSize(
		DWORD dwAspect,
		HDC hdcDraw,
		HDC hicTargetDev,
		DVTARGETDEVICE *ptd,
		DWORD dwMode,
		const SIZEL *psizelExtent,
		LONG *pwidth,
		LONG *pheight);

	//@cmember Drag & drop
	HRESULT		TxGetDropTarget(IDropTarget **ppDropTarget);

	//@cmember Bulk bit property change notifications
	HRESULT		OnTxPropertyBitsChange(DWORD dwMask, DWORD dwBits);

	//@cmember Fetch the cached drawing size
	HRESULT		TxGetCachedSize(DWORD *pdwWidth, DWORD *pdwHeight);

private:
	bool CreateService();
	bool CheckComIterface();
	int ExceptionFilter();

private:
	CComPtr<ITextHost> m_spTextHost;
	ITextServices* m_pTextServ = nullptr; // 为了避免 ExceptionFilter() 中操作该指针时出现异常，特意不用 CComPtr
};
