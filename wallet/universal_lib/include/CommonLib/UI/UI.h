/// @Author: chenjianjun
#pragma once
#include "../../Universal.h"
#include "Scale.h"

// 是否支持高DPI
#define ENABLE_HI_DPI_SUPPORT

#define HALIGN_LEFT		0
#define HALIGN_CENTER	1
#define HALIGN_RIGHT	2
#define VALIGN_TOP		0
#define VALIGN_CENTER	1
#define VALIGN_BOTTOM	2
#define ALIGN_NOT_CHANGE -1

struct AlignMode
{
	short iHAlign;	// 0 - left   1 - middle    2 - right    < 0 不变
	short iVAlign;	// 0 - top	  1 - middle    2 - bottom   < 0 不变
	AlignMode(short ih, short iv) : iHAlign(ih), iVAlign(iv) {}
};
struct AlignTopLeft : AlignMode { AlignTopLeft() : AlignMode(HALIGN_LEFT, VALIGN_TOP) {} };
struct AlignCenterLeft : AlignMode { AlignCenterLeft() : AlignMode(HALIGN_LEFT, VALIGN_CENTER) {} };
struct AlignBottomLeft : AlignMode { AlignBottomLeft() : AlignMode(HALIGN_LEFT, VALIGN_BOTTOM) {} };
struct AlignCenter : AlignMode { AlignCenter() : AlignMode(HALIGN_CENTER, VALIGN_CENTER) {} };
struct AlignCenterTop : AlignMode { AlignCenterTop() : AlignMode(HALIGN_CENTER, VALIGN_TOP) {} };
struct AlignTopRight : AlignMode { AlignTopRight() : AlignMode(HALIGN_RIGHT, VALIGN_TOP) {} };
struct AlignCenterRight : AlignMode { AlignCenterRight() : AlignMode(HALIGN_RIGHT, VALIGN_CENTER) {} };
struct AlignBottomRight : AlignMode { AlignBottomRight() : AlignMode(HALIGN_RIGHT, VALIGN_BOTTOM) {} };
struct AlignBottomCenter : AlignMode { AlignBottomCenter() : AlignMode(HALIGN_CENTER, VALIGN_BOTTOM) {} };
struct AlignVCenter : AlignMode { AlignVCenter() : AlignMode(ALIGN_NOT_CHANGE, VALIGN_CENTER) {} };
struct AlignHCenter : AlignMode { AlignHCenter() : AlignMode(HALIGN_CENTER, ALIGN_NOT_CHANGE) {} };

namespace Universal
{
	namespace UI
	{
		UNIVERSAL_LIB_API void			InitGDIPlusOnce();
		UNIVERSAL_LIB_API int			GetCurentSystemDPI();
		UNIVERSAL_LIB_API std::wstring	GetWindowClassName(HWND hWnd);
		UNIVERSAL_LIB_API void			AlignRect(RECT& rc, const RECT& rcAganist, const AlignMode& mode);
		UNIVERSAL_LIB_API int			GetFontHeight(HFONT hFont);

		// HI DPI support
		// 当前 DPI 类型，为了换算时不用转换成整数，将其定义为具体的百分比数
		enum DPI_SCALE_TYPE
		{
			DPI_SCALE_UNKNOWN	= 0,
			DPI_SCALE_100		= 100,
			DPI_SCALE_125		= 125,
			DPI_SCALE_150		= 150,
			DPI_SCALE_200		= 200,
		};

		// 当前 UI 最多支持的 DPI 列表，返回值必须按照从小到大的顺序排列
		UNIVERSAL_LIB_API const std::vector<Universal::UI::DPI_SCALE_TYPE>& GetSupportedDPI();

		//
		// 获取当前系统的 DPI 信息，浏览器运行过程中不会改变
		// 主 UI 会根据此值和具体的皮肤包确定主 UI 的放大比例，当前不支持200
		//
		UNIVERSAL_LIB_API DPI_SCALE_TYPE GetSystemScale();

