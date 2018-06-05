// Author: yangqi
#pragma once
#include "../UIObject.h"

enum class UIScrollType
{
	// horizontal
	Left,
	Right,
	PageLeft,
	PageRight,
	LeftEdge,
	RightEdge,

	// vertical
	Up,
	Down,
	PageUp,
	PageDown,
	Top,
	Bottom,

	ToHere,

	//
	Unknown,
};

__interface IUIScrollbar
{
	void SetOwner(CUIObject *pOwner);
	int  GetSize() const;
	void SetSize(int nSize);
	void UpdateSize();
	void UpdateScrollbar();
	void Scroll(UIScrollType eType, int nReserved = 0);
	
	CUIObject* GetUIObject() const;
	bool IsHorizontal() const;
	bool IsOutter() const;
};

template <bool t_bHorz, class T>
class CUIScrollbarT : public IUIScrollbar, public T
{
public:
	explicit CUIScrollbarT(bool bAsOutterScrollbar, int nSize, int nThumbButtonMinSize)
		: m_pT(nullptr), m_nSize(nSize), m_nThumbButtonMinSize(nThumbButtonMinSize), m_pOwner(nullptr)
	{
		m_pT = static_cast<T*>(this);
		m_pT->UIAddOrRemoveStyle(!bAsOutterScrollbar, UIStyle_NonClient);
	}

	virtual ~CUIScrollbarT()
	{
		if (m_pT->UIIsCaptured() || m_pT->UIIsChild(m_pT->UIGetCapture()))
			m_pT->UIReleaseCapture();

		if (m_pT->UIIsHovered() || m_pT->UIIsHoveredChild())
			m_pT->UIGetMgr()->SetHover(NULL);
	}

	// IUIScrollbar
	void SetOwner(CUIObject *pOwner) override { m_pOwner = pOwner; }
	int GetSize() const override { return m_nSize; }
	void SetSize(int nSize) override { m_nSize = nSize; }
	void UpdateSize() override
	{
		if (m_pT->UIIsCreated() && m_pOwner && m_pOwner->UIIsCreated())
		{
			bool bProcessed = false;
			const int nSize = m_pOwner->UIFireEvent(UIEvent_MeasureScrollbar, 0, t_bHorz ? 1 : 0, &bProcessed);
			if (bProcessed)
			{
				m_nSize = nSize;
			}
		}
	}
	void UpdateScrollbar() override { TuoAssert(0 && "UpdateScrollbar not implemented!"); }

	// nReserved 只对 UIScrollType::ToHere 有效
	void Scroll(UIScrollType eType, int nReserved = 0) override
	{
		auto funScrollHere = [&]()
		{
			ScrollbarInfo sbi = GetOwner()->UIGetScrollInfo(t_bHorz);
			const int nNewPos = nReserved * (sbi.iPage + sbi.iMax - sbi.iMin) / sbi.iPage - sbi.iPage / 2;
			SetScrollPos(nNewPos);
		};

#define BIND_FUN(_type_, _fun_) { UIScrollType::_type_, std::bind(&CUIObject::_fun_, GetOwner()), }
		using ScrollFun = std::function < void(void) >;
		const std::map<UIScrollType, ScrollFun> funcMap =
		{
			{ UIScrollType::ToHere, funScrollHere, },
			BIND_FUN(Top, UIScrollTop),
			BIND_FUN(Bottom, UIScrollBottom),
			BIND_FUN(LeftEdge, UIScrollLeftEdge),
			BIND_FUN(RightEdge, UIScrollRightEdge),
			BIND_FUN(Up, UIScrollUp),
			BIND_FUN(Down, UIScrollDown),
			BIND_FUN(Left, UIScrollLeft),
			BIND_FUN(Right, UIScrollRight),
			BIND_FUN(PageUp, UIScrollPageUp),
			BIND_FUN(PageDown, UIScrollPageDown),
			BIND_FUN(PageLeft, UIScrollPageLeft),
			BIND_FUN(PageRight, UIScrollPageRight),
		};
#undef BIND_FUN
		auto item = funcMap.find(eType);
		TuoAssert(item != funcMap.end());
		if (item != funcMap.end())
		{
			ScrollFun fun = item->second;
			fun();
		}
	}
	CUIObject* GetUIObject() const override { return m_pT; }
	bool IsHorizontal() const override { return t_bHorz; }
	bool IsOutter() const override { return !m_pT->UITestStyle(UIStyle_NonClient); }
	// horizontal
	void LeftEdge()	{ Scroll(UIScrollType::LeftEdge); }
	void RightEdge() { Scroll(UIScrollType::RightEdge); }
	void ScrollLeft() { Scroll(UIScrollType::Left); }
	void ScrollRight() { Scroll(UIScrollType::Right); }
	void PageLeft()	{ Scroll(UIScrollType::PageLeft); }
	void PageRight() { Scroll(UIScrollType::PageRight); }

	// vertical
	void Top() { Scroll(UIScrollType::Top); }
	void Bottom() { Scroll(UIScrollType::Bottom); }
	void ScrollUp()	{ Scroll(UIScrollType::Up); }
	void ScrollDown() { Scroll(UIScrollType::Down); }
	void PageUp() { Scroll(UIScrollType::PageUp); }
	void PageDown()	{ Scroll(UIScrollType::PageDown); }

protected:
	CUIObject*	GetOwner() const { return m_pOwner;	}
	int GetThumbButtonMinSize() const { return m_nThumbButtonMinSize; }
	void SetScrollPos(int nNewPos)
	{
		ScrollbarInfo sbi = GetOwner()->UIGetScrollInfo(t_bHorz);
		if (nNewPos != sbi.iDestPos)
		{
			GetOwner()->UIFireEvent(t_bHorz ? UIEvent_HScroll : UIEvent_VScroll,
				t_bHorz ? HScroll_TrackThumb : VScroll_TrackThumb,
				nNewPos - sbi.iDestPos);
		}
	}

private:
	T*			m_pT;
	int			m_nSize;
	const int	m_nThumbButtonMinSize;
	CUIObject*	m_pOwner;
};

namespace Scrollbar
{
	inline bool IsVisible(IUIScrollbar* p)
	{
		CUIObject* pObj = p ? p->GetUIObject() : nullptr;
		return pObj && pObj->UIIsVisible();
	}
}
