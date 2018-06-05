// Author: chenjianjun
#pragma once
#include "../../Universal.h"
#include "Cached.h"
#include "DC.h"


typedef HANDLE HTHEME;


struct Draw9
{
	Draw9() : left(0), right(0), top(0), bottom(0) {}
	Draw9(int l, int r, int t, int b) : left(l), right(r), top(t), bottom(b) {}
	int left, right, top, bottom;

	bool operator == (const Draw9& d) const
	{
		return (left == d.left && right == d.right && top == d.top && bottom == d.bottom);
	}

	bool operator != (const Draw9& d) const
	{
		return (left != d.left || right != d.right || top != d.top || bottom != d.bottom);
	}
};

struct Draw9H : Draw9
{
	Draw9H(int l, int r) : Draw9(l, r, 0, 0) {}
};

struct Draw9V : Draw9
{
	Draw9V(int t, int b) : Draw9(0, 0, t, b) {}
};

struct Draw9Frame
{
	Draw9Frame() : index(0), total(1) {}
	Draw9Frame(int i, int t) : index(i), total(t) { TuoAssert(index < total); }
	RECT ToRect(int w, int h) const
	{
		TuoAssert(w % total == 0);
		int iPicWidth = w / total;
		RECT rcSrc = { iPicWidth * index, 0, rcSrc.left + iPicWidth, h };
		return rcSrc;
	}
	RECT ToRect(HBITMAP hbmpSrc) const
	{
		BITMAP bm = { 0 };
		if (::GetObject(hbmpSrc, sizeof(BITMAP), &bm) == sizeof(BITMAP))
			return ToRect(bm.bmWidth, bm.bmHeight);
		else
			return ToRect(0, 0);
	}
	int index, total;
};


// 九宫格均分，从图片src的中央切分
//
// +---------------+-----------+---------------+
// | (w/2-1,h/2-1) | (1,h/2-1) | (w/2-1,h/2-1) |
// +---------------+-----------+---------------+
// |   (w/2-1,1)   |   (1,1)   |    (w/2,1)    |
// +---------------+-----------+---------------+
// | (w/2-1,h/2-1) | (1,h/2-1) | (w/2-1,h/2-1) |
// +---------------+-----------+---------------+

struct Draw9HCenter : Draw9H { Draw9HCenter() : Draw9H(-1, -1) {} };
struct Draw9VCenter : Draw9V { Draw9VCenter() : Draw9V(-1, -1) {} };
struct Draw9Center : Draw9 { Draw9Center() : Draw9(-1, -1, -1, -1) {} };

enum DrawOption
{
	DrawOption_UseBlt			= 0x10000000,
	DrawOption_NoCheckWidth		= 0x20000000,	// 使用Draw9V时，会检查dest宽度与原图一致，使用此选项则不会检查
	DrawOption_NoCheckHeight	= 0x40000000,	// 使用Draw9H时，会检查dest高度与原图一致，使用此选项则不会检查
	DrawOption_HCenterImgToDest	= 0x01000000,	// 画图的时候图片水平居中在dest上，不得与Draw9H/no check width同时使用
	DrawOption_VCenterImgToDest	= 0x02000000,	// 画图的时候图片垂直居中在dest上，不得与Draw9V/no check height同时使用
	DrawOption_Opaque			= 0x000000ff,
};
inline DrawOption operator | (const DrawOption opt1, const DrawOption opt2) { return (DrawOption)((DWORD)opt1 | (DWORD)opt2); }

#define DRAWOPT_ALPHA(alpha)			((DrawOption)((alpha) & 0xff))
#define DRAWOPT_NO_WIDTH_CHECK()		(DrawOption_NoCheckWidth | DrawOption_Opaque)
#define DRAWOPT_NO_HEIGHT_CHECK()		(DrawOption_NoCheckHeight | DrawOption_Opaque)
#define DRAWOPT_NO_CHECK()				(DrawOption_NoCheckWidth | DrawOption_NoCheckHeight | DrawOption_Opaque)
#define DRAWOPT_HCENTER_TO_DEST()		(DrawOption_HCenterImgToDest | DrawOption_Opaque)
#define DRAWOPT_VCENTER_TO_DEST()		(DrawOption_VCenterImgToDest | DrawOption_Opaque)
#define DRAWOPT_CENTER_TO_DEST()		(DrawOption_HCenterImgToDest | DrawOption_VCenterImgToDest | DrawOption_Opaque)


