#include "DragDrop.h"
#include "DropTarget.h"


CDragDropObject::CDragDropObject(CDragSource* pSrc) : m_cRefCount(0), m_pSource(pSrc)
{
}

CDragDropObject::~CDragDropObject()
{
    for (int i = 0; i < (int)m_StgMedium.size(); i++)
    {
        ReleaseStgMedium(m_StgMedium[i]);
        delete m_StgMedium[i];
    }
    for (int j = 0; j < (int)m_ArrFormatEtc.size(); j++)
        delete m_ArrFormatEtc[j];
}

void CDragDropObject::CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc)
{
    switch (pMedSrc->tymed)
    {
    case TYMED_HGLOBAL:
        pMedDest->hGlobal = (HGLOBAL)OleDuplicateData(pMedSrc->hGlobal, pFmtSrc->cfFormat, NULL);
        break;
    case TYMED_GDI:
        pMedDest->hBitmap = (HBITMAP)OleDuplicateData(pMedSrc->hBitmap, pFmtSrc->cfFormat, NULL);
        break;
    case TYMED_MFPICT:
        pMedDest->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(pMedSrc->hMetaFilePict, pFmtSrc->cfFormat, NULL);
        break;
    case TYMED_ENHMF:
        pMedDest->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(pMedSrc->hEnhMetaFile, pFmtSrc->cfFormat, NULL);
        break;
    case TYMED_FILE:
        pMedSrc->lpszFileName = (LPOLESTR)OleDuplicateData(pMedSrc->lpszFileName, pFmtSrc->cfFormat, NULL);
        break;
    case TYMED_ISTREAM:
        pMedDest->pstm = pMedSrc->pstm;
        pMedSrc->pstm->AddRef();
        break;
    case TYMED_ISTORAGE:
        pMedDest->pstg = pMedSrc->pstg;
        pMedSrc->pstg->AddRef();
        break;
    case TYMED_NULL:
    default:
        break;
    }
    pMedDest->tymed = pMedSrc->tymed;
    pMedDest->pUnkForRelease = NULL;
    if (pMedSrc->pUnkForRelease != NULL)
    {
        pMedDest->pUnkForRelease = pMedSrc->pUnkForRelease;
        pMedSrc->pUnkForRelease->AddRef();
    }
}

