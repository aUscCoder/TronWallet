// Author: ChenJianjun
#pragma once
#include "../../Universal.h"
#include "NativeAPIImpl.h"


class CUIRoot;
class CNativeAPI
{
public:

	explicit CNativeAPI(CUIRoot* pUIRoot) : m_pUIRoot(pUIRoot)
	{

	}

	~CNativeAPI()
	{

	}

	BEGIN_MSG_MAP(CNativeAPI)
		MESSAGE_HANDLER(WM_UILITE_NATIVE_API, OnNativeAPICall)
	END_MSG_MAP()

	UNIVERSAL_LIB_API LRESULT OnNativeAPICall(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnVerifyAPI(HANDLE hFileMapping);
	LRESULT OnGetFocusedClassName(HANDLE hFileMapping);
	LRESULT OnGetFocusedText(HANDLE hFileMapping);
	LRESULT OnGetVersion(HANDLE hFileMapping);
	LRESULT OnGetFocusedTypeString(HANDLE hFileMapping);
	LRESULT OnGetFocusedEditSel(HANDLE hFileMapping);
	LRESULT OnGetFocusedEditSelText(HANDLE hFileMapping);
	LRESULT OnGetObjectPoint(HANDLE hFileMapping);
	LRESULT OnGetObjectParent(HANDLE hFileMapping);
	LRESULT OnReleaseObjectID(HANDLE hFileMapping);
	LRESULT OnGetObjectType(HANDLE hFileMapping);
	LRESULT OnHighlightObject(HANDLE hFileMapping);

private:

	bool TestFileMapping(HANDLE hFileMapping);

	CUIRoot* m_pUIRoot;
};
