#pragma once
#include "CommonInclude.h"
#include "UILite/Controls/UIEdit.h"

class CPlaceHolderEdit : public CUIEdit
{
public:
    CPlaceHolderEdit(DWORD dwID, const std::wstring& text, DWORD dwEditStyle = 0);
    void SetPlaceholder(const std::wstring& placeholder);
    std::wstring Placeholder();
private:
    std::wstring placeholder_;
};
