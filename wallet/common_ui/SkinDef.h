#pragma once
#include "common_ui/CommonInclude.h"

struct SkinInfo 
{
    UINT uID;
    UINT uFrameCount;
    CBitmap hBmp;
};

enum class SkinDef
{
    NormalBtn = 0,
    CancelBtn,
    Close,
    Max,
    Min,
    Restore,
    Shadow,
    DropdownTriangle,
    Radio,
    VScrollBar,
    EditBG,
    ComboBoxBG,
    EyeNormal,
    EyeSelect,
    Logo,
    Seperator,
    Index,
    Count,
};

namespace Skin
{
    SkinInfo* GetSkinInfo();
    HBITMAP GetSkinBmp(SkinDef eSkin);
    const int kDefaultItemHeight = 30;
    const int kDefaultMargin = 10;
}