enum DrawFormat
{
	DrawFormat_Default			= 0,
	DrawFormat_Opaque			= 0x00000080,	// 不透明，文字区域的alpha通道会被填充为255，可以用在LayeredWindow的不透明区域，
												// 性能比用DrawFormat_WithAlpha高
	DrawFormat_WithAlpha		= 0x00000100,	// 使用独立的buffer，这样会降低性能，但对目标dc的alpha通道友好
	DrawFormat_VCenter			= 0x00000200,	// 垂直居中
	DrawFormat_Bottom			= 0x00000400,	// 底部对齐
	DrawFormat_Center			= 0x00000800,	// 水平居中，这个flag会影响文字绘制效率
	DrawFormat_Right			= 0x00001000,	// 右对齐
	DrawFormat_FadeOut			= 0x00002000,	// 画不下的时候最后一个字符渐淡来处理，隐含DrawFormat_WithAlpha
	DrawFormat_Truncate			= 0x00004000,	// 截断，这样的话会就不会出现画出半个文字的情况
	DrawFormat_EndEllipsis		= 0x00008000,	// 如果绘制不下的话以“...”结束
};
inline DrawFormat operator | (const DrawFormat opt1, const DrawFormat opt2) { return (DrawFormat)((DWORD)opt1 | (DWORD)opt2); }
#define DRAWFORMAT_ALL	(DrawFormat_Opaque | DrawFormat_WithAlpha | DrawFormat_VCenter | \
							DrawFormat_Center | DrawFormat_FadeOut | DrawFormat_Truncate |\
							DrawFormat_Bottom | DrawFormat_Right | DrawFormat_EndEllipsis)

namespace Universal
{
	namespace UI
	{
		namespace Paint
		{
			// rcSrc                                                rcDest
			//         |    |            -+                                 |         |            -+
			//         |    |             +-- d.top                         |         |             +-- d.top
			// --------+----+---------   -+                         --------+---------+---------   -+
			//         | @@ |                                               | @@@@@@@ |
			// --------+----+---------   -+                                 | @@@@@@@ |
			//         |    |             +-- d.bottom                      | @@@@@@@ |
			//         |    |            -+                         --------+---------+---------   -+
			//                                                              |         |             +-- d.bottom
			// |       |    |        |                                      |         |            -+
			// +---+---+    +---+----+
			//     |            |                                   | <- -> |         | <-  -> |
			//  d.left        d.right                                 d.left            d.right
			// 注意：如果采用9宫格切分，则切分之后左右（或上下）必须至少剩下1象素，中间至少剩下1象素
			//      也就是说，如果用Draw9Center，则务必保证原图至少有3象素，否则无法满足上面的条件
			UNIVERSAL_LIB_API void DrawImage9Grid(HDC hdc, const RECT& rcDest, HDC hdcSrc, const RECT& rcSrc,
				const Draw9 &d, DrawOption opt = DrawOption_Opaque);

			//////////////////////////////////////////////////////////////////////////

			UNIVERSAL_LIB_API void DrawLine(HDC hdc, const POINT& pt1, const POINT& pt2, COLORREF clr, int iWidth = 1,
											bool bLayered = false, bool bAntiAlias = true, bool bFillRect = false);
			UNIVERSAL_LIB_API void DrawFocusRect(HDC hdc, const RECT& rc, COLORREF clr, BYTE cAlpha = 0xff);
			UNIVERSAL_LIB_API void FillSolidRect(HDC hdc, CRect rc, COLORREF clr, BYTE cAlpha = 0xff);
			UNIVERSAL_LIB_API void FillGradientRect(HDC hdc, CRect rc, COLORREF clrBegin, BYTE alphaBegin, COLORREF clrEnd, BYTE alphaEnd, bool bHorz);
			UNIVERSAL_LIB_API void RoundRect(HDC hdc, const RECT& rc, int x, int y, BYTE cAlpha = 0xff);

			UNIVERSAL_LIB_API CRect MeasureText(HDC dc, LPCWSTR lpszText, int iTextLen, HFONT hFont,
				bool bMultiline = false, int iMaxWidth = -1, int iLineDistance = 0);
			UNIVERSAL_LIB_API int DrawText(HDC hdc, LPCWSTR lpszText, int cchText, const RECT& rc, UINT format, BYTE cAlpha = 0xff);
			UNIVERSAL_LIB_API int DrawTextEx(HDC hdc, LPCWSTR lpszText, int cchText, const RECT& rc, DrawFormat format = DrawFormat_Default, BYTE bApha = 255);
			UNIVERSAL_LIB_API RECT DrawTextFocusRect(HDC hdc, const RECT& rc, LPCWSTR lpszText, int iTextLen, DWORD dwFormat);
			UNIVERSAL_LIB_API int DrawGlowText(CDCHandle hdc, LPCWSTR lpText, int cchText,
				const RECT &rc, UINT format, bool bGlow, HTHEME hTheme = NULL);
		}
	}
}

