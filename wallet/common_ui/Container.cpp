#include "Container.h"

#include <algorithm>
#include "UI.h"
#include "CommonDef.h"
#include "UILite/Controls/UIButton.h"
#include "UILite/Controls/UIEdit.h"
#include "base/logging.h"
#include "base/sys_string_conversions.h"

#define ITEM_MARGIN       0

CUIContainer::CUIContainer()
{
}

CUIContainer::~CUIContainer()
{
  auto deleter = [](CUIObject* pObj) { delete pObj; };
  std::for_each(m_vecRadios.begin(), m_vecRadios.end(), deleter);
  std::for_each(m_vecCheckBoxes.begin(), m_vecCheckBoxes.end(), deleter);
  std::for_each(m_vecEdits.begin(), m_vecEdits.end(), deleter);
  std::for_each(m_vecSeprators.begin(), m_vecSeprators.end(), deleter);
  m_vecRadios.clear();
  m_vecCheckBoxes.clear();
  m_vecEdits.clear();
  m_vecSeprators.clear();
}

CUIRadio* CUIContainer::GetRadio(DWORD dwControlID)
{
  for (auto pRadio : m_vecRadios)
  {
    if (pRadio->GetControlID() == dwControlID)
      return pRadio;
  }
  return nullptr;
}

CUICheckBox* CUIContainer::GetCheckBox(DWORD dwControlID)
{
  auto func = [dwControlID](CUIControl* pUIControl) {
    return dwControlID == pUIControl->GetControlID();
  };

  auto itCheckBox = std::find_if(m_vecCheckBoxes.begin(), m_vecCheckBoxes.end(), func);
  if (itCheckBox != m_vecCheckBoxes.end())
    return *itCheckBox;

  return nullptr;
}

void CUIContainer::AddRadio(const std::wstring& strText, DWORD_PTR dwControlID, DWORD iGroupId, bool iCheck)
{
  auto pRadiok = CommonUI::CreateRadio(strText.c_str());
  pRadiok->SetGroupID(iGroupId);
  pRadiok->SetControlID(dwControlID);
  pRadiok->SetCheck(iCheck);

  auto it = std::find_if(m_vecRadios.begin(), m_vecRadios.end(), 
    [iGroupId](CUIRadio* pRadio) { return pRadio->GetGroupID() == iGroupId; });

  if (it == m_vecRadios.end())
    CreateSeperator();

  m_vecRadios.push_back(pRadiok);
  if (UIIsCreated())
  {
    pRadiok->UICreate(this);
    Layout();
  }
}

void CUIContainer::AddEdit(const std::wstring& strText, DWORD_PTR dwControlID, bool bReadOnly)
{
  auto pEdit = CommonUI::CreateEdit(dwControlID);
  pEdit->UISetText(strText.c_str());
  pEdit->UISetTextColor(0xffffff);
  pEdit->UISetReadOnly(bReadOnly);
  m_vecEdits.push_back(pEdit);
  if (UIIsCreated())
  {
    pEdit->UICreate(this);
    Layout();
  }
}

void CUIContainer::OnUICreate()
{
  UIModifyStyle(UIStyle_AutoScroll | UIStyle_VScroll, 0);
  UISetScrollbar(CommonUI::CreateVScrollBar(), true);

  for (auto pRadio : m_vecRadios)
    pRadio->UICreate(this);
  for (auto pCheckBox : m_vecCheckBoxes)
    pCheckBox->UICreate(this);
  for (auto pSeperator : m_vecSeprators)
    pSeperator->UICreate(this);
  for (auto pEdit : m_vecEdits)
    pEdit->UICreate(this);

  Layout();
}

void CUIContainer::OnUISize()
{
  Layout();
}

void CUIContainer::Layout()
{
  CRect rcClient = UIGetClientRect();
  int iTop = 0;
  int iLeft = ITEM_MARGIN;
  
  DWORD dwLastGroupID = -1;
  size_t indexSeperator = 0;

  auto func_radio = [this, &iTop, &dwLastGroupID, &indexSeperator, iLeft, rcClient](CUIRadio* pRadio) {
    if (pRadio->GetGroupID() != dwLastGroupID)
    {
      if (dwLastGroupID != -1)
      {
        m_vecSeprators[indexSeperator]->UISetPos(0, iTop, rcClient.Width(), 2);
        indexSeperator++;
        iTop += CommonUI::kButtonSpace + 2;
      }
      dwLastGroupID = pRadio->GetGroupID();
    }

    auto sz = pRadio->UIMeasure();
    pRadio->UISetPos(CRect(iLeft, iTop, iLeft + sz.cx, iTop + sz.cy));
    iTop += sz.cy + CommonUI::kButtonSpace;
  };

  auto func = [&iTop, iLeft, rcClient](CUIObject* pObj) {
    auto sz = pObj->UIMeasure();
    if (pObj->UIGetObjectType() == UIObject_Edit)
    {
      pObj->UISetPos(iLeft, iTop, rcClient.Width(), sz.cy);
    }
    else
    {
      pObj->UISetPos(CRect(iLeft, iTop, iLeft + sz.cx, iTop + sz.cy));
    }

    iTop += sz.cy + CommonUI::kButtonSpace;
  };

  std::for_each(m_vecEdits.begin(), m_vecEdits.end(), func);
  std::for_each(m_vecRadios.begin(), m_vecRadios.end(), func_radio);
  std::for_each(m_vecCheckBoxes.begin(), m_vecCheckBoxes.end(), func);

  UIUpdateScrollbar();
  UIRedraw();
}

void CUIContainer::CreateSeperator()
{
  DWORD dwControlID = m_vecSeprators.size() + 1;
  auto pEdit = CommonUI::CreateEdit(dwControlID);
  pEdit->UIEnable(false);
  m_vecSeprators.push_back(pEdit);
}

LRESULT CUIContainer::OnUIClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
  if (m_clickCallback)
    m_clickCallback(pSender, dwControlID, pNotifyHeader);

  return S_OK;
}

LRESULT CUIContainer::OnEnKillFocus(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
{
  if (m_killFocusCallback)
    m_killFocusCallback(pSender, dwControlID, pNotifyHeader);

  return S_OK;
}




