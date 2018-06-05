// Author: ChenJianjun
#pragma once


#define SetEventHandled(bHandled)	UISetHandled(bHandled)

#define DECLARE_UI_EVENT_MAP2()											\
	public:																\
		bool _bEventHandled_;											\
		bool UIIsHandled() const { return _bEventHandled_; }			\
		void UISetHandled(bool bHandled) { _bEventHandled_ = bHandled; }\
		bool _ProcessEvent(DEvent* pEvent, LRESULT& lResult);			\


#define BEGIN_UI_EVENT_MAP2(theClass)								\
	bool theClass::OnUIEvent(DEvent* pEvent, LRESULT& lResult)		\
	{																\
		bool bHandled = _bEventHandled_;							\
		bool bRet = _ProcessEvent(pEvent, lResult);					\
		_bEventHandled_ = bHandled;									\
		return bRet;												\
	}																\
	bool theClass::_ProcessEvent(DEvent* pEvent, LRESULT& lResult)	\
	{																\
		bool& bHandled = _bEventHandled_;							\
		bool bIsUIObject = true;									\
		(bIsUIObject);												\
		int Event = pEvent->iEvent;									\
		WPARAM wParam = pEvent->wParam;								\
		LPARAM lParam = pEvent->lParam;								\
		(Event);													\
		(wParam);													\
		(lParam);													\
		(lResult);													\
		(bHandled);

#define END_UI_EVENT_MAP2()										\
			return __super::OnUIEvent(pEvent, lResult);			\
	}


#define BEGIN_UI_EVENT_MAP(theClass)									\
	public:																\
		bool _bEventHandled_;											\
		bool UIIsHandled() const { return _bEventHandled_; }			\
		void UISetHandled(bool bHandled) { _bEventHandled_ = bHandled; }\