namespace Universal
{
	namespace UI
	{
		namespace Paint	// 各种重载
		{

			//////////////////////////////////////////////////////////////////////////
			// TuotuoXP: 我总结一下下面这些乱七八糟的 api 的规律
			// 1. source 可以以两种形式给：bitmap和dc
			// 2. 9宫格绘制的偏移参数有三种：Draw9/Draw9H/Draw9V，不给的话表示不用9宫格方式绘制
			// 3. rcSrc 和 Draw9Frame 二者给一个，都是用来给出 source 的区域，不给的话表示使用完整的 source 图
			//////////////////////////////////////////////////////////////////////////

			inline void DrawImage9Grid(HDC hdc, const RECT& rcDest, HBITMAP hbmpSrc, const RECT& rcSrc, const Draw9 &d,
				DrawOption opt = DrawOption_Opaque)
			{
				DrawImage9Grid(hdc, rcDest, CBitmapDC(hbmpSrc), rcSrc, d, opt);
			}

			// 不用九宫格法绘制（直接贴图）
			inline void DrawImage(HDC hdc, const RECT& rcDest, HBITMAP hbmpSrc, const RECT& rcSrc, DrawOption opt = DrawOption_Opaque)
			{
				DrawImage9Grid(hdc, rcDest, CBitmapDC(hbmpSrc), rcSrc, Draw9(), opt);
			}

			// 不给rcSrc（用帧数代替）
			inline void DrawImage9Grid(HDC hdc, const RECT& rcDest, HBITMAP hbmpSrc, const Draw9Frame &f, const Draw9 &d,
				DrawOption opt = DrawOption_Opaque)
			{
				DrawImage9Grid(hdc, rcDest, CBitmapDC(hbmpSrc), f.ToRect(hbmpSrc), d, opt);
			}
			
			// 不给rcSrc（用帧数代替），不用九宫格法绘制（直接贴图）
			inline void DrawImage(HDC hdc, const RECT& rcDest, HBITMAP hbmpSrc, const Draw9Frame &f, DrawOption opt = DrawOption_Opaque)
			{
				DrawImage9Grid(hdc, rcDest, CBitmapDC(hbmpSrc), f.ToRect(hbmpSrc), Draw9(), opt);
			}

			// 直接使用整个source图
			inline void DrawImage9Grid(HDC hdc, const RECT& rcDest, HBITMAP hbmpSrc, const Draw9 &d, DrawOption opt = DrawOption_Opaque)
			{
				DrawImage9Grid(hdc, rcDest, CBitmapDC(hbmpSrc), Draw9Frame().ToRect(hbmpSrc), d, opt);
			}

			// 直接使用整个source图，不用九宫格法绘制（直接贴图）
			inline void DrawImage(HDC hdc, const RECT& rcDest, HBITMAP hbmpSrc, DrawOption opt = DrawOption_Opaque)
			{
				DrawImage9Grid(hdc, rcDest, CBitmapDC(hbmpSrc), Draw9Frame().ToRect(hbmpSrc), Draw9(), opt);
			}

			//////////////////////////////////////////////////////////////////////////

			inline int DrawText(HDC hdc, LPCWSTR lpszText, int cchText, const RECT* prc, UINT format, BYTE cAlpha = 0xff)
			{
				return DrawText(hdc, lpszText, cchText, *prc, format, cAlpha);
			}

			inline int DrawTextEx(HDC hdc, LPCWSTR lpszText, int cchText, const RECT* rc, DrawFormat format = DrawFormat_Default, BYTE bAlpha = 255)
			{
				return DrawTextEx(hdc, lpszText, cchText, *rc, format, bAlpha);
			}
			
			inline void DrawTextFast(HDC hdc, LPCWSTR lpszText, const RECT& rc, bool bVCenter = false)
			{
				DrawTextEx(hdc, lpszText, -1, rc, (bVCenter ? DrawFormat_VCenter : DrawFormat_Default));
			}

			inline void DrawTextFast(HDC hdc, LPCWSTR lpszText, const RECT* rc, bool bVCenter = false)
			{
				DrawTextEx(hdc, lpszText, -1, *rc, (bVCenter ? DrawFormat_VCenter : DrawFormat_Default));
			}
		}
	}
}
