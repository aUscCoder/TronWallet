#include "SkinImage.h"
#include "CommonLib/UI/Bitmap.h"
#include "CommonLib/UI/Paint.h"

CSkinImage::CSkinImage(SkinDef eType) : m_eDef(eType)
{
    m_hBmp = Skin::GetSkinBmp(eType);
    UINT uID = Skin::GetSkinInfo()[(int)eType].uID;
    CSize sz;
    m_hBmp.GetSize(sz);
    m_iFrameWidth = sz.cx / Skin::GetSkinInfo()[(int)eType].uFrameCount;
}

CSkinImage::~CSkinImage()
{

}

int CSkinImage::GetFrameCount()
{
    return Skin::GetSkinInfo()[(int)m_eDef].uFrameCount;
}

int CSkinImage::GetFrameWidth()
{
    return m_iFrameWidth;
}

void CSkinImage::Draw(CDCHandle dc, CRect rc, int iFrameIndex)
{
    Draw(dc, rc, iFrameIndex, DRAWOPT_NO_CHECK());
}

void CSkinImage::Draw(CDCHandle dc, CRect rc, int iFrameIndex, DrawOption opt)
{
    if (iFrameIndex < (int)Skin::GetSkinInfo()[(int)m_eDef].uFrameCount)
    {
        Universal::UI::Paint::DrawImage(dc, rc, m_hBmp,
            Draw9Frame(iFrameIndex, Skin::GetSkinInfo()[(int)m_eDef].uFrameCount), opt);
    }
}

void CSkinImage::DrawGrid(CDCHandle dc, CRect rc, int iFrameIndex)
{
    DrawGrid(dc, rc, iFrameIndex, Draw9Center(), DRAWOPT_NO_CHECK());
}

void CSkinImage::DrawGrid(CDCHandle dc, CRect rc, int iFrameIndex, const Draw9 &d, DrawOption opt)
{
    if (iFrameIndex < Skin::GetSkinInfo()[(int)m_eDef].uFrameCount)
    {
        Universal::UI::Paint::DrawImage9Grid(dc, rc, m_hBmp,
            Draw9Frame(iFrameIndex, Skin::GetSkinInfo()[(int)m_eDef].uFrameCount), d, opt);
    }
}