__if_exists (theClass::UICreate)										\
{																		\
		virtual bool OnUIEvent(DEvent* pEvent, LRESULT& lResult)		\
		{																\
			bool bHandled = _bEventHandled_;							\
			bool bRet = _ProcessEvent(pEvent, lResult);					\
			_bEventHandled_ = bHandled;									\
			return bRet;												\
		}																\
}																		\
__if_not_exists (theClass::UICreate)									\
{																		\
		bool ProcessChainEvent(DEvent* pEvent, LRESULT& lResult)		\
		{																\
			bool bHandled = _bEventHandled_;							\
			bool bRet = _ProcessEvent(pEvent, lResult);					\
			_bEventHandled_ = bHandled;									\
			return bRet;												\
		}																\
}																		\
		bool _ProcessEvent(DEvent* pEvent, LRESULT& lResult)			\
		{																\
			bool& bHandled = _bEventHandled_;							\
__if_exists (theClass::UICreate)										\
{																		\
			bool bIsUIObject = true;									\
			(bIsUIObject);												\
}																		\
			int Event = pEvent->iEvent;									\
			WPARAM wParam = pEvent->wParam;								\
			LPARAM lParam = pEvent->lParam;								\
			(Event);													\
			(wParam);													\
			(lParam);													\
			(lResult);													\
			(bHandled);


#define UI_DECLARE_OBJECT_TYPE(ObjType)			\
	if (UIEvent_GetObjectType == Event)			\
	{											\
		bHandled = true;						\
		lResult = (LRESULT)ObjType;				\
		return true;							\
	}											\
	if (UIEvent_GetObjectTypeStr == Event)		\
	{											\
		bHandled = true;						\
		std::wstring* p = (std::wstring*)wParam;\
		lResult = (LRESULT)ObjType;				\
		*p = L#ObjType;							\
		return true;							\
	}

#define UI_DECLARE_ORIGINAL_OBJECT_TYPE(ObjType)	\
	if (UIEvent_GetOrigObjectType == Event)			\
	{												\
		bHandled = true;							\
		lResult = (LRESULT)ObjType;					\
		return true;								\
	}												\
	if (UIEvent_GetOrigObjectTypeStr == Event)		\
	{												\
		bHandled = true;							\
		std::wstring* p = (std::wstring*)wParam;	\
		lResult = (LRESULT)ObjType;					\
		*p = L#ObjType;								\
		return true;								\
	}

#define UI_REDRAW_ON_OWN_MOUSE_OR_FOCUS()								\
	if (UIEvent_MouseEnter == Event || UIEvent_MouseLeave == Event		\
		|| UIEvent_FocusEnter == Event || UIEvent_FocusLeave == Event)	\
	{																	\
		UIRedraw();														\
	}

#define UI_COMMAND_PASSTHROUGH()															\
	if (Event == UIEvent_Command)															\
	{																						\
		bHandled = false;																	\
		lResult = 0;																		\
		if (UIGetParent())																	\
			lResult = UIGetParent()->UIFireEvent(UIEvent_Command, NULL, lParam, &bHandled);	\
		if (bHandled)																		\
			return true;																	\
	}

#define UI_EVENT_PASSTHROUGH(e)																\
	if (Event == e)																			\
	{																						\
		bHandled = false;																	\
		lResult = 0;																		\
		if (UIGetParent())																	\
			lResult = UIGetParent()->UIFireEvent(e, wParam, lParam, &bHandled);				\
		if (bHandled)																		\
			return true;																	\
	}

//	LRESULT OnUIEventHandler(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled)
#define UI_EVENT_HANDLER(event, func)									\
	if ((event) == Event)												\
	{																	\
		bHandled = true;												\
		lResult = func(Event, wParam, lParam, bHandled);				\
		if (bHandled)													\
			return true;												\
	}

//	LRESULT OnUIEventRangeHandler(int iEvent, WPARAM wParam, LPARAM lParam, bool& bHandled)
#define UI_EVENT_RANGE(eventBegin, eventEnd, func)						\
	if (Event >= (eventBegin) && Event <= (eventEnd))					\
	{																	\
		bHandled = true;												\
		lResult = func(Event, wParam, lParam, bHandled);				\
		if (bHandled)													\
			return true;												\
	}

#define FORWARD_UI_EVENT_MEMBER(event, member)							\
		if ((event) == Event)											\
		{																\
			if ((member).OnUIEvent(pEvent, lResult))					\
				return true;											\
			if (this == (CUIObject*)&member)							\
				DebugBreak();											\
		}

#define END_UI_EVENT_MAP()												\
__if_exists (bIsUIObject)												\
{																		\
		return __super::OnUIEvent(pEvent, lResult);						\
}																		\
__if_not_exists (bIsUIObject)											\
{																		\
		return false;													\
}																		\
	}

#define CHAIN_UI_EVENT_MAP(theClass)									\
	if (theClass::ProcessChainEvent(pEvent, lResult))					\
		return true;													\


