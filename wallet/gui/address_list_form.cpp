#include "address_list_form.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "show_panel.h"
#include "core/account_manager.h"

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

class CAddressItemListCreator : public CUIListViewSubItemCreator
{
public:
    CAddressItemListCreator(CAddressListForm* form) : form_(form) {}
    CUIObject* OnCreateObject(CUIListView* pView, HLISTVIEWITEM hItem, const int iCol) override
    {
        auto CreateListItem = [this](const std::wstring& address, bool head)
        {
            auto item = new CCommonListViewItem();
            if (head)
            {
                std::shared_ptr<CUIStatic> add(new CUIStatic(address));
                add->UISetSize(320, 28);
                item->Push(add);
            }
            else
            {
                std::shared_ptr<CUIEdit> add(CommonUI::CreateStaticEdit(0, address));
                add->UISetReadOnly();
                add->UISetSize(320, 28);
                item->Push(add);
            }

            CUIObject* btn = CommonUI::CreateNormalButton(head ? L"Add Address" : L"show private key", 0);
            btn->UISetSize(160, 28);
            item->SetXMagin(360);
            
            item->Push(std::shared_ptr<CUIObject>(btn));

            std::shared_ptr<CUIEdit> key(CommonUI::CreateStaticEdit(0, L""));
            key->UISetSize(480, 28);
            key->UIShow(false);
            item->Push(key);

            item->SetClickCallback([item, key, btn, address, head, this](DWORD dwControldID) {
                if (head)
                {
                    form_->OnAddAddress();
                }
                else
                {
                    auto uf8_address = base::WideToUTF8(address);
                    auto privete_key = AccountManager::_().LookupPrivate(uf8_address);
                    auto bytes = privete_key.asBytesSec();
                    if (!bytes.empty())
                    {
                        auto hex = dev::toHex(bytes.ref());
                        btn->UIShow(false);
                        key->UISetText(base::UTF8ToWide(hex));
                        key->UIShow(true);
                        item->SetXMagin(60);
                        item->Layout();
                    }
                }
            });
            return item;
        };

        CUIObject* item = nullptr;
        int row = pView->GetItemIndex(hItem);
        if (row == 0)
        {
            item = CreateListItem(L"Address", true);
        }
        else
        {
            auto data = (std::string*)pView->GetItemData(row);
            if (data)
            {
                item = CreateListItem(base::UTF8ToWide(*data), false);
                delete data;
            }
        }

        item->UISetPainter(new CommonUI::CListItemBKPainter(row));
        return item;
    }

private:
    CAddressListForm* form_ = nullptr;
};


CAddressListForm::CAddressListForm() : CBaseForm(L"")
{
    addresses_.reset(new CUIListView);
    addresses_->SetItemBKPainter(new CNullPainter);
    addresses_->SetSubItemCreator(new CAddressItemListCreator(this));
    addresses_->SetScrollBar(CommonUI::CreateVScrollBar());
    addresses_->UIModifyStyle(UILVStyle_FullRowSelect | UILVStyle_SimpleList
        | UILVStyle_TrackingMouse | UILVStyle_SingleSelection, 0);
}

void CAddressListForm::Update(const std::vector<std::string>& addresses)
{
    addresses_->Clear();
    addresses_->InsertItem(0);
    addresses_->SetItemHeight(0, 40);
    for (int i = 0; i < addresses.size(); i++)
    {
        addresses_->InsertItem(i + 1, false, nullptr, (DWORD_PTR)new std::string(addresses[i]));
        addresses_->SetItemHeight(i + 1, 40);
    }
    SetTitle(base::UTF8ToWide(AccountManager::_().GetAccountName()));
    Layout();
}

void CAddressListForm::OnUICreate()
{
    UISetSize(880, 400);
    CBaseForm::OnUICreate();
    addresses_->UICreate(this);
}

void CAddressListForm::Layout()
{
    CBaseForm::Layout();
    auto rc = UIGetClientRect();
    int top = GetTitleHeight();
    top += CommonUI::kDefaultMargin;
    addresses_->UISetPos(0, top, rc.Width(), 320);
}

void CAddressListForm::OnAddAddress()
{
    if (address_text_form_)
        address_text_form_->UIDestroy();

    address_text_form_.reset(new CAddressTextForm(CAddressTextForm::Type::Generate, [](const std::string& address) {
        // TODO(jiangru)

    }));
    auto panel = (CShowPanel*)UIGetParent();
    address_text_form_->UICreate(panel);
    panel->SetContent(address_text_form_.get());
}