		//
		// 获取内核页面放大比例，浏览器运行过程中不会改变
		// 这里返回的其实是个整数, 而非枚举值!!!
		//
		UNIVERSAL_LIB_API DPI_SCALE_TYPE GetWebviewScale();

		UNIVERSAL_LIB_API DPI_SCALE_TYPE GetSupportedDPI(const DPI_SCALE_TYPE eRequest);

		//
		// 获取 UI 缩放信息， 会根据系统实际 DPI 和当前使用皮肤内的资源情况，返回具体的 DPI_SCALE_TYPE
		//
		UNIVERSAL_LIB_API DPI_SCALE_TYPE GetUiScale();
		UNIVERSAL_LIB_API void SetUiScale(DPI_SCALE_TYPE);
	}
}

//////////////////////////////////////////////////////////////////////////
//
// DPI 换算相关函数, 支持  int  RECT  POINT  SIZE
//
//////////////////////////////////////////////////////////////////////////
namespace Universal
{
	namespace UI
	{
		template <typename T>
		inline T Scale(CScale::ScaleType eType, const T& value, bool bScale)
		{
			CScale sc(eType);
			return sc.Scale(value, bScale);
		}

		//
		// 根据当前 DPI 进行换算，采用四舍五入的方式，对主 UI 相关的进行换算
		//
		// bScale 为 true 表示进行 DPI 缩放，否则为反向计算
		//
		// 比如当前系统 DPI 为 150%,
		//   UiScale(100, true)  return 150
		//   UiScale(100, false) return 67
		//
		template <typename T>
		T UiScale(const T& value, bool bScale)
		{
			return Scale(CScale::Ui_Scale, value, bScale);
		}

		//
		// 页面相关的缩放转换，不同于 UiScale()
		// 由于内核根据 GetSystemScale() 进行放大，因此也要根据 SystemScale 进行转换
		//
		template <typename T>
		T WebViewScale(const T& value)
		{
			return Scale(CScale::Webview_Scale, value, true);
		}

		//
		// 根据系统 DPI 进行换算
		// 根据 GetCurentSystemDPI() 进行，用系统 DPI 设置直接计算，不分阶梯
		//
		template <typename T>
		T SystemScale(const T& value)
		{
			return Scale(CScale::System_Scale, value, true);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 借用这个文件定义一些公共宏

// rect的宽度
inline int RCWidth(const RECT& rc) { return rc.right - rc.left; }

// rect的高度
inline int RCHeight(const RECT& rc) { return rc.bottom - rc.top; }

inline bool RCIsEmpty(const RECT& rc) { return rc.bottom <= rc.top || rc.right <= rc.left; }

// HI DPI support

// Windows正常DPI
#define NORMAL_DPI 96

// 主 UI 相关的 DPI 缩放
#ifndef UI_SCALE
# define UI_SCALE(x) Universal::UI::UiScale(x, true)
#endif

// 反向计算 DPI
#ifndef UI_REVERT
# define UI_REVERT(x) Universal::UI::UiScale(x, false)
#endif

// icon的大小
#ifndef ICON_SIZE
# define ICON_SIZE UI_SCALE(16)
#endif

// 主 UI 所支持的 DPI 放大上限（倍数）
#ifndef MAX_UI_SCALE
#  define MAX_UI_SCALE 2
#endif

// 主 UI 是否调整过 DPI
#ifndef IS_UI_SCALED
# define IS_UI_SCALED() (bool)(Universal::UI::GetUiScale() != Universal::UI::DPI_SCALE_100)
#endif

// 页面相关的 DPI 缩放
#ifndef WEBVIEW_SCALE
# define WEBVIEW_SCALE(x) Universal::UI::WebViewScale(x)
#endif

// 根据系统 DPI 进行缩放
#ifndef SYSTEM_SCALE
# define SYSTEM_SCALE(x) Universal::UI::SystemScale(x)
#endif

// 根据bWebScale来缩放一个值
#ifndef SCALE_VALUE
#define SCALE_VALUE(bWebScale, value) (bWebScale ? WEBVIEW_SCALE(value) : UI_SCALE(value))
#endif