// Author: ChenJianjun
#pragma once


class CPaintContext
{
public:

	explicit CPaintContext(CUIManager* p)
	{
		m_pUIMgr = p;
		m_pOldContext = m_pUIMgr->m_pCurrentPaintContext;
		m_pUIMgr->m_pCurrentPaintContext = this;
	}

	~CPaintContext()
	{
		m_pUIMgr->m_pCurrentPaintContext = m_pOldContext;
	}

private:

	CUIManager* m_pUIMgr;
	CPaintContext* m_pOldContext;
};
