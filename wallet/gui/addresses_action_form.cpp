#include "addresses_action_form.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/SkinDef.h"
#include "common_ui/CommonDef.h"
#include "base/utf_string_conversions.h"
#include "UILite/Controls/UILabel.h"
#include "CommonLib/UI//Icon.h"
#include "core/core_util.h"
#include "show_panel.h"

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

class CActionItem : public CUIControl
{
public:
    CActionItem(const std::wstring& address, CAddressActionForm* form);

protected:
    BEGIN_UI_EVENT_MAP(CActionItem)
        UI_EVENT_CREATE(OnUICreate)
        UI_EVENT_SIZE(OnUISize)
        UI_COMMAND_CODE(UINM_MOUSEENTER, OnMouseEnter)
        UI_COMMAND_CODE(UINM_MOUSELEAVE, OnMouseLeave)
        UI_COMMAND_CODE(UIBN_CLICK, OnUIBNClick)
    END_UI_EVENT_MAP()

    LRESULT OnMouseEnter(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
    LRESULT OnMouseLeave(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

    void OnUICreate();
    void OnUISize();
    LRESULT OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

private:
    CAddressActionForm* form_ = nullptr;
    std::unique_ptr<CUIEdit> address_;
    std::unique_ptr<CUIImage> eye_;
    std::unique_ptr<CUIButton> action_btn_;
};

CActionItem::CActionItem(const std::wstring& address, CAddressActionForm* form) : form_(form)
{
    eye_.reset(new CUIImage);
    eye_->SetBitmap(form_->eye_normal_bmp_);
    address_.reset(CommonUI::CreateStaticEdit(0, address));
    action_btn_.reset(CommonUI::CreateNormalButton(form_->action_btn_name_.c_str(), 0));
}

void CActionItem::OnUICreate()
{
    address_->UICreate(this);
    eye_->UICreate(this);
    action_btn_->UICreate(this);
}

void CActionItem::OnUISize()
{
    auto rc = UIGetClientRect();
    int left = CommonUI::kDefaultMargin;
    address_->UISetPos(left, (rc.Height() - 30) / 2, 340, 30);
    left += CommonUI::kDefaultMargin + 340;

    auto bmp_size = eye_->GetSize();
    eye_->UISetPos(left, (rc.Height() - bmp_size.cy) / 2, bmp_size.cx, bmp_size.cy);

    auto btn_size = action_btn_->UIMeasure();
    auto btn_width = btn_size.cx + 20;
    action_btn_->UISetPos(rc.Width() - CommonUI::kDefaultMargin - btn_width, (rc.Height() - 28) / 2, btn_width, 28);
}

LRESULT CActionItem::OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    if (form_)
        form_->OnActionBtnClick(address_->UIGetText(), dwControlID);
    return S_OK;
}

LRESULT CActionItem::OnMouseEnter(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    eye_->SetBitmap(form_->eye_select_bmp_);
    UIRedraw();
    return S_OK;
}

LRESULT CActionItem::OnMouseLeave(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    eye_->SetBitmap(form_->eye_normal_bmp_);
    UIRedraw();
    return S_OK;
}

class CActionItemListCreator : public CUIListViewSubItemCreator
{
public:
    CActionItemListCreator(CAddressActionForm* form) : form_(form)
    {
    }

