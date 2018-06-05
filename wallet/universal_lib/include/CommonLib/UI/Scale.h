// @Author: yangqi
#pragma once
#include "../../Universal.h"

namespace Universal
{
	namespace UI
	{
		class CScale
		{
		public:
			enum ScaleType
			{
				System_Scale,
				Ui_Scale,
				Webview_Scale,
			};

		public:
			UNIVERSAL_LIB_API explicit CScale(ScaleType eType);
			UNIVERSAL_LIB_API ~CScale();
			UNIVERSAL_LIB_API int	Scale(int nValue, bool bScale) const;
			UNIVERSAL_LIB_API WORD	Scale(WORD nValue, bool bScale) const;
			UNIVERSAL_LIB_API RECT	Scale(const RECT& rc, bool bScale) const;
			UNIVERSAL_LIB_API SIZE	Scale(const SIZE& sz, bool bScale) const;
			UNIVERSAL_LIB_API POINT	Scale(const POINT& pt, bool bScale) const;

		private:
			const ScaleType m_eType;
		};
	}
}
