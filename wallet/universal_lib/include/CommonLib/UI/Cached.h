// Author: ChenJianjun
#pragma once
#include "../../Universal.h"


namespace Universal
{
	namespace UI
	{
		namespace Cache
		{
			UNIVERSAL_LIB_API HDC GetDC();
			UNIVERSAL_LIB_API void ReleaseDC(HDC hdc);
			UNIVERSAL_LIB_API HDC GetDC32();
		}
	}
}
