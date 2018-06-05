// Author: tuotuoXP
#pragma once


#define WM_DWMSENDICONICTHUMBNAIL           0x0323
#define WM_DWMSENDICONICLIVEPREVIEWBITMAP   0x0326
#define WM_DWMCOMPOSITIONCHANGED			0x031E



#define DWM_TNP_RECTDESTINATION       0x00000001
#define DWM_TNP_RECTSOURCE            0x00000002
#define DWM_TNP_OPACITY               0x00000004
#define DWM_TNP_VISIBLE               0x00000008
#define DWM_TNP_SOURCECLIENTAREAONLY  0x00000010


#define MSGFLT_ADD		1



// Window attributes
enum _DWMWINDOWATTRIBUTE
{
	DWMWA_NCRENDERING_ENABLED = 1,      // [get] Is non-client rendering enabled/disabled
	DWMWA_NCRENDERING_POLICY,           // [set] Non-client rendering policy
	DWMWA_TRANSITIONS_FORCEDISABLED,    // [set] Potentially enable/forcibly disable transitions
	DWMWA_ALLOW_NCPAINT,                // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
	DWMWA_CAPTION_BUTTON_BOUNDS,        // [get] Bounds of the caption button area in window-relative space.
	DWMWA_NONCLIENT_RTL_LAYOUT,         // [set] Is non-client content RTL mirrored
	DWMWA_FORCE_ICONIC_REPRESENTATION,  // [set] Force this window to display iconic thumbnails.
	DWMWA_FLIP3D_POLICY,                // [set] Designates how Flip3D will treat the window.
	DWMWA_EXTENDED_FRAME_BOUNDS,        // [get] Gets the extended frame bounds rectangle in screen space
	DWMWA_HAS_ICONIC_BITMAP,            // [set] Indicates an available bitmap when there is no better thumbnail representation.
	DWMWA_DISALLOW_PEEK,                // [set] Don't invoke Peek on the window.
	DWMWA_EXCLUDED_FROM_PEEK,           // [set] LivePreview exclusion information
	DWMWA_LAST
};


enum _THUMBBUTTONFLAGS
{
	_THBF_ENABLED		= 0,
	_THBF_DISABLED		= 0x1,
	_THBF_DISMISSONCLICK	= 0x2,
	_THBF_NOBACKGROUND	= 0x4,
	_THBF_HIDDEN			= 0x8,
	_THBF_NONINTERACTIVE	= 0x10
};


enum _THUMBBUTTONMASK
{
	_THB_BITMAP	= 0x1,
	_THB_ICON	= 0x2,
	_THB_TOOLTIP	= 0x4,
	_THB_FLAGS	= 0x8
};


enum _TBPFLAG
{
	_TBPF_NOPROGRESS	= 0,
	_TBPF_INDETERMINATE	= 0x1,
	_TBPF_NORMAL	= 0x2,
	_TBPF_ERROR	= 0x4,
	_TBPF_PAUSED	= 0x8
};


enum _STPFLAG
{
	_STPF_NONE	= 0,
	_STPF_USEAPPTHUMBNAILALWAYS	= 0x1,
	_STPF_USEAPPTHUMBNAILWHENACTIVE	= 0x2,
	_STPF_USEAPPPEEKALWAYS	= 0x4,
	_STPF_USEAPPPEEKWHENACTIVE	= 0x8
};

enum _DWMNCRENDERINGPOLICY
{
	_DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
	_DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
	_DWMNCRP_ENABLED,        // Enabled non-client rendering; window style is ignored
	_DWMNCRP_LAST
};


typedef HANDLE HTHUMBNAIL;

struct _DWM_THUMBNAIL_PROPERTIES
{
	DWORD dwFlags;
	RECT rcDestination;
	RECT rcSource;
	BYTE opacity;
	BOOL fVisible;
	BOOL fSourceClientAreaOnly;
};


struct _TMARGINS
{
	int cxLeftWidth;      // width of left border that retains its size
	int cxRightWidth;     // width of right border that retains its size
	int cyTopHeight;      // height of top border that retains its size
	int cyBottomHeight;   // height of bottom border that retains its size
};



#include <pshpack8.h>
typedef struct _THUMBBUTTON
{
	_THUMBBUTTONMASK dwMask;
	UINT iId;
	UINT iBitmap;
	HICON hIcon;
	WCHAR szTip[ 260 ];
	_THUMBBUTTONFLAGS dwFlags;
} 	_THUMBBUTTON;

typedef struct _THUMBBUTTON *_LPTHUMBBUTTON;
#include <poppack.h> // NOLINT (1.4.4 xx)


