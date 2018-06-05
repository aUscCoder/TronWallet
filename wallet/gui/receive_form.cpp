#include "receive_form.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "common_ui/CommonDef.h"
#include "QRCode/IQRCode.h"
#include "CommonLib/UI/Paint.h"
#include "core/account_manager.h"

#define RECEIVEFORM_WIDTH 400
#define RECEIVEFORM_HEIGHT 400
#define RECEIVEFORM_ITEM_HEIGHT 30
#define RECEIVEFORM_VERT_GAP 20
#define QRCODE_SIZE 200
#define RECEIVEFORM_MARGIN 30

using namespace wallet_core;
using namespace wallet_gui;

CReceiveForm::CReceiveForm() : CBaseForm(L"Receive TRX", true)
{
    address_.reset(CommonUI::CreateStaticEdit(0, L""));
}

class CWhiteBKPainter : public IPainter
{
public:
    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha) override
    {
        Gdiplus::Graphics g(dc);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        CommonUI::Paint::FillRoundRect(&g, Gdiplus::Rect(rc.left, rc.top,
            rc.Width(), rc.Height()), Gdiplus::Color(255, 255, 255), 4);
        return S_OK;
    }
};

void CReceiveForm::OnUICreate()
{
    UISetSize(RECEIVEFORM_WIDTH, RECEIVEFORM_HEIGHT);
    CBaseForm::OnUICreate();
    address_->UICreate(this);
    address_->UISetBkPainter(new CWhiteBKPainter);
    address_->UISetReadOnly();
    SetTitle(L"Receive TRX", 80);
}

void CReceiveForm::Layout()
{
    CBaseForm::Layout();
    auto rc = UIGetClientRect();
    int top = GetTitleHeight();
    top += QRCODE_SIZE + RECEIVEFORM_VERT_GAP;
    address_->UISetPos(RECEIVEFORM_MARGIN, top, rc.Width() - 2 * RECEIVEFORM_MARGIN, RECEIVEFORM_ITEM_HEIGHT);
}

void CReceiveForm::OnUIPaint(CDCHandle dc, DWORD dwStatus)
{
    CBaseForm::OnUIPaint(dc, dwStatus);
    auto rc = UIGetClientRect();
    auto qrcode_top = GetTitleHeight();
    if (!bmp_qrcode_.IsNull())
    {
        CSize size;
        bmp_qrcode_.GetSize(size);
        Universal::UI::Paint::DrawImage(dc, CRect({ (rc.Width() - size.cx) / 2 , qrcode_top } , size), bmp_qrcode_);
    }
}

void CReceiveForm::OnUIShow(bool bShow)
{
    if (bShow)
    {
        auto encoded_address = AccountManager::_().EncodedAddress();
        if (encoded_address != str_address_)
        {
            str_address_ = encoded_address;
            DWORD size = 0;
            auto data = QRCode::GenerateQRCode(QRCODE_SIZE, encoded_address, &size);
            if (data && size > 0)
                bmp_qrcode_ = Universal::UI::Bitmap::LoadFromBuffer(data, size);
            address_->UISetText(base::UTF8ToWide(str_address_));
        }
    }
}

