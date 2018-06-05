#include "transaction_item.h"
#include "common_ui/CommonDef.h"
#include "common_ui/UI.h"

#define ADDRESS_WIDTH 320
#define TRANSACTION_ITEM_GAP 10
#define TYPE_WIDTH 30
#define TIME_WIDTH 160
#define VALUE_WIDTH 100
#define ITEM_HEIGHT 28

namespace wallet_gui
{
    CTransactionItem::CTransactionItem(const std::wstring& from, const std::wstring& type, const std::wstring& to,
        const std::wstring& value, const std::wstring& time)
    {
        from_.reset(CommonUI::CreateStaticEdit(0, from));
        if (!type.empty())
            type_.reset(CommonUI::CreateIndexStatic(type));
        to_.reset(CommonUI::CreateStaticEdit(0, to));
        value_.reset(CommonUI::CreateStaticEdit(0, value));
        time_.reset(CommonUI::CreateStaticEdit(0, time));
    }

    void CTransactionItem::OnUICreate()
    {
        SetEditProperty(from_);
        SetEditProperty(to_);
        SetEditProperty(value_);
        SetEditProperty(time_);
        if (type_)
            type_->UICreate(this);
    }

    void CTransactionItem::SetEditProperty(std::unique_ptr<CUIEdit>& edit)
    {
        edit->UICreate(this);
        edit->UISetReadOnly();
    }

    void CTransactionItem::OnUISize()
    {
        Layout();
    }

    void CTransactionItem::Layout()
    {
        auto rc = UIGetClientRect();
        if (!rc.IsRectEmpty())
        {
            int total_width = 2 * ADDRESS_WIDTH + TIME_WIDTH + VALUE_WIDTH + TYPE_WIDTH + 4 * TRANSACTION_ITEM_GAP;
            int left = TRANSACTION_ITEM_GAP;
            auto top = (rc.Height() - ITEM_HEIGHT) / 2;
            from_->UISetPos(left, 0, ADDRESS_WIDTH, rc.Height());
            left += ADDRESS_WIDTH + TRANSACTION_ITEM_GAP;
            if (type_)
                type_->UISetPos(left, top, TYPE_WIDTH, ITEM_HEIGHT);
            left += TYPE_WIDTH + TRANSACTION_ITEM_GAP * 3;
            to_->UISetPos(left, top, ADDRESS_WIDTH, ITEM_HEIGHT);
            left += ADDRESS_WIDTH + TRANSACTION_ITEM_GAP;
            value_->UISetPos(left, top, VALUE_WIDTH, ITEM_HEIGHT);
            left += VALUE_WIDTH + TRANSACTION_ITEM_GAP;
            time_->UISetPos(left, top, TIME_WIDTH, ITEM_HEIGHT);
        }
    }
}