MIDL_INTERFACE("ea1afb91-9e28-4b86-90e9-9e9f8a5eefaf")
_ITaskbarList3 : public ITaskbarList2
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetProgressValue(HWND hwnd, ULONGLONG ullCompleted, ULONGLONG ullTotal) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProgressState(HWND hwnd, _TBPFLAG tbpFlags) = 0;
	virtual HRESULT STDMETHODCALLTYPE RegisterTab(HWND hwndTab, HWND hwndMDI) = 0;
	virtual HRESULT STDMETHODCALLTYPE UnregisterTab(HWND hwndTab) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetTabOrder(HWND hwndTab, HWND hwndInsertBefore) = 0;

	virtual HRESULT STDMETHODCALLTYPE SetTabActive(HWND hwndTab, HWND hwndMDI, DWORD dwReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE ThumbBarAddButtons(HWND hwnd, UINT cButtons, _LPTHUMBBUTTON pButton) = 0;
	virtual HRESULT STDMETHODCALLTYPE ThumbBarUpdateButtons(HWND hwnd, UINT cButtons, _LPTHUMBBUTTON pButton) = 0;
	virtual HRESULT STDMETHODCALLTYPE ThumbBarSetImageList(HWND hwnd, HIMAGELIST himl) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetOverlayIcon(HWND hwnd, HICON hIcon, LPCWSTR pszDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetThumbnailTooltip(HWND hwnd, LPCWSTR pszTip) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetThumbnailClip(HWND hwnd, RECT *prcClip) = 0;
};


MIDL_INTERFACE("c43dc798-95d1-4bea-9030-bb99e2983a1a")
_ITaskbarList4 : public _ITaskbarList3
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetTabProperties(HWND hwndTab, _STPFLAG stpFlags) = 0;
};


//////////////////////////////////////////////////////////////////////////

struct _PROPERTYKEY
{
	GUID fmtid;
	DWORD pid;
};


MIDL_INTERFACE("886d8eeb-8cf2-4446-8d02-cdba1dbdcf99")
_IPropertyStore : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE GetCount(DWORD *cProps) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetAt(DWORD iProp, _PROPERTYKEY *pkey) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetValue(const _PROPERTYKEY &key, PROPVARIANT *pv) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetValue(const _PROPERTYKEY &key, const PROPVARIANT &propvar) = 0;
	virtual HRESULT STDMETHODCALLTYPE Commit() = 0;
};

//////////////////////////////////////////////////////////////////////////

__declspec(selectany) HMODULE g_hDWMModule = (HMODULE)-1;
__declspec(selectany) HMODULE g_hUxThemeModule = (HMODULE)-1;

#pragma warning(push)
#pragma warning(disable:4996)
inline HMODULE GetDWMModule()
{
	if (g_hDWMModule == (HMODULE)-1)
	{
		OSVERSIONINFO os = { sizeof(OSVERSIONINFO) };
		if (::GetVersionEx(&os) && os.dwMajorVersion >= 6)
			g_hDWMModule = ::LoadLibraryW(L"dwmapi.dll");
		else
			g_hDWMModule = NULL;
	}
	return g_hDWMModule;
}
#pragma warning(pop)


