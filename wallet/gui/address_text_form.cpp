#include "address_text_form.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "UILite/Controls/UIButton.h"
#include "UILite/Controls/UIEdit.h"
#include "core/account_manager.h"

using namespace wallet_core;
using namespace wallet_gui;

enum class ControlID
{
    EDIT_ADDRESS_LIST = 1200,
    BTN_OK,
};

CAddressTextForm::CAddressTextForm(Type t, Receive callback)
    : CBaseForm(t == CAddressTextForm::Import ? L"Import Address" : L"Generate New Address")
    , type_(t), callback_(callback)
{
    detail_.reset(new CUIStatic(t == CAddressTextForm::Import ? L"Please enter your watching-only addresses:" : L"New address are listed below:"));
    textbox_.reset(CommonUI::CreateEdit((DWORD)ControlID::EDIT_ADDRESS_LIST));
    btn_.reset(CommonUI::CreateNormalButton(t == CAddressTextForm::Import ? L"Submit" : L"Ok", (DWORD)ControlID::BTN_OK));
}

void CAddressTextForm::OnUICreate()
{
    if (type_ == CAddressTextForm::Import)
        UISetSize(400, 300);
    else if (type_ == CAddressTextForm::Generate)
        UISetSize(400, 200);
      
    CBaseForm::OnUICreate();
    detail_->UICreate(this);
    if (type_ == CAddressTextForm::Import)
        textbox_->UIModifyStyle(UIEditStyle_MultiLine | UIEditStyle_AutoHScroll, UIEditStyle_VCenter | UIEditStyle_AutoHScroll);
    else if (type_ == CAddressTextForm::Generate)
        textbox_->UISetReadOnly();
    textbox_->UICreate(this);
    btn_->UICreate(this);
    if (type_ == CAddressTextForm::Generate)
        GenerateAddresses();
    Layout();
}

void CAddressTextForm::Layout()
{
    CBaseForm::Layout();
    auto rc = UIGetClientRect();
    int top = GetTitleHeight();
    top += CommonUI::kDefaultMargin;
    detail_->UISetPos(CommonUI::kDefaultMargin, top, rc.Width() - 2 * CommonUI::kDefaultMargin, 20);
    top += 20 + CommonUI::kDefaultMargin;

    int textbox_hegiht = 0;
    if (type_ == CAddressTextForm::Import)
        textbox_hegiht = 120;
    else if (type_ == CAddressTextForm::Generate)
        textbox_hegiht = 34;

    textbox_->UISetPos(CommonUI::kDefaultMargin, top, rc.Width() - 2 * CommonUI::kDefaultMargin, textbox_hegiht);
    top += textbox_hegiht + 2 * CommonUI::kDefaultMargin;
    auto left = (rc.Width() - 120) / 2;
    btn_->UISetPos(left, top, 120, 34);
}

void CAddressTextForm::GenerateAddresses()
{
    textbox_->UISetText(base::UTF8ToWide(AccountManager::_().GenerateAddress()));
}

void CAddressTextForm::OnButtonClick(DWORD dwControlID)
{
    if (dwControlID == DWORD(ControlID::BTN_OK))
    {
        auto text = base::WideToUTF8(textbox_->UIGetText());
        if (callback_)
            callback_(text);
        UIShow(false);
    }
}
