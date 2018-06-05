// Author: tuotuoxp
#pragma once


typedef int (WINAPI *_DTT_CALLBACK_PROC)(
	__in HDC hdc,
	__inout_ecount(cchText) LPWSTR pszText,
	__in int cchText,
	__inout LPRECT prc,
	__in UINT dwFlags,
	__in LPARAM lParam);



// ---- bits used in dwFlags of DTTOPTS ----
#define _DTT_TEXTCOLOR       (1UL << 0)      // crText has been specified
#define _DTT_BORDERCOLOR     (1UL << 1)      // crBorder has been specified
#define _DTT_SHADOWCOLOR     (1UL << 2)      // crShadow has been specified
#define _DTT_SHADOWTYPE      (1UL << 3)      // iTextShadowType has been specified
#define _DTT_SHADOWOFFSET    (1UL << 4)      // ptShadowOffset has been specified
#define _DTT_BORDERSIZE      (1UL << 5)      // iBorderSize has been specified
#define _DTT_FONTPROP        (1UL << 6)      // iFontPropId has been specified
#define _DTT_COLORPROP       (1UL << 7)      // iColorPropId has been specified
#define _DTT_STATEID         (1UL << 8)      // IStateId has been specified
#define _DTT_CALCRECT        (1UL << 9)      // Use pRect as and in/out parameter
#define _DTT_APPLYOVERLAY    (1UL << 10)     // fApplyOverlay has been specified
#define _DTT_GLOWSIZE        (1UL << 11)     // iGlowSize has been specified
#define _DTT_CALLBACK        (1UL << 12)     // pfnDrawTextCallback has been specified
#define _DTT_COMPOSITED      (1UL << 13)     // Draws text with antialiased alpha (needs a DIB section)
#define _DTT_VALIDBITS       (_DTT_TEXTCOLOR | \
								_DTT_BORDERCOLOR | \
								_DTT_SHADOWCOLOR | \
								_DTT_SHADOWTYPE | \
								_DTT_SHADOWOFFSET | \
								_DTT_BORDERSIZE | \
								_DTT_FONTPROP | \
								_DTT_COLORPROP | \
								_DTT_STATEID | \
								_DTT_CALCRECT | \
								_DTT_APPLYOVERLAY | \
								_DTT_GLOWSIZE | \
								_DTT_COMPOSITED)


struct __DTTOPTS
{
	DWORD             dwSize;              // size of the struct
	DWORD             dwFlags;             // which options have been specified
	COLORREF          crText;              // color to use for text fill
	COLORREF          crBorder;            // color to use for text outline
	COLORREF          crShadow;            // color to use for text shadow
	int               iTextShadowType;     // TST_SINGLE or TST_CONTINUOUS
	POINT             ptShadowOffset;      // where shadow is drawn (relative to text)
	int               iBorderSize;         // Border radius around text
	int               iFontPropId;         // Font property to use for the text instead of TMT_FONT
	int               iColorPropId;        // Color property to use for the text instead of TMT_TEXTCOLOR
	int               iStateId;            // Alternate state id
	BOOL              fApplyOverlay;       // Overlay text on top of any text effect?
	int               iGlowSize;           // Glow radious around text
	_DTT_CALLBACK_PROC pfnDrawTextCallback; // Callback for DrawText
	LPARAM            lParam;              // Parameter for callback
};



#define DECLARE_THEME_FUNC_PTR(functype, ptr, name) \
	if (!CheckThemeAPI()) \
		return 0; \
	static functype ptr = NULL; \
	if (ptr == (functype)-1) \
		return 0; \
	if (ptr == NULL) \
	{ \
		ptr = (functype)::GetProcAddress(g_hThemeDLL, name); \
		if (ptr == NULL) \
		{ \
			ptr = (functype)-1; \
			return 0; \
		} \
	}

__declspec(selectany) HMODULE g_hThemeDLL = NULL;


inline bool CheckThemeAPI()
{
	if (g_hThemeDLL == (HMODULE)-1)
		return false;
	if (g_hThemeDLL == NULL)
	{
		TCHAR szDLLPath[MAX_PATH];
		DWORD dwPathLen = ::SearchPath(NULL, _T("UxTheme.dll"), NULL, _countof(szDLLPath), szDLLPath, NULL);
		if (dwPathLen > 0)
			g_hThemeDLL = ::LoadLibrary(szDLLPath);
		if (g_hThemeDLL == (HMODULE)-1)
			return false;
	}
	return true;
}

inline HRESULT TuoDrawThemeTextEx(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwFlags,
	LPRECT pRect, const __DTTOPTS *pOptions)
{
	typedef HRESULT (WINAPI * f)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwFlags,
		LPRECT pRect, const __DTTOPTS *pOptions);
	DECLARE_THEME_FUNC_PTR(f, p, "DrawThemeTextEx")
	return p(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwFlags, pRect, pOptions);
}

inline HTHEME TuoOpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	typedef HTHEME (WINAPI * f)(HWND hwnd, LPCWSTR pszClassList);
	DECLARE_THEME_FUNC_PTR(f, p, "OpenThemeData")
	return p(hwnd, pszClassList);
}

inline HRESULT TuoCloseThemeData(HTHEME hTheme)
{
	typedef HRESULT (WINAPI * f)(HTHEME hTheme);
	DECLARE_THEME_FUNC_PTR(f, p, "CloseThemeData")
	return p(hTheme);
}

inline HRESULT TuoDrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, __in_opt LPCRECT pClipRect)
{
	typedef HRESULT (WINAPI * f)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, __in_opt LPCRECT pClipRect);
	DECLARE_THEME_FUNC_PTR(f, p, "DrawThemeBackground")

	return p(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

inline HRESULT TuoGetThemePartSize(HTHEME hTheme, __in_opt HDC hdc, int iPartId, int iStateId, __in_opt LPCRECT prc,
	enum THEMESIZE eSize, __out SIZE *psz)
{
	typedef HRESULT (WINAPI * f)(HTHEME hTheme, __in_opt HDC hdc, int iPartId, int iStateId, __in_opt LPCRECT prc,
		enum THEMESIZE eSize, __out SIZE *psz);

	DECLARE_THEME_FUNC_PTR(f, p, "GetThemePartSize")
	return p(hTheme, hdc, iPartId, iStateId, prc, eSize, psz);
};

//////////////////////////////////////////////////////////////////////////
// cjj

class CxxTheme
{
public:
	CxxTheme(HWND hWnd, LPCWSTR lpszClassList) { m_hTheme = ::TuoOpenThemeData(hWnd, lpszClassList); }
	~CxxTheme() { ::TuoCloseThemeData(m_hTheme); }

	operator HTHEME() { return m_hTheme; }

private:

	HTHEME m_hTheme;
};