#define DWM_API(TYPE, NAME, ARGS, CALLARGS) \
	typedef TYPE (WINAPI * __t##NAME) ARGS; \
	__declspec(selectany) __t##NAME __f##NAME = (__t##NAME)-1; \
	static TYPE Tuo##NAME ARGS \
	{ \
		if (__f##NAME == (__t##NAME)-1) \
		{ \
			HMODULE h = ::GetDWMModule(); \
			if (h) \
				__f##NAME = (__t##NAME)::GetProcAddress(h, #NAME); \
			else \
				__f##NAME = NULL; \
		} \
		if (__f##NAME) \
			return __f##NAME CALLARGS; \
		return (TYPE)(0); \
	}

DWM_API(HRESULT, DwmRegisterThumbnail, (HWND hwndDest, HWND hwndSource, HTHUMBNAIL *phThumbnailId), (hwndDest, hwndSource, phThumbnailId))
DWM_API(HRESULT, DwmUpdateThumbnailProperties, (HTHUMBNAIL hThumbnailId, const _DWM_THUMBNAIL_PROPERTIES* ptnProps), (hThumbnailId, ptnProps))
DWM_API(HRESULT, DwmUnregisterThumbnail, (HTHUMBNAIL hThumbnailId), (hThumbnailId))
DWM_API(HRESULT, DwmSetIconicThumbnail, (HWND hwnd, HBITMAP hbmp, DWORD dwSITFlags), (hwnd, hbmp, dwSITFlags))
DWM_API(HRESULT, DwmSetIconicLivePreviewBitmap, (HWND hwnd, HBITMAP hbmp, POINT *pptClient, DWORD dwSITFlags), (hwnd, hbmp, pptClient, dwSITFlags))
DWM_API(HRESULT, DwmSetWindowAttribute, (HWND hwnd, DWORD dwAttrib, LPCVOID pvAttrib, DWORD cbAttrib), (hwnd, dwAttrib, pvAttrib, cbAttrib))
DWM_API(HRESULT, DwmInvalidateIconicBitmaps, (HWND hwnd), (hwnd))
DWM_API(HRESULT, DwmExtendFrameIntoClientArea, (HWND hWnd, const _TMARGINS *pMarInset), (hWnd, pMarInset))
DWM_API(HRESULT, DwmDefWindowProc, (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult), (hwnd, msg, wParam, lParam, plResult))
DWM_API(HRESULT, DwmIsCompositionEnabled, (BOOL *pfEnabled), (pfEnabled))
DWM_API(HRESULT, DwmGetWindowAttribute, (HWND hwnd, DWORD dwAttrib, PVOID pvAttrib, DWORD cbAttrib), (hwnd, dwAttrib, pvAttrib, cbAttrib))


//////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable:4996)
inline HMODULE GetUxThemeModule()
{
	if (g_hUxThemeModule == (HMODULE)-1)
	{
		OSVERSIONINFO os = { sizeof(OSVERSIONINFO) };
		if (::GetVersionEx(&os) && os.dwMajorVersion >= 6)
			g_hUxThemeModule = ::LoadLibraryW(L"UxTheme.dll");
		else
			g_hUxThemeModule = NULL;
	}
	return g_hUxThemeModule;
}
#pragma warning(pop)

#define UxTheme_API(TYPE, NAME, ARGS, CALLARGS) \
	typedef TYPE (WINAPI * __t##NAME) ARGS; \
	__declspec(selectany) __t##NAME __f##NAME = (__t##NAME)-1; \
	static TYPE Tuo##NAME ARGS \
{ \
	if (__f##NAME == (__t##NAME)-1) \
{ \
	HMODULE h = ::GetUxThemeModule(); \
	if (h) \
	__f##NAME = (__t##NAME)::GetProcAddress(h, #NAME); \
			else \
			__f##NAME = NULL; \
} \
	if (__f##NAME) \
	return __f##NAME CALLARGS; \
	return (TYPE)(0); \
}

UxTheme_API(BOOL,		IsCompositionActive, (), ())
UxTheme_API(HRESULT,	GetThemeStream, (HTHEME hTheme, int iPartId, int iStateId, int iPropId,
				VOID **ppvStream, DWORD* pcbStream, HINSTANCE hInst), (hTheme, iPartId, iStateId, iPropId, ppvStream, pcbStream, hInst));
UxTheme_API(HRESULT,	GetThemeBitmap, (HTHEME hTheme, int iPartId, int iStateId, int iPropId,
				ULONG dwFlags, HBITMAP* phBitmap), (hTheme, iPartId, iStateId, iPropId, dwFlags, phBitmap));
//////////////////////////////////////////////////////////////////////////


inline bool IsAeroEffectEnabled()
{
	BOOL bEnable = FALSE;
	::TuoDwmIsCompositionEnabled(&bEnable);
	return bEnable != FALSE;
}


typedef BOOL (WINAPI * __tChangeWindowMessageFilter)(UINT message, DWORD dwFlag);
__declspec(selectany) __tChangeWindowMessageFilter __fChangeWindowMessageFilter = (__tChangeWindowMessageFilter)-1;

static BOOL TuoChangeWindowMessageFilter(UINT message, DWORD dwFlag) // NOLINT (2.3.1 xx)
{
	if (__fChangeWindowMessageFilter == (__tChangeWindowMessageFilter)-1)
	{
		HMODULE hUser32 = ::LoadLibrary(L"user32.dll");
		if (hUser32)
			__fChangeWindowMessageFilter = (__tChangeWindowMessageFilter)::GetProcAddress(hUser32, "ChangeWindowMessageFilter");
		else
			__fChangeWindowMessageFilter = NULL;
	}
	if (__fChangeWindowMessageFilter)
		return __fChangeWindowMessageFilter(message, dwFlag);
	return FALSE;
}



typedef HRESULT (WINAPI * __tSHGetPropertyStoreForWindow)(HWND hwnd, REFIID riid, void **ppv);
static __tSHGetPropertyStoreForWindow __fSHGetPropertyStoreForWindow = (__tSHGetPropertyStoreForWindow)-1;

static void TuoSetWindowAppName(HWND hWnd, LPCTSTR lpszAppName) // NOLINT (2.3.1 xx)
{
	if (__fSHGetPropertyStoreForWindow == (__tSHGetPropertyStoreForWindow)-1)
	{
		HMODULE hShell32 = ::LoadLibrary(L"shell32.dll");
		if (hShell32)
			__fSHGetPropertyStoreForWindow = (__tSHGetPropertyStoreForWindow)::GetProcAddress(hShell32, "SHGetPropertyStoreForWindow");
		else
			__fSHGetPropertyStoreForWindow = NULL;
	}

	if (__fSHGetPropertyStoreForWindow)
	{
		static const _PROPERTYKEY PKEY_AppUserModel_ID = { { 0x9F4C2855, 0x9F79, 0x4B39, { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } }, 5 };
		PROPVARIANT pv;
		pv.vt = VT_BSTR;
		pv.bstrVal = (BSTR)lpszAppName;
		_IPropertyStore *pps;
		if (SUCCEEDED(__fSHGetPropertyStoreForWindow(hWnd, __uuidof(_IPropertyStore), (void**)&pps)))
		{
			pps->SetValue(PKEY_AppUserModel_ID, pv);
			pps->Release();
		}
	}
}
