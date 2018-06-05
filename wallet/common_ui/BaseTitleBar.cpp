#include "BaseTitleBar.h"
#include "UILite/Controls/UIButton.h"
#include "BaseWindow.h"
#include "SkinResource.h"


class CBitmapButtonPainter : public IPainter
{
public:
     
    CBitmapButtonPainter(DWORD dwID)
    {
        m_bmp = Universal::UI::Bitmap::LoadFromRes(NULL, dwID, L"PNG");
    }

    LRESULT DoPaint(CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwStatus, BYTE cAlpha)
    {
        CUIStatsImageFrameInfo info(3, 0, 1, 0, 2, 0, 1);
        Universal::UI::Paint::DrawImage(dc, pUI->UIGetClientRect(), m_bmp, info.ToFraw9Frame(dwStatus), DRAWOPT_CENTER_TO_DEST());
        return S_OK;
    }

    CBitmap m_bmp;
};

CUIBaseTitleBar::CUIBaseTitleBar(CUIBaseWindow* pWindow) : m_pWindow(pWindow)
{
    m_pBtnMin = std::make_unique<CUIImageButton>();
    m_pBtnMax = std::make_unique<CUIImageButton>();
    m_pBtnRestore = std::make_unique<CUIImageButton>();
    m_pBtnClose = std::make_unique<CUIImageButton>();
    struct 
    {
        CUIImageButton* pBmp;
        DWORD dwID;
    }arrayBtn[] = {
        { m_pBtnMin.get(), IDB_PNG_BTN_MIN },
        { m_pBtnMax.get(), IDB_PNG_BTN_MAX },
        { m_pBtnRestore.get(), IDB_PNG_BTN_RESTORE },
        { m_pBtnClose.get(), IDB_PNG_BTN_CLOSE },
    };
    for (auto& btnInfo : arrayBtn)
    {
        btnInfo.pBmp->UISetBkPainter(new CBitmapButtonPainter(btnInfo.dwID), true);
    }
}

CUIBaseTitleBar::~CUIBaseTitleBar()
{
    m_pBtnClose->UIDestroy();
    m_pBtnMax->UIDestroy();
    m_pBtnMin->UIDestroy();
    m_pBtnRestore->UIDestroy();
}

void CUIBaseTitleBar::OnUICreate()
{
    m_pBtnMin->UICreate(this);
    m_pBtnMax->UICreate(this);
    m_pBtnRestore->UICreate(this);
    m_pBtnClose->UICreate(this);
}

void CUIBaseTitleBar::OnUISize()
{
    CRect rcClient = UIGetClientRect();
    CUIImageButton* arrayBtn[] = { m_pBtnMin.get(), m_pBtnMax.get(), m_pBtnClose.get() };

    int iLeft = rcClient.right - rcClient.Height() * _countof(arrayBtn);
    for (auto& btn : arrayBtn)
    {
        btn->UISetPos(iLeft, 0, rcClient.Height(), rcClient.Height());
        iLeft += rcClient.Height();
    }

    m_pBtnRestore->UISetPos(m_pBtnMax->UIGetRect());
    if (GetWindowLong(UIGetHWND(), GWL_STYLE) & WS_MAXIMIZE)
    {
        m_pBtnRestore->UIShow(true);
        m_pBtnMax->UIShow(false);
    }
    else
    {
        m_pBtnMax->UIShow(true);
        m_pBtnRestore->UIShow(false);
    }
}

LRESULT CUIBaseTitleBar::OnBNClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
    if (pSender == m_pBtnClose.get())
    {
        m_pWindow->OnClose();
    }
    else if (pSender == m_pBtnMin.get())
    {
        m_pWindow->ShowWindow(SW_MINIMIZE);
    }
    else if (pSender == m_pBtnMax.get())
    {

        m_pWindow->ShowWindow(SW_MAXIMIZE);
    }
    else if (pSender == m_pBtnRestore.get())
    {
        m_pWindow->ShowWindow(SW_RESTORE);
    }
    return S_OK;
}
