#include "base_form.h"
#include "wallet_evnets.h"

using namespace wallet_gui;

CBaseForm::CBaseForm(const std::wstring& title, bool full) : full_(full)
{
    if (!title.empty())
        title_.reset(new CUIStatic(title, true, false, true));
}

int CBaseForm::GetTitleHeight()
{
    int height = 0;
    if (title_)
    {
        height = title_->UIGetRect().bottom;
    }
    return height;
}

void CBaseForm::SetTitle(const std::wstring& title, int height)
{
    if (title_)
        title_->UISetText(title);
    else
    {
        title_.reset(new CUIStatic(title, true, false, true));
        if (UIIsCreated())
            title_->UICreate(this);
    }

    title_height_ = height;
    title_->UISetFont(CommonUI::UI::GetBigFont());
    title_->UISetTextColor(CommonUI::white);
    Layout();
}

void CBaseForm::CreateSubItems()
{
    for (const auto& s : item_infos_)
    {
        std::unique_ptr<CUIObject> sub;
        if (s.control_infos.size() == 1)
        {
            sub.reset(CreateControl(s.control_infos[0]));
        }
        else
        {
            auto item = new CommonUI::CCommonListViewItem;
            item->SetEqually(s.average_);
            using namespace std::placeholders;
            item->SetClickCallback(std::bind(&CBaseForm::OnButtonClick, this, _1));
            item->SetEditTextChangeCallback(std::bind(&CBaseForm::OnEditTextChange, this, _1));
            for (const auto& info : s.control_infos)
            {
                item->Push(std::shared_ptr<CUIObject>(CreateControl(info)));
            }
            sub.reset(item);
        }
        sub_items_.push_back(std::move(sub));
    }

    for (auto& s : sub_items_)
        s->UICreate(this);

    Layout();
}

CUIObject* CBaseForm::GetSub(DWORD id)
{
    CUIObject* sub = nullptr;
    auto Find = [this, id] ()
    {
        std::tuple<bool, size_t, size_t> result = std::make_tuple(false, 0, 0);
        for (size_t i = 0; i < item_infos_.size(); i++)
        {
            const auto& infos = item_infos_[i];
            for (size_t j = 0; j < infos.control_infos.size(); j++)
            {
                if (infos.control_infos[j].id_ == id)
                {
                    result = std::make_tuple(true, i, j);
                }
            }
        }
        return result;
    };
    
    size_t i = 0, j = 0;
    bool get = false;
    std::tie(get, i, j) = Find();

    if (get)
    {
        if (item_infos_[i].control_infos.size() > 1)
        {
            auto item = (CommonUI::CCommonListViewItem*)sub_items_[i].get();
            sub = item->GetSub(j).get();
        }
        else
            sub = sub_items_[i].get();
    }
    return sub;
}

void CBaseForm::OnUICreate()
{
    if (title_)
    {
        title_->UICreate(this);
        title_->UISetFont(CommonUI::UI::GetBigFont());
        title_->UISetTextColor(CommonUI::white);
    }
    CreateSubItems();
}

void CBaseForm::OnUIDestroy()
{
    if (UIGetParent())
        UIGetParent()->UIFireEvent(Event_Sub_Form_Destoy, (WPARAM)this);
}

void CBaseForm::OnUISize()
{
    Layout();
}

void CBaseForm::Layout()
{    
    CRect rc = UIGetClientRect();
    int top = 0;
    if (title_)
    {
        title_->UISetSize(rc.Width(), title_height_);
        top = title_height_;
    }
 
    for (size_t i = 0; i < item_infos_.size(); i++)
    {
        auto& row_info = item_infos_[i];
        auto margin_x = row_info.magin_.cx ? row_info.magin_.cx : CommonUI::kDefaultMargin;
        auto margin_y = row_info.magin_.cy ? row_info.magin_.cy : CommonUI::kDefaultMargin;
        auto width = row_info.size_.cx ? row_info.size_.cx : rc.Width() - 2 * margin_x;
        auto height = row_info.size_.cy ? row_info.size_.cy : 30;
        sub_items_[i]->UISetPos(margin_x, top, width, height);
        top += height + margin_y;
    }
}

void CBaseForm::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    auto rc = UIGetClientRect();
    Gdiplus::Graphics g(dc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    CommonUI::Paint::FillRoundRect(&g, Gdiplus::Rect(rc.left, rc.top,
        rc.Width(), rc.Height()), Gdiplus::Color(27, 27, 27), CommonUI::kDefaultMargin);

    CommonUI::Paint::DrawRoundRect(&g, Gdiplus::Rect(rc.left, rc.top,
        rc.Width(), rc.Height()), Gdiplus::Color(255, 255, 255), 10, 1);

    if (!full_)
    {
        auto top = GetTitleHeight();
        if (tail_height_ == 0)
        {
            if (top == 0)
            {
                CommonUI::Paint::FillRoundRect(&g, Gdiplus::Rect(rc.left, top,
                    rc.Width(), rc.Height()), Gdiplus::Color(255, 255, 255), CommonUI::kDefaultMargin);
            }
            else
            {
                CRect middle(0, top, rc.Width(), top + 15);
                dc.FillSolidRect(middle, CommonUI::white);
                rc.top = middle.bottom - 10;
                CommonUI::Paint::FillRoundRect(&g, Gdiplus::Rect(rc.left, rc.top,
                    rc.Width(), rc.Height()), Gdiplus::Color(255, 255, 255), CommonUI::kDefaultMargin);
            }
        }
        else
        {
            auto tail_top = rc.Height() - tail_height_;
            CRect body_rc(0, top, rc.Width(), tail_top);
            dc.FillSolidRect(body_rc, CommonUI::white);
        }
    }
}

void CBaseForm::OnButtonClick(DWORD dwControlID)
{
}

void CBaseForm::OnEditTextChange(DWORD dwControlID)
{
}

LRESULT CBaseForm::OnUIBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    OnButtonClick(dwControlID);
    return S_OK;
}

LRESULT CBaseForm::OnReturn(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    return S_OK;
}

void CBaseForm::AutoHideShowStatus(const std::wstring& status)
{
    ::PostMessage(UIGetHWND(), WalletEvent::Event_Hide_Status, (WPARAM)new std::wstring(status), 0);
}

void CBaseForm::ShowStatus(const std::wstring& status)
{
    ::PostMessage(UIGetHWND(), WalletEvent::Event_Show_Status, (WPARAM)new std::wstring(status), 0);
}

void CBaseForm::MessageBox(const std::wstring& msg)
{
    ::MessageBox(UIGetHWND(), msg.c_str(), L"", MB_OK);
}

void CBaseForm::OnTransactionCreateFailed(std::shared_ptr<protocol::Transaction> transaction)
{
    // TODO(jiangru)
    AutoHideShowStatus(L"Create transaction failed!");
}
