#include "CommonStdafx.h"
#include "IQRCode.h"
#include "CommonLib/UI/Bitmap.h"
#include "CommonLib/UI/Paint.h"


static void DrawCell(int scale, int x, int y, RGBQUAD* pBits, COLORREF clr, int width)
{
  x++; y++;
  for (int i = x * scale; i < (x + 1) * scale; i++)
  {
    for (int j = y * scale; j < (y + 1) * scale; j++)
    {
      auto pDest = pBits + i * width + j;
      pDest->rgbReserved = 255;
      pDest->rgbRed = GetRValue(clr);
      pDest->rgbGreen = GetGValue(clr);
      pDest->rgbBlue = GetBValue(clr);
    }
  }
}


// 使用QRcode_encodeString编码的时候，会对字符串优化，同样长度的字符，QRCode大小不一样。改为QRcode_encodeString8bit
BYTE* QRCode::GenerateQRCode(int image_size, const std::string& text, DWORD* size)
{
  QRcode* pQR = QRcode_encodeString8bit(text.c_str(), 0, QR_ECLEVEL_M);
  if (!pQR)
    return NULL;

  int scale = image_size / pQR->width;
  int iWidth = scale * pQR->width + 2 * scale;

  COLORREF bkColor = RGB(0xff, 0xff, 0xff);
  COLORREF frontColor = RGB(0, 0, 0);

  CDC hMemDC = CreateCompatibleDC(NULL);
  RGBQUAD* pBits = NULL;
  CBitmap bmp = Universal::UI::Bitmap::CreateDIBSection32(iWidth, -iWidth, (void**)&pBits);
  HBITMAP hOldBmp = hMemDC.SelectBitmap(bmp);
  Universal::UI::Paint::FillSolidRect(hMemDC, CRect(0, 0, iWidth, iWidth), bkColor);
  hMemDC.SelectBitmap(hOldBmp);

  auto qrcode_width = pQR->width;
  for (int i = 0; i < qrcode_width; i++)
  {
    for (int j = 0; j < qrcode_width; j++)
    {
      if (pQR->data[i * qrcode_width + j] & 0x01)
      {
        DrawCell(scale, i, j, pBits, frontColor, iWidth);
      }
    }
  }
  QRcode_free(pQR);
  return Universal::UI::Bitmap::SaveToBuffer(bmp, size, SaveType_PNG, 100);
}