#define BEGIN_ROOT_COMMAND_MAP() \
	virtual bool OnUICommand(LPCWSTR lpszWindowID, DWORD dwControlID, DWORD dwCommand, UINH* pHDR, LRESULT& lResult)\
	{\
		bool bHandled = false;\
		(lpszWindowID);\
		(dwControlID);\
		(dwCommand);\
		(pHDR);\
		(lResult);\
		(bHandled);\

// LRESULT OnRootCommandHandler(UINH* pHDR, bool& bHandled)
#define ROOT_COMMAND_HANDLER(WindowID, ControlID, CommandCode, func)\
	if (lpszWindowID && dwControldID == ControlID && dwCommand == CommandCode && _wcsicmp(WindowID, lpszWindowID) == 0) \
	{ \
		bHandled = true;\
		lResult = func(pHDR, bHandled);\
		if (bHandled)\
			return true;\
	}

// LRESULT OnRootCommandWindowIDHandler(DWORD dwControlID, DWORD dwCommandCode, UINH* pHDR, bool& bHandled)
#define ROOT_COMMAND_WINDOWID_HANDLER(WindowID, func)\
	if (lpszWindowID &&  _wcsicmp(WindowID, lpszWindowID) == 0) \
	{ \
		bHandled = true;\
		lResult = func(dwControlID, dwCommand, pHDR, bHandled);\
		if (bHandled)\
			return true;\
	}

// LRESULT OnRootCommandControlIDHandler(LPCWSTR lpszWindowID, DWORD dwCommandCode, UINH* pHDR, bool& bHanded)
#define ROOT_COMMAND_CONTROLID_HANDLER(ControlID, func)\
	if (ControlID == dwControlID)\
	{\
		bHandled = true;\
		lResult = func(lpszWindowID, dwCommand, pHDR, bHandled);\
		if (bHandled)\
			return true;\
	}

// LRESULT OnRootCommandIDHandler(LPCWSTR lpszWindowID, DWORD dwControlID, UINH* pHDR, bool& bHandled)
#define ROOT_COMMAND_COMMANDID_HANDLER(CommandID, func)\
	if (CommandID == dwCommand)\
	{\
		bHandled = true;\
		lResult = func(lpszWindowID, dwControlID, pHDR, bHandled);\
		if (bHandled)\
			return true;\
	}
	
// LRESULT OnRootCommandCodeHandler(LPCWSTR lpszWindowID, DWORD dwControlID, UINH* pHDR, bool& bHandled)
#define ROOT_COMMAND_COMMANDCODE_HANDLER(CommandCode, func)\
	if (CommandCode == dwCommand)\
	{\
		bHandled = true;\
		lResult = func(lpszWindowID, dwControlID, pHDR, bHandled);\
		if (bHandled)\
			return true;\
	}

// LRESULT OnRootControlCommandCodeHandler(DWORD dwControlID, DWORD dwCommandCode, UINH* pHDR, bool& bHandled)
#define ROOT_COMMAND_CONTROLID_COMMANDCODE_HANDLER(ControlID, CommandCode, func)\
	if (CommandCode == dwCommand && dwControlID == ControlID)\
	{\
		bHandled = true;\
		lResult = func(dwControlID, dwCommand, pHDR, bHandled);\
		if (bHandled)\
			return true;\
	}

#define END_ROOT_COMMAND_MAP()\
		return false;\
	}