    CUIObject* OnCreateObject(CUIListView* pView, HLISTVIEWITEM hItem, const int iCol) override
    {
        CUIObject* item = nullptr;
        int row = pView->GetItemIndex(hItem);
        auto data = (std::string*)pView->GetItemData(row);
        if (data)
        {
            item = new CActionItem(base::UTF8ToWide(*data), form_);
            item->UISetPainter(new CListItemBKPainter(row));
            delete data;
        }
        return item;
    }

private:
    CAddressActionForm* form_ = nullptr;
};


CAddressActionForm::CAddressActionForm(const std::wstring& title, const std::wstring& action_btn_name)
    : CBaseForm(title), action_btn_name_(action_btn_name)
{
    eye_normal_bmp_ = Skin::GetSkinBmp(SkinDef::EyeNormal);
    eye_select_bmp_ = Skin::GetSkinBmp(SkinDef::EyeSelect);

    action_list_.reset(new CUIListView);
    action_list_->SetItemBKPainter(new CNullPainter);
    action_list_->SetSubItemCreator(new CActionItemListCreator(this));
    action_list_->SetScrollBar(CommonUI::CreateVScrollBar());
    action_list_->UIModifyStyle(UILVStyle_FullRowSelect | UILVStyle_SimpleList
        | UILVStyle_TrackingMouse | UILVStyle_SingleSelection, 0);
}

void CAddressActionForm::Update(const std::set<std::string>& addresses)
{
    action_list_->Clear();
    int i = 0;
    for (auto& a : addresses)
    {
        action_list_->InsertItem(i, false, NULL, (DWORD_PTR)new std::string(a));
        action_list_->SetItemHeight(i, 40);
        i++;
    }
    Layout();
}

void CAddressActionForm::OnUICreate()
{
    UISetSize(600, 400);
    CBaseForm::OnUICreate();
    action_list_->UICreate(this);
}

void CAddressActionForm::Layout()
{
    CBaseForm::Layout();
    auto rc = UIGetClientRect();
    int top = GetTitleHeight();
    action_list_->UISetPos(0, top, rc.Width(), 360);
}

void CSendActionForm::OnUICreate()
{
    CAddressActionForm::OnUICreate();
    send_form_.UIShow(false);
    send_form_.UICreate(UIGetParent());
}

static std::string ParseAddress(const std::wstring& address)
{
    auto add = base::WideToUTF8(address);
    bool valid = false;
    std::string tron_add;
    std::tie(valid, tron_add) = AddressDecodeBase58Check(add);
    if (valid)
        tron_add;
    else
        tron_add.clear();
    return tron_add;
}

void CSendActionForm::OnActionBtnClick(const std::wstring& address, DWORD id)
{
    auto add = ParseAddress(address);
    if (!add.empty())
    {
        send_form_.UpdteAddress(add, [this](bool success) {
            auto panel = (CShowPanel*)UIGetParent();
            panel->SetContent(&send_form_);
        });
    }
}

void CVoteActionForm::OnUICreate()
{
    CAddressActionForm::OnUICreate();
    vote_form_.UIShow(false);
    vote_form_.UICreate(UIGetParent());
}

void CVoteActionForm::OnActionBtnClick(const std::wstring& address, DWORD id)
{
    auto add = ParseAddress(address);
    if (!add.empty())
    {
        vote_form_.SetAddress(add);
        vote_form_.GetWitnessList([this](bool success) {
            auto panel = (CShowPanel*)UIGetParent();
            panel->SetContent(&vote_form_);
        });
    }
}

void CFreezeActionForm::OnUICreate()
{
    CAddressActionForm::OnUICreate();
    freeze_form_.UIShow(false);
    freeze_form_.UICreate(UIGetParent());
}

void CFreezeActionForm::OnActionBtnClick(const std::wstring& address, DWORD id)
{
    auto add = ParseAddress(address);
    if (!add.empty())
    {
        freeze_form_.SetAddress(add);
        auto panel = (CShowPanel*)UIGetParent();
        panel->SetContent(&freeze_form_);
    }
}

void CBalanceActionForm::OnUICreate()
{
    CAddressActionForm::OnUICreate();
    balance_form_.UIShow(false);
    balance_form_.UICreate(UIGetParent());
}

void CBalanceActionForm::OnActionBtnClick(const std::wstring& address, DWORD id)
{
    auto add = ParseAddress(address);
    if (!add.empty())
    {
        balance_form_.SetAddress(add);
    }
}