HRESULT STDMETHODCALLTYPE CDragDropObject::QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
{
    *ppvObject = NULL;
    if (IID_IUnknown == riid || IID_IDataObject == riid)
        *ppvObject = this;
    
    if (NULL != *ppvObject)
    {
        ((LPUNKNOWN)*ppvObject)->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CDragDropObject::AddRef(void)
{
    return ++m_cRefCount;
}

ULONG STDMETHODCALLTYPE CDragDropObject::Release(void)
{
    long nTemp;
    nTemp = --m_cRefCount;
    if (nTemp == 0)
        delete this;
    return nTemp;
}

/* [local] */ HRESULT STDMETHODCALLTYPE CDragDropObject::GetData(/* [unique][in] */ FORMATETC __RPC_FAR *pformatetcIn, /* [out] */ STGMEDIUM __RPC_FAR *pmedium)
{
    if (pformatetcIn == NULL || pmedium == NULL)
        return E_INVALIDARG;
    pmedium->hGlobal = NULL;

    ATLASSERT(m_StgMedium.size() == m_ArrFormatEtc.size());
    for (int i = 0; i < (int)m_ArrFormatEtc.size(); ++i)
    {
        if (pformatetcIn->tymed & m_ArrFormatEtc[i]->tymed
            && pformatetcIn->dwAspect == m_ArrFormatEtc[i]->dwAspect
            && pformatetcIn->cfFormat == m_ArrFormatEtc[i]->cfFormat)
        {
            CopyMedium(pmedium, m_StgMedium[i], m_ArrFormatEtc[i]);
            return S_OK;
        }
    }
    return DV_E_FORMATETC;
}

/* [local] */ HRESULT STDMETHODCALLTYPE CDragDropObject::GetDataHere(/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc, /* [out][in] */ STGMEDIUM __RPC_FAR *pmedium)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDragDropObject::QueryGetData(/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc)
{
    if (pformatetc == NULL)
        return E_INVALIDARG;

    //support others if needed DVASPECT_THUMBNAIL  //DVASPECT_ICON   //DVASPECT_DOCPRINT
    if (!(DVASPECT_CONTENT & pformatetc->dwAspect))
        return (DV_E_DVASPECT);

    HRESULT  hr = DV_E_TYMED;
    for (int i = 0; i < (int)m_ArrFormatEtc.size(); ++i)
    {
        if (pformatetc->tymed & m_ArrFormatEtc[i]->tymed)
        {
            if (pformatetc->cfFormat == m_ArrFormatEtc[i]->cfFormat)
                return S_OK;
            else
                hr = DV_E_CLIPFORMAT;
        }
        else
        {
            hr = DV_E_TYMED;
        }
    }
    return hr;
}

HRESULT STDMETHODCALLTYPE CDragDropObject::GetCanonicalFormatEtc(/* [unique][in] */ FORMATETC __RPC_FAR *pformatectIn, /* [out] */ FORMATETC __RPC_FAR *pformatetcOut)
{
    if (pformatetcOut == NULL)
        return E_INVALIDARG;
    return DATA_S_SAMEFORMATETC;
}

/* [local] */ HRESULT STDMETHODCALLTYPE CDragDropObject::SetData(/* [unique][in] */ FORMATETC __RPC_FAR *pformatetc, /* [unique][in] */ STGMEDIUM __RPC_FAR *pmedium, /* [in] */ BOOL fRelease)
{
    if (pformatetc == NULL || pmedium == NULL)
        return E_INVALIDARG;

    ATLASSERT(pformatetc->tymed == pmedium->tymed);
    FORMATETC* fetc = new FORMATETC;
    STGMEDIUM* pStgMed = new STGMEDIUM;

    if (fetc == NULL || pStgMed == NULL)
        return E_OUTOFMEMORY;

    ZeroMemory(fetc, sizeof(FORMATETC));
    ZeroMemory(pStgMed, sizeof(STGMEDIUM));

    *fetc = *pformatetc;
    m_ArrFormatEtc.push_back(fetc);

    if (fRelease)
    {
        *pStgMed = *pmedium;
    }
    else
    {
        CopyMedium(pStgMed, pmedium, pformatetc);
    }
    m_StgMedium.push_back(pStgMed);

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CDragDropObject::EnumFormatEtc(/* [in] */ DWORD dwDirection, /* [out] */ IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenumFormatEtc)
{
    if (ppenumFormatEtc == NULL)
        return E_POINTER;

    *ppenumFormatEtc = NULL;
    switch (dwDirection)
    {
    case DATADIR_GET:
        //        *ppenumFormatEtc= new CEnumFormatEtc(m_ArrFormatEtc);
        if (*ppenumFormatEtc == NULL)
            return E_OUTOFMEMORY;
        (*ppenumFormatEtc)->AddRef();
        break;

    case DATADIR_SET:
    default:
        return E_NOTIMPL;
        break;
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CDragDropObject::DAdvise(FORMATETC __RPC_FAR *pformatetc, DWORD advf,
    IAdviseSink __RPC_FAR *pAdvSink,  DWORD __RPC_FAR *pdwConnection)
{
    return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT STDMETHODCALLTYPE CDragDropObject::DUnadvise(/* [in] */ DWORD dwConnection)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CDragDropObject::EnumDAdvise(/* [out] */ IEnumSTATDATA __RPC_FAR *__RPC_FAR *ppenumAdvise)
{
    return OLE_E_ADVISENOTSUPPORTED;
}

//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE CDragSource::QueryInterface(REFIID riid, void **ppvObject)
{
    *ppvObject = NULL;
    if (IID_IUnknown == riid || IID_IDropSource == riid)
        *ppvObject = this;

    if (*ppvObject != NULL)
    {
        ((LPUNKNOWN)*ppvObject)->AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CDragSource::AddRef()
{
    return ++m_cRefCount;
}

ULONG STDMETHODCALLTYPE CDragSource::Release()
{
    long nTemp;
    nTemp = --m_cRefCount;
    ATLASSERT(nTemp >= 0);
    if (nTemp == 0)
        delete this;
    return nTemp;
}

HRESULT STDMETHODCALLTYPE CDragSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
    if (fEscapePressed)
        return DRAGDROP_S_CANCEL;
    if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON)))
    {
        m_bDropped = true;
        return DRAGDROP_S_DROP;
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CDragSource::GiveFeedback(DWORD dwEffect)
{
    return OnGiveFeedback(dwEffect);
}

//////////////////////////////////////////////////////////////////////////

CDragHelper::CDragHelper(CDragSource* pSrc, CPoint ptOffset) : m_pSrc(pSrc), m_ptOffset(ptOffset)
{
    m_pObj = new CDragDropObject(pSrc);
    pSrc->SetDragHelper(this);
    m_pObj->AddRef();
    m_pSrc->AddRef();

    CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC_SERVER,
        IID_IDropTargetHelper, (LPVOID*)&m_pdsh);
}

CDragHelper::~CDragHelper()
{
    m_pObj->Release();
    m_pSrc->AddRef();
}

void CDragHelper::SetData(void* pData)
{
    FORMATETC fmtetc = { CF_OBJITEM, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    STGMEDIUM medium = { TYMED_HGLOBAL };
    medium.hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, sizeof(int));

    BYTE* pMem = (BYTE*)::GlobalLock(medium.hGlobal);
    memcpy(pMem, &pData, sizeof(int));
    ::GlobalUnlock(medium.hGlobal);

    m_pObj->SetData(&fmtetc, &medium, FALSE);
}

DWORD CDragHelper::BeginDrag(DWORD dwProposedAction, DWORD dwEffect)
{
    return ::DoDragDrop(m_pObj, m_pSrc, dwProposedAction, &dwEffect);
}

bool CDragHelper::IsValidDragDistance(CPoint ptLast, CPoint ptCur)
{
    return (ptCur.x - ptLast.x) * (ptCur.x - ptLast.x) + (ptCur.y - ptLast.y) * (ptCur.y - ptLast.y) > 9;
}

void CDragHelper::SetBitmap(HBITMAP hBMP, CPoint ptOffset)
{
    CBitmapHandle bmpHandler(hBMP);
    CSize sz;
    bmpHandler.GetSize(sz);
    SHDRAGIMAGE dragImage = { sz, ptOffset, NULL, 0 };
    m_pdsh->InitializeFromBitmap(&dragImage, m_pObj);
}
