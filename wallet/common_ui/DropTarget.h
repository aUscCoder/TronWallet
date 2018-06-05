// Author: miaojinquan
#pragma once
#include "common_ui/CommonInclude.h"
#include "UILite/Core/UIRichObject.h"

class CDragData;
class CDragSource;
class CDragEvent
{
public:

    CDragEvent(CDragSource* pSrc, CDragData* pData, DWORD dwProposedAction) :
        m_pData(pData), m_pSrc(pSrc), m_dwProposedAction(dwProposedAction), m_pt(0, 0), m_ptOffset(0, 0) {}
    ~CDragEvent() {}

    CDragData* GetData() { return m_pData; }
    CDragSource* GetSrc() { return m_pSrc; }
    void SetPos(const CPoint pt) { m_pt = pt; }
    CPoint GetPos() const { return m_pt; }

    void SetOffset(const CPoint pt) { m_ptOffset = pt; }
    CPoint GetOffset() const { return m_ptOffset; }

    DWORD GetProposedAction() const { return m_dwProposedAction; }
    void SetProposedAction(DWORD dwAction) { m_dwProposedAction = dwAction; }

private:

    CDragData*        m_pData;
    CDragSource*    m_pSrc;
    CPoint            m_pt;
    CPoint            m_ptOffset;
    DWORD            m_dwProposedAction;
};

class CDragData
{
public:

    CDragData(void* pByte = NULL, size_t szSize = 0) : m_pByte(pByte), m_szSize(szSize) {}
    ~CDragData() {}

    void SetData(void* pByte, size_t szSize);
    void* GetData() { return m_pByte; }
    size_t GetDataLen() { return m_szSize; }

private:

    void*    m_pByte = nullptr;
    size_t    m_szSize = 0;
};

bool operator == (CDragData& d1, CDragData& d2);



class CDropTarget
{
public:

    void Init();
    void Uninit();
    virtual HWND GetHWND() { return NULL; }
    virtual bool HitTest(CPoint) { return false; }
    virtual DWORD OnDragEnter(CDragEvent* pEvent) { return pEvent->GetProposedAction(); }
    virtual DWORD OnDragOver(CDragEvent* pEvent) { return pEvent->GetProposedAction(); }
    virtual DWORD OnDragLeave(CDragEvent* pEvent) { return 0; }
    virtual DWORD OnDrop(CDragEvent* pEvent) { return pEvent->GetProposedAction(); }
};

class CDragDropMgr : public IDropTarget
{
public:

    static CDragDropMgr* GetInstance()
    {
        static CDragDropMgr mgr;
        return &mgr;
    }

    ~CDragDropMgr();

    void RegisterTarget(CDropTarget* pDropTarget);
    void UnRegisterTarget(CDropTarget* pDropTarget);
    CDropTarget* GetTarget(CPoint pt);

    // IUnknown implementation
    HRESULT __stdcall QueryInterface(REFIID iid, void ** ppvObject);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);

    // IDropTarget implementation
    HRESULT __stdcall DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
    HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
    HRESULT __stdcall DragLeave(void);
    HRESULT __stdcall Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);

private:
    
    CDragDropMgr();

    // internal helper function
    DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
    bool QueryDataObject(IDataObject *pDataObject);
    
    // Private member variables
    long   m_lRefCount;
    HWND   m_hWnd;
    std::vector<CDropTarget*> m_vecDropTarget;
};

