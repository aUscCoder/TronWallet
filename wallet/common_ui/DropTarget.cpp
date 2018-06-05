#include "DropTarget.h"
#include "DragDrop.h"


//////////////////////////////////////////////////////////////////////////
static CDropTarget* s_curTarget = NULL;
static CDragEvent*    s_pCurDragEvent = NULL;
void CDropTarget::Init()
{
    CDragDropMgr::GetInstance()->RegisterTarget(this);
}

void CDropTarget::Uninit()
{
    CDragDropMgr::GetInstance()->UnRegisterTarget(this);
}

//////////////////////////////////////////////////////////////////////////

HRESULT __stdcall CDragDropMgr::QueryInterface(REFIID iid, void ** ppvObject)
{
    if (iid == IID_IUnknown || iid == IID_IDropTarget)
    {
        *ppvObject = this;
        return NOERROR;
    }
    *ppvObject = NULL;
    return ResultFromScode(E_NOINTERFACE);
}

ULONG __stdcall CDragDropMgr::AddRef(void)
{
    return 1;
}

ULONG __stdcall CDragDropMgr::Release(void)
{
    return 1;
}

HRESULT __stdcall CDragDropMgr::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
    *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
    FORMATETC fmtetc = { CF_OBJITEM, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM medium;
    pDataObject->GetData(&fmtetc, &medium);
    void* pData = (void*)GlobalLock(medium.hGlobal);
    GlobalUnlock(medium.hGlobal);
    CDragDropObject* pObj = (CDragDropObject*)pDataObject;
    CDragEvent *pDragEvent = new CDragEvent(pObj->GetSrc(), (CDragData*)pData, *pdwEffect);
    s_pCurDragEvent = pDragEvent;
    CDropTarget* pTarget = GetTarget(CPoint(pt.x, pt.y));
    if (pTarget)
    {
        *pdwEffect = pTarget->OnDragEnter(pDragEvent);
        s_curTarget = pTarget;
    }
    else
    {
        *pdwEffect = DROPEFFECT_NONE;
        s_curTarget = NULL;
    }

    return S_OK;
}

HRESULT __stdcall CDragDropMgr::DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
    if (s_pCurDragEvent)
    {
        *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
        CDropTarget* pTarget = GetTarget(CPoint(pt.x, pt.y));
        if (pTarget)
        {
            s_pCurDragEvent->SetProposedAction(*pdwEffect);
            if (s_curTarget != NULL && s_curTarget != pTarget)
                s_curTarget->OnDragLeave(s_pCurDragEvent);
            
            if (s_curTarget != pTarget)
            {
                s_curTarget = pTarget;
                s_curTarget->OnDragEnter(s_pCurDragEvent);
            }
            *pdwEffect = pTarget->OnDragOver(s_pCurDragEvent);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }
    }
    else
    {
        *pdwEffect = DROPEFFECT_NONE;
    }
    return S_OK;
}

HRESULT __stdcall CDragDropMgr::DragLeave(void)
{
    if (s_curTarget)
        s_curTarget->OnDragLeave(s_pCurDragEvent);
    delete s_pCurDragEvent;
    return S_OK;
}

HRESULT __stdcall CDragDropMgr::Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
    *pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
    if (s_pCurDragEvent)
    {
        s_pCurDragEvent->SetProposedAction(*pdwEffect);
        CDropTarget* pTarget = GetTarget(CPoint(pt.x, pt.y));
        if (pTarget)
        {
            *pdwEffect = pTarget->OnDrop(s_pCurDragEvent);
        }
        else
        {
            *pdwEffect = DROPEFFECT_NONE;
        }
    }
    else
    {
        *pdwEffect = DROPEFFECT_NONE;
    }

    delete s_pCurDragEvent;
    s_pCurDragEvent = NULL;
    return S_OK;
}

void CDragDropMgr::RegisterTarget(CDropTarget* pDropTarget)
{
    ::RegisterDragDrop(pDropTarget->GetHWND(), this);

    std::vector<CDropTarget*>::iterator itr = std::find(m_vecDropTarget.begin(), m_vecDropTarget.end(), pDropTarget);
    if (itr != m_vecDropTarget.end())
        return;
    
    m_vecDropTarget.push_back(pDropTarget);
}

void CDragDropMgr::UnRegisterTarget(CDropTarget* pDropTarget)
{
    std::vector<CDropTarget*>::iterator itr = std::find(m_vecDropTarget.begin(), m_vecDropTarget.end(), pDropTarget);
    if (itr == m_vecDropTarget.end())
        return;

    m_vecDropTarget.erase(itr);
}

CDropTarget* CDragDropMgr::GetTarget(CPoint pt)
{
    std::vector<CDropTarget*>::iterator itr = m_vecDropTarget.begin();
    CDropTarget* pDropTarget = NULL;
    while (itr != m_vecDropTarget.end())
    {
        CDropTarget* pTarget = *itr;
        if (pTarget->HitTest(pt))
        {
            pDropTarget = *itr;
            break;
        }
        
        ++itr;
    }
    return pDropTarget;
}

CDragDropMgr::CDragDropMgr()
{

}

CDragDropMgr::~CDragDropMgr()
{

}

DWORD CDragDropMgr::DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
    DWORD dwEffect = 0;
    // 1. 检查pt来看是否允许drop操作在某个位置
    // 2. 计算出基于grfKeyState的drop效果
    if (grfKeyState & MK_CONTROL)
    {
        dwEffect = dwAllowed & DROPEFFECT_COPY;
    }
    else if (grfKeyState & MK_SHIFT)
    {
        dwEffect = dwAllowed & DROPEFFECT_MOVE;
    }
    if (dwEffect == 0)
    {
        if (dwAllowed & DROPEFFECT_COPY) dwEffect = DROPEFFECT_COPY;
        if (dwAllowed & DROPEFFECT_MOVE) dwEffect = DROPEFFECT_MOVE;
    }
    return dwEffect;
}

bool CDragDropMgr::QueryDataObject(IDataObject *pDataObject)
{
    FORMATETC fmtetc = { CF_OBJITEM, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    // does the data object support CF_TEXT using a HGLOBAL?
    return pDataObject->QueryGetData(&fmtetc) == S_OK ? true : false;
}

void CDragData::SetData(void* pByte, size_t szSize)
{
    m_pByte = pByte;
    m_szSize = szSize;
}

bool operator==(CDragData& d1, CDragData& d2)
{
    bool bValid = false;
    do 
    {
        if (d1.GetDataLen() != d2.GetDataLen())
            break;

        const size_t iSize = std::min(d1.GetDataLen(), d2.GetDataLen());
        if (iSize == 0)
            break;

        if (memcmp(d1.GetData(), d2.GetData(), iSize) != 0)
            break;
        bValid = true;
    } while (false);
    
    return bValid;
}
