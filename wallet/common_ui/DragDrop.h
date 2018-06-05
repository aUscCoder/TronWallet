// Author: miaojinquan
#pragma once
#include "common_ui/CommonInclude.h"
#include <objidl.h>
#define CF_OBJITEM        ::RegisterClipboardFormat(L"ThorDesigner_ObjectItem_DATA_FORMAT")

class CDragSource;
class CDragDropMgr;
class CDragHelper;
class CDragDropObject : public IDataObject
{
public:

    CDragDropObject(CDragSource* pSrc);
    ~CDragDropObject();

    CDragSource* GetSrc() const { return m_pSource; }

    void CopyMedium(STGMEDIUM* pMedDest, STGMEDIUM* pMedSrc, FORMATETC* pFmtSrc);
    //IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    //IDataObject
    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetData(
        /* [unique][in] */ FORMATETC __RPC_FAR *pformatetcIn,
        /* [out] */ STGMEDIUM __RPC_FAR *pmedium);

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetDataHere(
        /* [unique][in] */ FORMATETC __RPC_FAR *pformatetc,
        /* [out][in] */ STGMEDIUM __RPC_FAR *pmedium);

    virtual HRESULT STDMETHODCALLTYPE QueryGetData(
        /* [unique][in] */ FORMATETC __RPC_FAR *pformatetc);

    virtual HRESULT STDMETHODCALLTYPE GetCanonicalFormatEtc(
        /* [unique][in] */ FORMATETC __RPC_FAR *pformatectIn,
        /* [out] */ FORMATETC __RPC_FAR *pformatetcOut);

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetData(
        /* [unique][in] */ FORMATETC __RPC_FAR *pformatetc,
        /* [unique][in] */ STGMEDIUM __RPC_FAR *pmedium,
        /* [in] */ BOOL fRelease);

    virtual HRESULT STDMETHODCALLTYPE EnumFormatEtc(
        /* [in] */ DWORD dwDirection,
        /* [out] */ IEnumFORMATETC __RPC_FAR *__RPC_FAR *ppenumFormatEtc);

    virtual HRESULT STDMETHODCALLTYPE DAdvise(
        /* [in] */ FORMATETC __RPC_FAR *pformatetc,
        /* [in] */ DWORD advf,
        /* [unique][in] */ IAdviseSink __RPC_FAR *pAdvSink,
        /* [out] */ DWORD __RPC_FAR *pdwConnection);

    virtual HRESULT STDMETHODCALLTYPE DUnadvise(
        /* [in] */ DWORD dwConnection);

    virtual HRESULT STDMETHODCALLTYPE EnumDAdvise(
        /* [out] */ IEnumSTATDATA __RPC_FAR *__RPC_FAR *ppenumAdvise);

protected:

    long m_cRefCount;
    std::vector<FORMATETC*> m_ArrFormatEtc;
    std::vector<STGMEDIUM*> m_StgMedium;
    CDragSource*            m_pSource;
};

class CDragSource : public IDropSource
{

public:

    CDragSource() : m_cRefCount(0), m_bDropped(false) { AddRef(); }
    
    void SetDragHelper(CDragHelper* pHelper) { m_pDragHelper = pHelper; }
    CDragHelper* GetDragHelper() { return m_pDragHelper; }

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDropSource
    HRESULT STDMETHODCALLTYPE QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect);

    virtual HRESULT OnGiveFeedback(DWORD dwEffect) { return DRAGDROP_S_USEDEFAULTCURSORS; }

protected:

    bool m_bDropped;
    long m_cRefCount;

    CDragHelper* m_pDragHelper;
};

class CDragHelper
{
public:

    explicit CDragHelper(CDragSource* pSrc, CPoint ptOffset = CPoint(0, 0));
    ~CDragHelper();

    static bool IsValidDragDistance(CPoint ptLast, CPoint ptCur);

    void SetData(void* pData);
    void SetBitmap(HBITMAP hBMP, CPoint ptOffset = CPoint(0, 0));
    CPoint GetOffset() const { return m_ptOffset; }
    DWORD BeginDrag(DWORD dwProposedAction = DROPEFFECT_MOVE | DROPEFFECT_COPY, DWORD dwEffect = 0);

private:

    CDragDropObject*    m_pObj;
    CDragSource*        m_pSrc;
    IDragSourceHelper*    m_pdsh;
    CPoint                m_ptOffset;
};