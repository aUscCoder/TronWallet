// Author: ChenJianjun
#pragma once
#include "../../CommonLib/UI/Paint.h"
#include "../../Universal.h"
#include "UICoreDef.h"


#define _M_PI	(3.1415926)

#define SAFE_DELETE_PTR(p)	\
	{ \
		if (p)				\
		{					\
			delete p;		\
			p = NULL;		\
		} \
	}

inline void NormalizeRect(RECT& rc)
{
	rc.right = std::max(rc.left, rc.right);
	rc.bottom = std::max(rc.top, rc.bottom);
}

class CValueFromStatus
{
public:

	CValueFromStatus() { memset(this, 0, sizeof(CValueFromStatus)); }

	CValueFromStatus(int iNormalVal, int iHoverVal, int iFocusVal, int iPressedVal, int iDisabledVal, int iActiveVal = 0)
	{
		m_iVal[UIIndex_Normal] = iNormalVal;
		m_iVal[UIIndex_Hover] = iHoverVal;
		m_iVal[UIIndex_Focus] = iFocusVal;
		m_iVal[UIIndex_Pressed] = iPressedVal;
		m_iVal[UIIndex_Disabled] = iDisabledVal;
		m_iVal[UIIndex_Active] = iActiveVal;
	}

	~CValueFromStatus() {}

	const int& operator[](unsigned short idx) const;

	UNIVERSAL_LIB_API int GetValue(DWORD dwStatus) const;

private:

	int m_iVal[6];
};

class CUIStatsImageFrameInfo: public CValueFromStatus
{

public:

	CUIStatsImageFrameInfo()
	{
		m_cFrameCount = 1;
	}

	CUIStatsImageFrameInfo(int iCount, int iNormal, int iHover, int iFocus, int iPressed, int iDisabled, int iActive = 0) :
		CValueFromStatus(iNormal, iHover, iFocus, iPressed, iDisabled, iActive), m_cFrameCount(iCount) {}

	~CUIStatsImageFrameInfo() {}

	const int& operator[](unsigned short idx) const { return (*(CValueFromStatus*)this)[idx]; }

	int count() const{ return m_cFrameCount; }

	Draw9Frame ToFraw9Frame(DWORD dwStates) const { return Draw9Frame(GetValue(dwStates), m_cFrameCount); }

private:

	int m_cFrameCount;
};

typedef CUIStatsImageFrameInfo CStatusImageInfo;

struct CDPIDevice
{
	explicit CDPIDevice(HWND h)	{ m_hWnd = h; m_hDC = ::GetDC(m_hWnd); }
	~CDPIDevice() { ::ReleaseDC(m_hWnd, m_hDC); }
	operator HDC() { return m_hDC; }
	HWND m_hWnd;
	HDC m_hDC;
};

class CBackupClipRgn
{
public:

	CBackupClipRgn(CDCHandle dc, const RECT& rcNewClip) : m_dc(dc)
	{
		m_rgnOld.CreateRectRgn(0, 0, 1, 1);
		m_dc.GetClipRgn(m_rgnOld);

		CRgn rgn = ::CreateRectRgnIndirect(&rcNewClip);
		::ExtSelectClipRgn(m_dc, rgn, RGN_AND);
	}

	~CBackupClipRgn()
	{
		m_dc.SelectClipRgn(m_rgnOld);
	}

private:

	CDCHandle m_dc;
	CRgn m_rgnOld;
};

namespace UI
{
	namespace Util
	{
		namespace Paint
		{
			// iState - BACKGROUND_NORMAL / BACKGROUND_HOVER / BACKGROUND_FOCUS / BACKGROUND_PRESSED / BACKGROUND_DISABLED / BACKGROUND_NOPRESSED_FOCUS
			UNIVERSAL_LIB_API bool DrawBackgroundWithStats(HDC dc, const RECT& rc, int iState);
		}

		UNIVERSAL_LIB_API HCURSOR GetCursorArrow();
		UNIVERSAL_LIB_API HCURSOR GetCursorHand();
		UNIVERSAL_LIB_API HCURSOR GetCursorIBeam();
		UNIVERSAL_LIB_API HCURSOR GetCursorSizeWE();
		UNIVERSAL_LIB_API HCURSOR GetCursorSizeNS();
		UNIVERSAL_LIB_API HCURSOR GetCursorSizeAll();
		
		inline int DPtoLPX(HDC hdc, int iX) { return ::MulDiv(iX, 1440, ::GetDeviceCaps(hdc, LOGPIXELSX)); }
		inline int LPtoDPX(HDC hdc, int iX) { return ::MulDiv(iX, ::GetDeviceCaps(hdc, LOGPIXELSX), 1440); }
		inline int SPtoLPX(int iX) { return DPtoLPX(CDPIDevice(NULL), iX); }
		inline int LPtoSPX(int iX) { return LPtoDPX(CDPIDevice(NULL), iX); }

		inline int DPtoLPY(HDC hdc, int iY) { return ::MulDiv(iY, 1440, ::GetDeviceCaps(hdc, LOGPIXELSY)); }
		inline int LPtoDPY(HDC hdc, int iY) { return ::MulDiv(iY, ::GetDeviceCaps(hdc, LOGPIXELSY), 1440); }
		inline int SPtoLPY(int iY) { return DPtoLPY(CDPIDevice(NULL), iY); }
		inline int LPtoSPY(int iY) { return LPtoDPY(CDPIDevice(NULL), iY); }

		UNIVERSAL_LIB_API void EnableFakeMouseMove(bool bEnable = true);
		UNIVERSAL_LIB_API void PostFakeMouseMove(HWND hWnd);

		__int64 GetSystemRunningTime();
	}
}

#define UICHECK_OBJECT_VALID() UIASSERT(m_dwValidSignature___ == 0x19860403 && L"Object used after deconstruct!")

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class CTracePaintCost
{
public:

	CTracePaintCost();
	~CTracePaintCost();

private:

	__int64 m_iStart;
};
