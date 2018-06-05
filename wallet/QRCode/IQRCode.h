#pragma once
#include "qrencode.h"
#include <string>
#include <intsafe.h>

namespace QRCode
{
  extern "C" BYTE* GenerateQRCode(int image_size, const std::string& text, DWORD* size);
}
