#pragma once
#include "SkinDef.h"
#include "CommonLib/UI/Paint.h"

class CSkinImage
{
public:

    CSkinImage(SkinDef eType);
    ~CSkinImage();

    int GetFrameCount();
    int GetFrameWidth();

    void Draw(CDCHandle dc, CRect rc, int iFrameIndex = 0);
    void Draw(CDCHandle dc, CRect rc, int iFrameIndex, DrawOption opt);
    void DrawGrid(CDCHandle dc, CRect rc, int iFrameIndex = 0);
    void DrawGrid(CDCHandle dc, CRect rc, int iFrameIndex, const Draw9 &d, DrawOption opt);

private:

    int        m_iFrameWidth = 0;
    CBitmapHandle m_hBmp = nullptr;
    SkinDef m_eDef;
};
