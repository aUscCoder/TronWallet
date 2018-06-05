// Author: yangqi
/**
 * \file GifImage.h
 * \brief 
 * \author yangqi
 * \date October 2017
 *
 */
#pragma once
class CGifImage : public Gdiplus::Image
{
public:
    CGifImage(LPCWSTR lpszFileName);
    CGifImage(LPBYTE pBuf, size_t nSize);

    ~CGifImage();

    void Draw(HDC hDC, int x, int y, UINT cx, UINT cy);
    UINT NextFrame();

private:
    bool TestForAnimatedGIF();
    bool LoadFromBuffer(BYTE* pBuff, int nSize);

private:
    UINT m_nFrameCount = 0;
    Gdiplus::PropertyItem* m_pPropertyItem = nullptr;
    UINT m_nFramePosition = 0;
};

