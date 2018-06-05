// Author: chenjianjun
#pragma once


struct IPreTranslateMessageHook;
class CActionManager;
class CUIMessageLoop
{
public:

	CUIMessageLoop();
	~CUIMessageLoop();

	bool AddPreTranslateMsgHook(IPreTranslateMessageHook* pHook);
	bool RemovePreTranslateMsgHook(IPreTranslateMessageHook* pHook);

	int Loop();

private:

	bool _PreTranslateMessage(MSG* lpMsg);

	std::vector<IPreTranslateMessageHook*> m_vecPreTranslateMsg;
};

struct IModalCallback;
class CUIObject;
class CUIApp
{
public:

	CUIApp();
	~CUIApp();

	void AddModalUICallback(IModalCallback* p);
	void RemoveModalUICallback(IModalCallback* p);
	bool IsModalUICallbackValid(IModalCallback* p);

private:

	CRITICAL_SECTION m_cs;
	std::vector<IModalCallback*> m_vecValidModalUICallback;

};

extern CUIApp g_UIAppModule;
