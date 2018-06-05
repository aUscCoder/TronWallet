#pragma once
#include "CommonInclude.h"
#include "UILite/Controls/UIControlEvent.h"
#include "UILite/Core/UIObject.h"
#include "UILite/Controls/UIControlDef.h"
#include "UILite/UICrack.h"
#include <memory>

class CUIRadio;
class CUICheckBox;
class CUIEdit;

using EventCallback = std::function<LRESULT(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)>;

class CUIContainer : public CUIObject
{
public:

  CUIContainer();
  ~CUIContainer();

  void SetClickCallback(EventCallback callback) { m_clickCallback = callback; }
  void SetKillFocusCallback(EventCallback callback) { m_killFocusCallback = callback; }

  void AddRadio(const std::wstring& strText, DWORD_PTR dwControlID, DWORD iGroupId, bool iCheck);
  void AddEdit(const std::wstring& strText, DWORD_PTR dwControlID, bool bReadOnly = false);

  CUIRadio* GetRadio(DWORD dwControlID);
  CUICheckBox* GetCheckBox(DWORD dwControlID);

protected:

  BEGIN_UI_EVENT_MAP(CUIContainer)
    UI_EVENT_CREATE(OnUICreate)
    UI_EVENT_SIZE(OnUISize)
    UI_COMMAND_CODE(UIBN_CLICK, OnUIClick)
    UI_COMMAND_CODE(UIEN_KILLFOCUS, OnEnKillFocus)
  END_UI_EVENT_MAP()


  LRESULT OnUIClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
  LRESULT OnEnKillFocus(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);

  void  OnUICreate();
  void  OnUISize();
  void  Layout();

private:
  void CreateSeperator();

private:
  EventCallback m_clickCallback = nullptr;
  EventCallback m_killFocusCallback = nullptr;

  std::vector<CUIRadio*> m_vecRadios;
  std::vector<CUICheckBox*> m_vecCheckBoxes;
  std::vector<CUIEdit*> m_vecEdits;
  std::vector<CUIEdit*> m_vecSeprators;
};