// void OnUIModalInit()
#define UI_EVENT_MODALINIT(func)\
	if (UIEvent_ModalInit == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIModalUninit()
#define UI_EVENT_MODALUNINIT(func)\
	if (UIEvent_ModalUninit == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIPrePaint()
#define UI_EVENT_PREPAINT(func)\
	if (UIEvent_PrePaint == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIPaint(CDCHandle dc, DWORD dwStatus)
#define UI_EVENT_PAINT(func)\
	if (UIEvent_Paint == Event)\
	{\
		bHandled = true;\
		func((HDC)lParam, (DWORD)wParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIPaintBackground(CDCHandle dc, DWORD dwStatus)
#define UI_EVENT_PAINTBACKGROUND(func)\
	if (UIEvent_PaintBackgound == Event)\
	{\
		bHandled = true;\
		func((HDC)lParam, (DWORD)wParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIPaintTransition(CDCHandle dc, DWORD dwOldStatus, DWORD dwNewStatus, float fProgress)
#define UI_EVENT_PAINTTRANSITION(func)\
	if (UIEvent_PaintTransition == Event)\
	{\
		bHandled = true;\
		PaintTransitionWParam *pWParam = (PaintTransitionWParam *)wParam;\
		func((HDC)lParam, pWParam->dwOldStatus, pWParam->dwNewStatus, pWParam->fProgress);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUILButtonDown(UINT nFlags, POINT point)
#define UI_EVENT_LBUTTONDOWN(func)\
	if (UIEvent_LeftButtonDown == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUILButtonUp(UINT nFlags, POINT point)
#define UI_EVENT_LBUTTONUP(func)\
	if (UIEvent_LeftButtonUp == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUILButtonDblClk(UINT nFlags, POINT point)
#define UI_EVENT_LBUTTONDBLCLK(func)\
	if (UIEvent_LeftButtonDblClk == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIRButtonDown(UINT nFlags, POINT point)
#define UI_EVENT_RBUTTONDOWN(func)\
	if (UIEvent_RightButtonDown == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIRButtonUp(UINT nFlags, POINT point)
#define UI_EVENT_RBUTTONUP(func)\
	if (UIEvent_RightButtonUp == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIRButtonDblClk(UINT nFlags, POINT point)
#define UI_EVENT_RBUTTONDBLCLK(func)\
	if (UIEvent_RightButtonDblClk == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIMButtonDown(UINT nFlags, POINT point)
#define UI_EVENT_MBUTTONDOWN(func)\
	if (UIEvent_MiddleButtonDown == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIMButtonUp(UINT nFlags, POINT point)
#define UI_EVENT_MBUTTONUP(func)\
	if (UIEvent_MiddleButtonUp == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIMButtonDblClk(UINT nFlags, POINT point)
#define UI_EVENT_MBUTTONDBLCLK(func)\
	if (UIEvent_MiddleButtonDblClk == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIXButtonDown(UINT nFlags, POINT point)
#define UI_EVENT_XBUTTONDOWN(func)\
	if (UIEvent_XButtonDown == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIXButtonUp(UINT nFlags, POINT point)
#define UI_EVENT_XBUTTONUP(func)\
	if (UIEvent_XButtonUp == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIXButtonDblClk(UINT nFlags, POINT point)
#define UI_EVENT_XBUTTONDBLCLK(func)\
	if (UIEvent_XButtonDblClk == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// UINT_PTR OnUIHitTest(POINT point)
#define UI_EVENT_HITTEST(func)\
	if (UIEvent_HitTest == Event)\
	{\
		bHandled = true;\
		POINT point = { wParam, lParam };\
		lResult = (LRESULT)func(point);\
		if (bHandled)\
			return true;\
	}

// UINCHitTestCode OnUINCHitTest(POINT point)
#define UI_EVENT_NCHITTEST(func)\
	if (UIEvent_NCHitTest == Event)\
	{\
		bHandled = true;\
		POINT point = { wParam, lParam };\
		lResult = (LRESULT)func(point);\
		if (bHandled)\
			return true;\
	}

// void OnUIGetChildrenForPaint(std::vector<CUIObject*>* pResultVec, CRect* pRect)
#define UI_EVENT_GETCHILDRENFORPAINT(func)\
	if (UIEvent_GetChildrenForPaint == Event)\
	{\
		bHandled = true;\
		func((std::vector<CUIObject*>*)wParam, (CRect*)lParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// bool OnUIGetKidsZOrder(std::vector<CUIObject*>* pResultVec)
#define UI_EVENT_GETKIDSZORDER(func)\
	if (UIEvent_GetKidsZOrder == Event)\
	{\
		bHandled = true;\
		lResult = (LRESULT)func((std::vector<CUIObject*>*)wParam);\
		if (bHandled)\
			return true;\
	}

// void OnUIMouseMove(UINT nFlags, POINT point)
#define UI_EVENT_MOUSEMOVE(func)\
	if (UIEvent_MouseMove == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)wParam;\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		func(nFlags, point);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// bool OnUIMouseWheel(UINT nFlags, short zDelta, POINT pt)
#define UI_EVENT_MOUSEWHEEL(func)\
	if (UIEvent_MouseWheel == Event)\
	{\
		bHandled = true;\
		UINT nFlags = (UINT)LOWORD(wParam);\
		short zDelta = (short)HIWORD(wParam);\
		POINT point = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};\
		lResult = (LRESULT)func(nFlags, zDelta, point);\
		if (bHandled)\
			return true;\
	}

// void OnUIMouseEnter()
#define UI_EVENT_MOUSEENTER(func)\
	if (UIEvent_MouseEnter == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIMouseLeave()
#define UI_EVENT_MOUSELEAVE(func)\
	if (UIEvent_MouseLeave == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
#define UI_EVENT_KEYDOWN(func)\
	if (UIEvent_KeyDown == Event)\
	{\
		bHandled = true;\
		UINT nChar = (UINT)wParam;\
		UINT nRepCnt = (UINT)lParam & 0xFFFF;\
		UINT nFlags = (UINT)((lParam & 0xFFFF0000) >> 16);\
		func(nChar, nRepCnt, nFlags);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
#define UI_EVENT_KEYUP(func)\
	if (UIEvent_KeyUp == Event)\
	{\
		bHandled = true;\
		UINT nChar = (UINT)wParam;\
		UINT nRepCnt = (UINT)lParam & 0xFFFF;\
		UINT nFlags = (UINT)((lParam & 0xFFFF0000) >> 16);\
		func(nChar, nRepCnt, nFlags);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIChar(UINT nChar, UINT nRepCnt, UINT nFlags)
#define UI_EVENT_CHAR(func)\
	if (UIEvent_Char == Event)\
	{\
		bHandled = true;\
		UINT nChar = (UINT)wParam;\
		UINT nRepCnt = (UINT)lParam & 0xFFFF;\
		UINT nFlags = (UINT)((lParam & 0xFFFF0000) >> 16);\
		func(nChar, nRepCnt, nFlags);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}


// void OnUISysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
#define UI_EVENT_SYSKEYDOWN(func)\
	if (UIEvent_SysKeyDown == Event)\
	{\
		bHandled = true;\
		UINT nChar = (UINT)wParam;\
		UINT nRepCnt = (UINT)lParam & 0xFFFF;\
		UINT nFlags = (UINT)((lParam & 0xFFFF0000) >> 16);\
		func(nChar, nRepCnt, nFlags);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUISysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
#define UI_EVENT_SYSKEYUP(func)\
	if (UIEvent_SysKeyUp == Event)\
	{\
		bHandled = true;\
		UINT nChar = (UINT)wParam;\
		UINT nRepCnt = (UINT)lParam & 0xFFFF;\
		UINT nFlags = (UINT)((lParam & 0xFFFF0000) >> 16);\
		func(nChar, nRepCnt, nFlags);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUISysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
#define UI_EVENT_SYSCHAR(func)\
	if (UIEvent_SysChar == Event)\
	{\
		bHandled = true;\
		UINT nChar = (UINT)wParam;\
		UINT nRepCnt = (UINT)lParam & 0xFFFF;\
		UINT nFlags = (UINT)((lParam & 0xFFFF0000) >> 16);\
		func(nChar, nRepCnt, nFlags);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUISetFocus()
#define UI_EVENT_SETFOCUS(func)\
	if (UIEvent_SetFocus == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUISetText(const std::wstring& strText)
#define UI_EVENT_SETTEXT(func)\
	if (UIEvent_SetText == Event)\
	{\
		bHandled = true;\
		func(*(const std::wstring*)wParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUISetFont(HFONT hNewFont)
#define UI_EVENT_SETFONT(func)				\
	if (UIEvent_SetFont == Event)			\
	{										\
		bHandled = true;					\
		func((HFONT)wParam);				\
		lResult = 0;						\
		if (bHandled)						\
			return true;					\
	}

// HFONT OnUIGetFont()
#define UI_EVENT_GETFONT(func)					\
	if (UIEvent_GetFont == Event)				\
	{											\
		bHandled = true;						\
		lResult = (LRESULT)func();				\
		if (bHandled)							\
			return true;						\
	}

// int OnUIGetTextLength()
#define UI_EVENT_GETTEXTLENGTH(func)\
	if (UIEvent_GetTextLength == Event)\
	{\
		bHandled = true;\
		lResult = (LRESULT)func();\
		if (bHandled)\
			return true;\
	}

// void OnUIGetText(std::wstring& strTextOut)
#define UI_EVENT_GETTEXT(func)\
	if (UIEvent_GetText == Event)\
	{\
		bHandled = true;\
		func(*(std::wstring*)wParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// COLORREF OnUIGetTextColor()
#define UI_EVENT_GETTEXTCOLOR(func)				\
	if (UIEvent_GetTextColor == Event)			\
	{											\
		bHandled = true;						\
		lResult = (LRESULT)func();				\
		if (bHandled)							\
			return true;						\
	}

// COLORREF OnUIGetDisabledTextColor()
#define UI_EVENT_GETDISABLEDTEXTCOLOR(func)		\
	if (UIEvent_GetDisabledTextColor == Event)	\
	{											\
		bHandled = true;						\
		lResult = (LRESULT)func();				\
		if (bHandled)							\
			return true;						\
	}

// void OnUISetTextColor(COLORREF clrText)
#define UI_EVENT_SETTEXTCOLOR(func)				\
	if (UIEvent_SetTextColor == Event)			\
	{											\
		bHandled = true;						\
		func((COLORREF)wParam);					\
		lResult = 0;							\
		if (bHandled)							\
			return true;						\
	}

// void OnUISetTextColor(COLORREF clrText)
#define UI_EVENT_SETDISABLEDTEXTCOLOR(func)		\
	if (UIEvent_SetDisabledTextColor == Event)	\
	{											\
		bHandled = true;						\
		func((COLORREF)wParam);					\
		lResult = 0;							\
		if (bHandled)							\
			return true;						\
	}

// UIObjectType OnUIGetObjectType()
#define UI_EVENT_GETOBJECTTYPE(func)			\
	if (UIEvent_GetObjectType == Event)			\
	{											\
		bHandled = true;						\
		lResult = (LRESULT)func();				\
		if (bHandled)							\
			return true;						\
	}

// bool OnUISetCursor(UINCHitTestCode ht, int iMouseEvent)
#define UI_EVENT_SETCURSOR(func)\
	if (UIEvent_SetCursor == Event)\
	{\
		bHandled = (bool)func((UINCHitTestCode)LOWORD(lParam), (int)HIWORD(lParam));\
		lResult = bHandled;\
		if (bHandled)\
			return true;\
	}

// void OnUIKillFocus()
#define UI_EVENT_KILLFOCUS(func)\
	if (UIEvent_KillFocus == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUITimer(UINT_PTR nIDEvent, LPARAM lParam)
#define UI_EVENT_TIMER(func)\
	if (UIEvent_Timer == Event)\
	{\
		bHandled = true;\
		func((UINT_PTR)wParam, lParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIWindowChange()
#define UI_EVENT_WINDOWCHANGE(func)\
	if (UIEvent_WindowChange == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIHScroll(UIHScrollEvent eEvent, int iDelta)
#define UI_EVENT_HSCROLL(func)\
	if (UIEvent_HScroll == Event)\
	{\
		bHandled = true;\
		func((UIHScrollEvent)wParam, lParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIVScroll(UIVScrollEvent eEvent, int iDelta)
#define UI_EVENT_VSCROLL(func)\
	if (UIEvent_VScroll == Event)\
	{\
		bHandled = true;\
		func((UIVScrollEvent)wParam, lParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIScrollChanged(bool bHScroll, int iScrollPos)
#define UI_EVENT_SCROLLCHANGED(func)		\
	if (UIEvent_ScrollChange == Event)		\
	{										\
		bHandled = true;					\
		func(!!wParam, (int)lParam);		\
		lResult = 0;						\
		if (bHandled)						\
			return true;					\
	}

// void OnUIActive(bool bActive)
#define UI_EVENT_ACTIVE(func)\
	if (UIEvent_Active == Event)\
	{\
		bHandled = true;\
		func(wParam != 0);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIActiveApp(bool bActive, int iThreadID)
#define UI_EVENT_ACTIVEAPP(func)\
	if (UIEvent_ActiveApp == Event)\
	{\
		bHandled = true;\
		func((wParam ? true : false), (int)lParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIStyleChanged(DWORD dwOldStyle)
#define UI_EVENT_STYLECHANGE(func)			\
	if (UIEvent_StyleChange == Event)		\
	{										\
		bHandled = true;					\
		func((DWORD)lParam);				\
		lResult = 0;						\
		if (bHandled)						\
			return true;					\
	}

// void OnUIFocusEnter()
#define UI_EVENT_FOCUSENTER(func)			\
	if (UIEvent_FocusEnter == Event)		\
	{										\
		bHandled = true;					\
		func();								\
		lResult = 0;						\
		if (bHandled)						\
			return true;					\
	}

// void OnUIFocusLeave()
#define UI_EVENT_FOCUSLEAVE(func)			\
	if (UIEvent_FocusLeave == Event)		\
	{										\
		bHandled = true;					\
		func();								\
		lResult = 0;						\
		if (bHandled)						\
			return true;					\
	}

// void OnUISize()
#define UI_EVENT_SIZE(func)\
	if (UIEvent_Size == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUITextColorChange()
#define UI_EVENT_TEXTCOLORCHANGE(func)\
	if (UIEvent_TextColorChange == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIFontChange()
#define UI_EVENT_FONTCHANGE(func)\
	if (UIEvent_FontChange == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIDefaultFontChange()
#define UI_EVENT_DEFAULTFONTCHANGE(func)\
	if (UIEvent_DefaultFontChange == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIUITextChange()
#define UI_EVENT_UITEXTCHANGE(func)\
	if (UIEvent_UITextChange == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIEnableUI(bool bEnable)
#define UI_EVENT_ENABLEUI(func)\
	if (UIEvent_EnableUI == Event)\
	{\
		bHandled = true;\
		func(wParam != FALSE);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIShowUI(bool bShow)
#define UI_EVENT_SHOWUI(func)\
	if (UIEvent_ShowUI == Event)\
	{\
		bHandled = true;\
		func(wParam != FALSE);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUICaptureChanged(CUIObject* pCapture)
#define UI_EVENT_CAPTURECHANGE(func)\
	if (UIEvent_CaptureChange == Event)\
	{\
		bHandled = true;\
		func((CUIObject*)lParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIActionStopped(CDecorator* pAction)
#define UI_EVENT_ACTIONSTOPPED(func) \
	if (UIEvent_ActionStopped == Event)\
	{\
		bHandled = true;\
		func((CDecorator*)wParam);\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// LRESULT OnUICommand(CUIObject* pSender, DWORD dwControlID, DWORD dwCommandCode, UINH* pNotifyHeader)
#define UI_COMMAND_HANDLER(func)											\
	if (UIEvent_Command == Event)											\
	{																		\
		bHandled = true;													\
		UINH* p = (UINH*)lParam;											\
		lResult = func(p->pControl, p->pdwControlID, p->dwCode, p);			\
		if (bHandled)														\
			return true;													\
	}

// LRESULT OnUICommandCode(CUIObject* pSender, DWORD dwControlID, UINH* pNotifyHeader)
#define UI_COMMAND_CODE(code, func)											\
	if (UIEvent_Command == Event && (code) == ((UINH*)lParam)->dwCode)		\
	{																		\
		bHandled = true;													\
		UINH* p = (UINH*)lParam;											\
		lResult = func(p->pControl, p->pdwControlID, p);					\
		if (bHandled)														\
			return true;													\
	}

// LRESULT OnUICommandID(CUIObject* pSender, DWORD dwCommandCode, UINH* pNotifyHeader)
#define UI_COMMAND_ID(id, func)												\
	if (UIEvent_Command == Event && (id) == ((UINH*)lParam)->pdwControlID)	\
	{																		\
		bHandled = true;													\
		UINH* p = (UINH*)lParam;											\
		lResult = func(p->pControl, p->dwCode, p);							\
		if (bHandled)														\
			return true;													\
	}

// LRESULT OnUICommandIDCode(CUIObject* pSender, UINH* pNotifyHeader)
#define UI_COMMAND_ID_CODE(id, code, func)									\
	if (UIEvent_Command == Event && (code) == ((UINH*)lParam)->dwCode		\
		&& (id) == ((UINH*)lParam)->pdwControlID)							\
	{																		\
		bHandled = true;													\
		UINH* p = (UINH*)lParam;											\
		lResult = func(p->pControl, p);										\
		if (bHandled)														\
			return true;													\
	}

// LRESULT OnUIReflectCommand(DWORD dwCommandCode, UINH* pNotifyHeader)
#define UI_REFLECT_COMMAND_HANDLER(func)								\
	if (UIEvent_ReflectCommand == Event)								\
	{																	\
		bHandled = true;												\
		UINH* p = (UINH*)lParam;										\
		lResult = func(p->dwCode, p);									\
		if (bHandled)													\
			return true;												\
	}

// LRESULT OnUIReflectCommandCode(DWORD dwCommandCode, UINH* pNotifyHeader)
#define UI_REFLECT_COMMAND_CODE(code, func)										\
	if (UIEvent_ReflectCommand == Event && (code) == ((UINH*)lParam)->dwCode)	\
	{																			\
		bHandled = true;														\
		UINH* p = (UINH*)lParam;												\
		lResult = func(p->dwCode, p);											\
		if (bHandled)															\
			return true;														\
	}

// LRESULT OnUIReflectCommandCodeKey(DWORD dwCommandCode, UINH* pNotifyHeader)
#define UI_REFLECT_COMMAND_CODE_PARAM(code, param, func)																\
	if (UIEvent_ReflectCommand == Event && (code) == ((UINH*)lParam)->dwCode && (param) == ((UINH*)lParam)->dwParam)	\
	{																													\
		bHandled = true;																								\
		UINH* p = (UINH*)lParam;																						\
		lResult = func(p->dwCode, p);																					\
		if (bHandled)																									\
			return true;																								\
	}

// void OnUICreate()
#define UI_EVENT_CREATE(func)\
	if (UIEvent_Create == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// void OnUIDestroy()
#define UI_EVENT_DESTROY(func)\
	if (UIEvent_Destroy == Event)\
	{\
		bHandled = true;\
		func();\
		lResult = 0;\
		if (bHandled)\
			return true;\
	}

// bool OnUIMeasureClient(SIZE& sz)
#define UI_EVENT_MEASURECLIENT(func)\
	if (UIEvent_MeasureClient == Event)\
	{\
		bHandled = true;\
		lResult = (LRESULT)func(*(SIZE*)lParam);\
		if (bHandled)\
			return true;\
	}

// bool OnUIMeasureChild(CUIObject* pChild, SIZE& sz)
#define UI_EVENT_MEASURECHILD(func)\
	if (UIEvent_MeasureChild == Event)\
	{\
		bHandled = true;\
		lResult = (LRESULT)func((CUIObject*)wParam, *(SIZE*)lParam);\
		if (bHandled)\
			return true;\
	}

// int OnUIMeasureScrollbar(bool bHorz)
#define UI_EVENT_MEASURESCROLLBAR(func)\
	if (UIEvent_MeasureScrollbar == Event)\
	{\
		bHandled = true;\
		lResult = (int)func(lParam == TRUE);\
		if (bHandled)\
			return true;\
	}

// void OnUIGetTooltip(UIGetTooltip* pTooltip)
#define UI_EVENT_GETTOOLTIP(func)						\
	if (UIEvent_GetTooltipText == Event)				\
	{													\
		bHandled = true;								\
		func((UIGetTooltip*)wParam);					\
		lResult = S_OK;									\
		if (bHandled)									\
			return true;								\
	}

// void OnUIContextMenu(CUIObject* pSender, CPoint ptScreen)
#define UI_EVENT_CONTEXTMENU(func)									\
	if (UIEvent_ContextMenu == Event)								\
	{																\
		bHandled = true;											\
		CPoint ptScreen(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));\
		func((CUIObject*)wParam, ptScreen);							\
		lResult = S_OK;												\
		if (bHandled)												\
			return true;											\
	}
