// Author: ChenJianjun
#pragma once
#include "UIButton.h"


struct UIComboBtnClick
{
	UINH hdr;
	int iButton;
};

struct AdditionalButton
{
	std::wstring strText;
	int iIndent;
	int iImage;
	DWORD_PTR dwData;
};

class CUIDropdownList;
class CUIComboButton: public CUIButton, public IHookMessage
{
public:

	UNIVERSAL_LIB_API explicit CUIComboButton(DWORD_PTR dwControlID = 0, LPCWSTR lpszText = NULL);
	~CUIComboButton();
	
	UNIVERSAL_LIB_API int AddButton(LPCWSTR lpszText, int iImage, DWORD_PTR dwData, int iIndent = 4);
	UNIVERSAL_LIB_API void RemoveAllButtons();
	int GetButtonCount() const { return m_vecAdditional.size(); }

	bool SetButtonImage(int iButton, int iImage);
	bool SetButtonIndent(int iButton, int iIndent);
	bool SetButtonData(int iButton, DWORD_PTR dwData);
	void SetImageList(HIMAGELIST hImageList) { m_imageList = hImageList; }
	void SetDropdownShadow(HBITMAP hbm) { m_hbmShadow = hbm; }
	DWORD_PTR GetButtonData(int iButton) const;
	LPCWSTR GetButtonText(int iButton) const;
	int GetDropdownPartWidth() const { return m_iDropdownPartWidth; }
	void SetDropdownPartWidth(int iWidth);
	void SetDropdownMargin(int iMargin) { m_iDropdownMargin = iMargin; }
	void SetDropdownItemHeight(int iHeight) { m_iDropdownItemHeight = iHeight; }
	void SetDropdownButtonBkPainter(IPainter* pPainter, bool bManaged = true);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus);
	void OnUIPaint(CDCHandle dc, DWORD dwStatus);
	bool OnUIMeasureClient(SIZE& sz);
	void OnUIMouseMove(UINT nFlags, POINT point);
	void OnUIMouseLeave();
	void OnUIKillFocus();
	LRESULT OnUIWantEnter(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnUIWantEscape(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);
	LRESULT OnDropdownListClick(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
	LRESULT OnDropdownListClose(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader);
	LRESULT OnReflectCommand(DWORD dwCommandCode, UINH* pNotifyHeader);
	LRESULT OnResetTemporarilyStatus(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled);

private:

	bool OnHookMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult) override;
	void ShowDropdown();
	void HideDropdown();

	CImageList m_imageList;
	HBITMAP m_hbmShadow;
	int m_iDropdownPartWidth;
	int m_iDropdownMargin;
	int m_iDropdownItemHeight;
	bool m_bDisableDropdownTemporarily;

	CUIDropdownList* m_pDropdownList;
	IPainter* m_pItemBkPainter;
	bool m_bItemBkPainterManaged;

	std::vector<AdditionalButton*> m_vecAdditional;
};

//////////////////////////////////////////////////////////////////////////

inline void CUIComboButton::SetDropdownButtonBkPainter(IPainter* pPainter, bool bManaged)
{
	if (m_pItemBkPainter && m_bItemBkPainterManaged)
		delete m_pItemBkPainter;

	m_pItemBkPainter = pPainter;
	m_bItemBkPainterManaged = bManaged;
}

inline DWORD_PTR CUIComboButton::GetButtonData(int iButton) const
{
	if (iButton < 0 || iButton >= (int)m_vecAdditional.size())
		return NULL;
	return m_vecAdditional[iButton]->dwData;
}

inline LPCWSTR CUIComboButton::GetButtonText(int iButton) const
{
	if (iButton < 0 || iButton >= (int)m_vecAdditional.size())
		return L"";
	return m_vecAdditional[iButton]->strText.c_str();
}

inline void CUIComboButton::SetDropdownPartWidth(int iWidth)
{
	if (iWidth != m_iDropdownPartWidth)
	{
		m_iDropdownPartWidth = iWidth;
		UIRedraw();
	}
}

inline bool CUIComboButton::SetButtonImage(int iButton, int iImage)
{
	if (iButton < 0 || iButton >= (int)m_vecAdditional.size())
		return false;

	m_vecAdditional[iButton]->iImage = iImage;
	return true;
}

inline bool CUIComboButton::SetButtonIndent(int iButton, int iIndent)
{
	if (iButton < 0 || iButton >= (int)m_vecAdditional.size())
		return false;

	m_vecAdditional[iButton]->iIndent = iIndent;
	return true;

}

inline bool CUIComboButton::SetButtonData(int iButton, DWORD_PTR dwData)
{
	if (iButton < 0 || iButton >= (int)m_vecAdditional.size())
		return false;

	m_vecAdditional[iButton]->dwData = dwData;
	return true;

}
