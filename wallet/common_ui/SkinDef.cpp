#include "SkinDef.h"
#include "common_ui/SkinResource.h"
#include "CommonLib/UI/Bitmap.h"

SkinInfo g_skinMap[] = 
{
    { IDB_PNG_BTN_NORMAL, 2 },
    { IDB_PNG_BTN_CANCEL, 2 },
    { IDB_PNG_BTN_CLOSE, 3 },
    { IDB_PNG_BTN_MAX, 3 },
    { IDB_PNG_BTN_MIN, 3 },
    { IDB_PNG_BTN_RESTORE, 3 },
    { IDB_PNG_SHADOW, 1 },
    { IDB_PNG_DROPDOWN_TRIANGLE, 2 },
    { IDB_PNG_RADIO, 2 },
    { IDB_PNG_VSCROLLBAR, 3 },
    { IDB_PNG_EDIT_BG, 2 },
    { IDB_PNG_COMBOBOX_BG, 5 },
    { IDB_PNG_EYE_NORMAL, 1 },
    { IDB_PNG_EYE_SELECT, 1 },
    { IDB_PNG_LOGO, 1 },
    { IDB_PNG_SEPERATOR, 1},
    { IDB_PNG_INDEX, 1},
};

static_assert(_countof(g_skinMap) == (int)SkinDef::Count, "skin def macro count error");
SkinInfo* Skin::GetSkinInfo()
{
    return g_skinMap;
}

HBITMAP Skin::GetSkinBmp(SkinDef eSkin)
{
    if (g_skinMap[(int)eSkin].hBmp.IsNull())
    {
        UINT uID = g_skinMap[(int)eSkin].uID;
        g_skinMap[(int)eSkin].hBmp = Universal::UI::Bitmap::LoadFromRes(NULL, uID, L"PNG");
    }
    return g_skinMap[(int)eSkin].hBmp;
}
