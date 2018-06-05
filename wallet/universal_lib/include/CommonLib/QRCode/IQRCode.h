// Author: liujing
#pragma once
#include "../../Universal.h"
#include "qrencode.h"


namespace QRCodeSpace
{
	UNIVERSAL_LIB_API QRcode *QREncodeString8bit(const char *string, int version, QRecLevel level);
	UNIVERSAL_LIB_API void QRFree(QRcode *qrcode);
}
