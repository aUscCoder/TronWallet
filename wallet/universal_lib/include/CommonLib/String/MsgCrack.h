///
/// @author chenjianjun
///
#pragma once
#include "../../Universal.h"


#define Msg2Str(msg) (ConvertMsg2Str(msg).c_str())
UNIVERSAL_LIB_API std::wstring ConvertMsg2Str(UINT uMsg);
