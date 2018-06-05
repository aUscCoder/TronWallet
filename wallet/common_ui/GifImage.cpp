#include "CommonInclude.h"
#include "GifImage.h"
#pragma comment(lib,"gdiplus.lib")
#include <gdiplus.h>
using namespace Gdiplus;


CGifImage::CGifImage(LPCWSTR lpszFileName) : Gdiplus::Image(lpszFileName)
{
    TestForAnimatedGIF();
}

CGifImage::CGifImage(LPBYTE pBuf, size_t nSize)
{
    LoadFromBuffer(pBuf, nSize);

    TestForAnimatedGIF();
}

CGifImage::~CGifImage()
{
}

bool CGifImage::LoadFromBuffer(BYTE* pBuff, int nSize)
{
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
    if (hGlobal)
    {
        void* pData = GlobalLock(hGlobal);
        if (pData)
            memcpy(pData, pBuff, nSize);

        GlobalUnlock(hGlobal);

        IStream* pStream;
        CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);

        nativeImage = NULL;
        DllExports::GdipLoadImageFromStreamICM(pStream, &nativeImage);
    }

    return true;
}

bool CGifImage::TestForAnimatedGIF()
{
    UINT count = 0;
    count = GetFrameDimensionsCount();
    ATLASSERT(count);
    GUID* pDimensionIDs = new GUID[count];

    // Get the list of frame dimensions from the Image object.
    GetFrameDimensionsList(pDimensionIDs, count);

    // Get the number of frames in the first dimension.
    m_nFrameCount = GetFrameCount(&pDimensionIDs[0]);

    // Assume that the image has a property item of type PropertyItemEquipMake.
    // Get the size of that property item.
    int nSize = GetPropertyItemSize(PropertyTagFrameDelay);
    if (nSize > 0)
    {
        // Allocate a buffer to receive the property item.
        m_pPropertyItem = (PropertyItem*)malloc(nSize);
        GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
    }

    delete  pDimensionIDs;

    return m_nFrameCount > 1;
}

void CGifImage::Draw(HDC hDC, int x, int y, UINT cx, UINT cy)
{
    Graphics graphics(hDC);
    graphics.DrawImage(this, x, y, cx, cy);
}

UINT CGifImage::NextFrame()
{
    if (++m_nFramePosition >= m_nFrameCount)
        m_nFramePosition = 0;

    static const GUID   pageGuid = FrameDimensionTime;
    SelectActiveFrame(&pageGuid, m_nFramePosition);

    UINT nPause = 100;
    if (m_pPropertyItem)
    {
        // NOTE HERE: frame-delay values should be multiply by 10
        nPause = ((UINT*)m_pPropertyItem->value)[m_nFramePosition] * 10;
        if (nPause == 0)
        {
            nPause = 100;
        }
    }
    return nPause;
}
