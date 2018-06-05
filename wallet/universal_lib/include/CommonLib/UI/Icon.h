// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "UI.h"


namespace Universal
{
	namespace UI
	{
		namespace Icon
		{
			UNIVERSAL_LIB_API HICON CreateFromMem(const void *pBits, unsigned int iLen, int iIconSize = UI_SCALE(16));
			UNIVERSAL_LIB_API HICON Resize(HICON hIcon, int w, int h = -1); // h 为 -1 表示与 w 相等
			UNIVERSAL_LIB_API HICON FromHBITMAP(HBITMAP hBitmap);
			UNIVERSAL_LIB_API BYTE* SaveToMem(HICON hIcon, DWORD &dwIcoSize);
			UNIVERSAL_LIB_API SIZE  GetSize(HICON hIcon);
		};
	};
};
