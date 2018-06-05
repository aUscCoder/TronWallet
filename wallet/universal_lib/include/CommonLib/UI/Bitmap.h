// Author: yangqi
/**
 \file	UI\Bitmap.h
 \author yangqi
 \brief	Declares the bitmap class.
 */

#pragma once
#include "../../Universal.h"


enum BitmapSaveFileType
{
	SaveType_BMP,
	SaveType_JPG,
	SaveType_PNG,
	SaveType_GIF
};


namespace Universal
{
	namespace UI
	{
		namespace Bitmap
		{
			/**
			 \fn	UNIVERSAL_LIB_API HBITMAP CreateDIBSection(LONG nWidth, LONG nHeight, WORD wBitCount, HANDLE hSection, void **ppBits);
			
			 \brief	Creates bitmap section.
			
			 \author	yangqi
			 \date	2016-03-17
			
			 \param	nWidth			  	The width.
			 \param	nHeight			  	The height.
			 \param	wBitCount		  	Number of bits.
			 \param	hSection		  	Handle of the section.
			 \param [in,out]	ppBits	If non-null, the bits.
			
			 \return	The new bitmap section.
			 */
			UNIVERSAL_LIB_API HBITMAP CreateDIBSection(LONG nWidth, LONG nHeight, WORD wBitCount, HANDLE hSection, void **ppBits);
			UNIVERSAL_LIB_API HBITMAP CreateCompatibleBitmap(int w, int h);
			UNIVERSAL_LIB_API HBITMAP CreateFromIcon(HICON hIcon, int w, int h);
			UNIVERSAL_LIB_API HBITMAP LoadFromBuffer(const void *pData, DWORD dwLen, int iWidth = 0, int iHeight = 0);
			UNIVERSAL_LIB_API HBITMAP LoadFromRes(HINSTANCE hInstance, UINT nResourceID, LPCWSTR pResourceType);
			UNIVERSAL_LIB_API HBITMAP LoadFromFile(LPCWSTR lpszFileName);
			UNIVERSAL_LIB_API HBITMAP Copy(HBITMAP hBitmap);
			UNIVERSAL_LIB_API BYTE* SaveToBuffer(HBITMAP hBitmap, DWORD *pdwBufSize, BitmapSaveFileType eFileType,
				const LONG lQuality, bool bIgnoreAlpha = true);
			UNIVERSAL_LIB_API bool SaveToFile(HBITMAP hBitmap, LPCWSTR lpszFileName, BitmapSaveFileType eFileType,
				const LONG lQuality, bool bIgnoreAlpha = true);
			UNIVERSAL_LIB_API HBITMAP Resize(HBITMAP hBitmap, const CSize &sizeTo, bool bUseGDI = true, bool bHighQuality = false);
			UNIVERSAL_LIB_API Gdiplus::Bitmap* BitmapFromHBITMAP(HBITMAP hBitmap);

			/*!
				对一个HBITMAP进行模糊，该函数计算量较大，请将图片缩小后blur，慎重使用
				\param hBitmap[in]		输入图像
				\param iRadius[in]		卷积核半径
				\param iBlurTimes[in]	模糊迭代次数

				\return 返回一个32位的模糊过的图（非高斯模糊，使用的是均值，因为高斯核有浮点运算）
			*/
			UNIVERSAL_LIB_API HBITMAP Blur(HBITMAP hBitmap, const size_t uRadius, const size_t uBlurTimes);
			/**
			 \fn	inline HBITMAP CreateDIBSection32(int w, int h, void **ppBits)
			
			 \brief	Creates bitmap section 32.
			
			 \author	yangqi
			 \date	2016-03-17
			
			 \param	w				  	The width.
			 \param	h				  	The height.
			 \param [in,out]	ppBits	If non-null, the bits.
			
			 \return	The new bitmap section 32.
			 */
			inline HBITMAP CreateDIBSection32(int w, int h, void **ppBits)
			{
				return Universal::UI::Bitmap::CreateDIBSection(w, h, 32, nullptr, ppBits);
			}
		}
	}
}
