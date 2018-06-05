// Author: yangqi
#pragma once
#include "../../../Universal.h"
#include "../../UICrack.h"
#include "../ResourceData.h"
#include "../UIObject.h"
#include "UIScrollbar.h"

//////////////////////////////////////////////////////////////////////////
// 这里的代码由以下代码整合而来
//   CUIDefaultVScrollUp / CUIDefaultVScrollDown / CUIDefaultVScrollThumbButton / CUIDefaultVScrollbar
//   CUIDefaultHScrollUp / CUIDefaultHScrollDown / CUIDefaultHScrollThumbButton / CUIDefaultHScrollbar
//////////////////////////////////////////////////////////////////////////
#define AUTO_DELETE_PAINTER(p, b) if ((b) && p) { delete (p); (p) = NULL; (b) = false; }

template <bool t_bHorz>
class CUIDefaultScrollbarT : public CUIScrollbarT < t_bHorz, CUIObject >
{
private:
	enum
	{
		TIMER_AUTOSCROLL = 0x20,
		TIMER_CHECK_LBUTTONUP = 0x21,
		CHECK_LBUTTONUP_ELAPSE = 50,
		AUTOSCROLL_DELAY_ELAPSE = 500,
		AUTOSCROLL_ELAPSE = 50,
	};

	enum class Component
	{
		left,
		h_thumb_button,
		right,
		top,
		v_thumb_button,
		bottom,
		h_thumb_gripper,
		v_thumb_gripper,
		h_track,
		v_track,
	};

	struct STATUS_INFO
	{
		const void* pData;
		int dwLen;
		CUIStatsImageFrameInfo imgStatusInfo;
	};

	static bool GetStatusInfo(Component eComponent, STATUS_INFO& info)
	{
#define DEFINE_STATUS_ITEM(_component_, _res_, _status_) { Component::##_component_, { g_c##_res_##Res, g_c##_res_##ResSize, { \
		_status_##_FRAME_COUNT, _status_##_NORMAL, _status_##_HOVER, _status_##_FOCUS, _status_##_PRESSED, _status_##_DISABLED } } }
		static const std::map<Component, STATUS_INFO> siMap = {
			DEFINE_STATUS_ITEM(left, HScrollLeft, HSCROLLLEFT),
			DEFINE_STATUS_ITEM(right, HScrollRight, HSCROLLRIGHT),
			DEFINE_STATUS_ITEM(top, VScrollUp, VSCROLLUP),
			DEFINE_STATUS_ITEM(bottom, VScrollDown, VSCROLLDOWN),
			DEFINE_STATUS_ITEM(h_thumb_button, HScrollThumb, HSCROLLTHUMB),
			DEFINE_STATUS_ITEM(h_thumb_gripper, HScrollThumbGripper, HSCROLLTHUMBGRIPPER),
			DEFINE_STATUS_ITEM(v_thumb_button, VScrollThumb, VSCROLLTHUMB),
			DEFINE_STATUS_ITEM(v_thumb_gripper, VScrollThumbGripper, VSCROLLTHUMBGRIPPER),
			DEFINE_STATUS_ITEM(h_track, HScrollTrack, HSCROLLTRACK),
			DEFINE_STATUS_ITEM(v_track, VScrollTrack, VSCROLLTRACK),
		};
#undef DEFINE_STATUS_ITEM
		auto it = siMap.find(eComponent);
		TuoAssert(it != siMap.end() && "invalid scroll type.");
		const bool bFound = it != siMap.end();
		if (bFound)
			info = it->second;
		return bFound;
	}

public:
	explicit CUIDefaultScrollbarT(bool bAsOutterScrollbar = false, int nSize = GetSystemMetrics(SM_CYVSCROLL),
		int nThumbButtonMinSize = UI_SCALE(5)) : CUIScrollbarT < t_bHorz, CUIObject >(bAsOutterScrollbar, nSize, nThumbButtonMinSize),
		m_pComponent1(new CScrollbarComponentT < t_bHorz, t_bHorz ? Component::left : Component::top >(this)),
		m_pComponent2(new CScrollbarComponentT < t_bHorz, t_bHorz ? Component::right : Component::bottom >(this)),
		m_pThumbButton(new CUIDefaultScrollThumbButtonT< t_bHorz, t_bHorz ? Component::h_thumb_button : Component::v_thumb_button >(this))
	{
		UIModifyStyle(UIStyle_NoFocus, 0);
	}
	~CUIDefaultScrollbarT()
	{
		if (UIIsCreated())
		{
			if (UIIsCaptured() || UIIsChild(UIGetCapture()))
				UIReleaseCapture();
			if (UIIsHovered() || UIIsHoveredChild())
				UIGetMgr()->SetHover(nullptr);
		}

		if (m_pPainter)
		{
			AUTO_DELETE_PAINTER(m_pPainter->pTriangle1, m_pPainter->bTriangle1Managed);
			AUTO_DELETE_PAINTER(m_pPainter->pTriangle2, m_pPainter->bTriangle2Managed);
			AUTO_DELETE_PAINTER(m_pPainter->pNormalBk, m_pPainter->bNormalBkManaged);
			AUTO_DELETE_PAINTER(m_pPainter->pTrackBk, m_pPainter->bTrackBkManaged);
			AUTO_DELETE_PAINTER(m_pPainter->pThumb, m_pPainter->bThumbManaged);
		}
		
	}

	BEGIN_UI_EVENT_MAP(CUIDefaultScrollbarT)
		UI_REDRAW_ON_OWN_MOUSE_OR_FOCUS()
		UI_EVENT_CREATE(OnCreate)
		UI_EVENT_PAINT(OnPaint)
		UI_EVENT_SIZE(OnSize)
		UI_EVENT_STYLECHANGE(OnStyleChanged)
		UI_EVENT_LBUTTONDOWN(OnLButtonDown)
		UI_EVENT_LBUTTONUP(OnLButtonUp)
		UI_EVENT_LBUTTONDBLCLK(OnLButtonDblClick)
		UI_EVENT_CONTEXTMENU(OnUIContextMenu)
		UI_EVENT_TIMER(OnTimer)
	END_UI_EVENT_MAP()

	void SetScrollPainter(ScrollbarPainterInfo* pPainterInfo)
	{
		if (m_pPainter == NULL)
		{
			m_pPainter = new ScrollbarPainterInfo;
			ZeroMemory(m_pPainter, sizeof(ScrollbarPainterInfo));
		}
		else
		{
			AUTO_DELETE_PAINTER(m_pPainter->pTriangle1, m_pPainter->bTriangle1Managed);
			AUTO_DELETE_PAINTER(m_pPainter->pTriangle2, m_pPainter->bTriangle2Managed);
			AUTO_DELETE_PAINTER(m_pPainter->pNormalBk, m_pPainter->bNormalBkManaged);
			AUTO_DELETE_PAINTER(m_pPainter->pTrackBk, m_pPainter->bTrackBkManaged);
			AUTO_DELETE_PAINTER(m_pPainter->pThumb, m_pPainter->bThumbManaged);
		}

		if (pPainterInfo)
		{
			*m_pPainter = *pPainterInfo;
		}
		else
		{
			delete m_pPainter;
			m_pPainter = nullptr;
		}
	}
#undef AUTO_DELETE_PAINTER

	void UpdateScrollbar() override
	{
		if (GetOwner() == nullptr)
			return;

		ScrollbarInfo sbi = GetOwner()->UIGetScrollInfo(t_bHorz);
		int iWidth = 0;
		const CRect rc = UIGetClientRect();
		CRect rc1st = t_bHorz ? CRect(0, 0, 0, GetSize()) : CRect(0, 0, GetSize(), 0);
		CRect rc2nd = t_bHorz ? CRect(rc.right, 0, rc.right, GetSize()) : CRect(0, rc.bottom, GetSize(), rc.bottom);

		if (!UITestStyle(UIScrollStyle_NoTriangle))
		{
			iWidth = GetSize() + 1;
			if (t_bHorz)
			{
				rc1st.right = iWidth;
				rc2nd.left = rc.right - iWidth;
			}
			else
			{
				rc1st.bottom = iWidth;
				rc2nd.top = rc.bottom - iWidth;
			}
			m_pComponent1->UISetPos(rc1st);
			m_pComponent2->UISetPos(rc2nd);
		}

		if (sbi.iMax > 0)
		{
			m_pComponent1->UIDisable(false);
			m_pComponent2->UIDisable(false);
			m_pThumbButton->UIDisable(false);
			m_pThumbButton->UIModifyStyle(UIStyle_Visible, 0);
			int iLen = (t_bHorz ? rc.Width() : rc.Height()) - iWidth * 2;
			float fscale = (float)1.0 * iLen / (sbi.iMax - sbi.iMin + sbi.iPage);
			int iThumb = (int)(sbi.iPage * fscale);
			const int iThumbButtonMinWidth = GetThumbButtonMinSize();
			if (iThumb < iThumbButtonMinWidth)
			{
				fscale = (float)1.0 * (iLen - (iThumbButtonMinWidth - iThumb)) / (sbi.iMax - sbi.iMin + sbi.iPage);
				iThumb = iThumbButtonMinWidth;
			}

			int iOffset = 0;
			int iPos = std::min(std::max(sbi.iMin, sbi.iPos), sbi.iMax);
			if (iPos == sbi.iMin)
				iOffset = 0;
			else if (iPos == sbi.iMax)
				iOffset = t_bHorz ? (rc2nd.left - rc1st.right - iThumb) : (rc2nd.top - rc1st.bottom - iThumb);
			else
				iOffset = (int)(iPos * fscale);

			const CRect rcNewThumb = t_bHorz ? CRect(iWidth + iOffset, 0, iWidth + iOffset + iThumb, GetSize()) :
											CRect(0, iWidth + iOffset, GetSize(), iWidth + iOffset + iThumb);
			if (rcNewThumb != m_pThumbButton->UIGetRect())
				m_pThumbButton->UISetPos(rcNewThumb);
		}
		else
		{
			m_pComponent1->UIDisable(true);
			m_pComponent2->UIDisable(true);
			m_pThumbButton->UIDisable(true);
			m_pThumbButton->UIModifyStyle(0, UIStyle_Visible);

			m_pThumbButton->UISetPos(-10, -10, 0, 0);
		}

		UIRedraw();
	}

	void OnSize() { UpdateScrollbar(); }

	void OnStyleChanged(DWORD dwOldStyle) { UIRedraw(); }

	void OnCreate()
	{
		if (!UITestStyle(UIScrollStyle_NoTriangle))
		{
			m_pComponent1->UICreate(this, UIStyle_NonClient & UIGetStyle());
			m_pComponent2->UICreate(this, UIStyle_NonClient & UIGetStyle());
		}
		m_pThumbButton->UICreate(this, UIStyle_NonClient & UIGetStyle());
	}

	void OnPaint(CDCHandle dc, DWORD dwStatus)
	{
		TuoAssert(UIGetPainter() == NULL && "滚动条不能设置自己的Painter, 需要通过滚动条所属窗口的setScrollPainterInfo来设置.");

		CRect rc = UIGetClientRect();
		ScrollbarPainterInfo* pPainterInfo = m_pPainter;

		if (pPainterInfo && pPainterInfo->pNormalBk)
		{
			pPainterInfo->pNormalBk->DoPaint(this, dc, rc, UIGetStatus() & ~UIStatus_Pressed);
		}
		else
		{
			STATUS_INFO info;
			if (GetStatusInfo(t_bHorz ? Component::h_track : Component::v_track, info))
			{
				CBitmap bmp = Universal::UI::Bitmap::LoadFromBuffer(info.pData, info.dwLen);
				Universal::UI::Paint::DrawImage(dc, rc, bmp, info.imgStatusInfo.ToFraw9Frame(0), t_bHorz ? DRAWOPT_NO_WIDTH_CHECK() : DRAWOPT_NO_HEIGHT_CHECK());
			}
		}

		ScrollbarInfo sbi = GetOwner()->UIGetScrollInfo(t_bHorz);
		if (!m_bLowerTrack && !m_bUpperTrack || sbi.iMax == 0)
			return;

		CRect rcTrack;
		CRect rcThumb = m_pThumbButton->UIGetRect();
		if (m_bLowerTrack)
		{
			if (!UITestStyle(UIScrollStyle_NoTriangle))
			{
				const CRect rc1st = m_pComponent1->UIGetRect();
				rcTrack = t_bHorz ? CRect(rc1st.right, rc.top, rcThumb.left, rc.bottom) : CRect(0, rc1st.bottom, rc.right, rcThumb.top);
			}
			else
			{
				rcTrack = t_bHorz ? CRect(0, rc.top, rcThumb.left, rc.bottom) : CRect(0, 0, rc.right, rcThumb.top);
			}
		}
		else
		{
			if (!UITestStyle(UIScrollStyle_NoTriangle))
			{
				const CRect rc2nd = m_pComponent2->UIGetRect();
				rcTrack = t_bHorz ? CRect(rcThumb.right, rc.top, rc2nd.left, rc.bottom) : CRect(0, rcThumb.bottom, rc.right, rc2nd.top);
			}
			else
			{
				rcTrack = t_bHorz ? CRect(rcThumb.right, rc.top, rc.right, rc.bottom) : CRect(0, rcThumb.bottom, rc.right, rc.bottom);
			}
		}

		if (pPainterInfo && pPainterInfo->pTrackBk)
		{
			pPainterInfo->pTrackBk->DoPaint(this, dc, rcTrack, UIStatus_Pressed);
		}
		else
		{
			STATUS_INFO info;
			if (GetStatusInfo(t_bHorz ? Component::h_track : Component::v_track, info) && !rcTrack.IsRectEmpty())
			{
				CBitmap bmp = Universal::UI::Bitmap::LoadFromBuffer(info.pData, info.dwLen);
				Universal::UI::Paint::DrawImage(dc, rcTrack, bmp, info.imgStatusInfo.ToFraw9Frame(UIStatus_Pressed),
					t_bHorz ? DRAWOPT_NO_WIDTH_CHECK() : DRAWOPT_NO_HEIGHT_CHECK());
			}
		}
	}

	void OnLButtonDown(UINT nFlags, CPoint point)
	{
		LButtonDownClick(nFlags, point);
	}

	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		if (m_bLButtonDown)
		{
			m_bLButtonDown = false;
			UIKillTimer(TIMER_CHECK_LBUTTONUP);
			UIKillTimer(TIMER_AUTOSCROLL);
			m_bUpperTrack = false;
			m_bLowerTrack = false;
			UIReleaseCapture();
			UIRedraw();
		}
	}

	void OnLButtonDblClick(UINT nFlags, CPoint point)
	{
		LButtonDownClick(nFlags, point);
	}

	void OnUIContextMenu(CUIObject* pSender, CPoint ptScreen)
	{
		if (ptScreen.x == -1 || ptScreen.y == -1)
			return;

		CMenu menu;
		menu.CreatePopupMenu();
		const CONTEXT_MENU_INFO_PTR& pInfo = GetContextMenuInfo();
		for (size_t i = 0; i < pInfo->size(); ++i)
		{
			const ContextMenuItem& info = pInfo->at(i);
			menu.InsertMenu(i, info.nFlag, info.nID, info.szText);
		}

		const DWORD dwRet = ::TrackPopupMenuEx(menu, TPM_RETURNCMD | TPM_NONOTIFY, ptScreen.x, ptScreen.y, UIGetHWND(), NULL);
		ProcessContextMenuResult(dwRet, ptScreen);
	}

	void ProcessContextMenuResult(DWORD dwRet, const CPoint& ptScreen)
	{
		if ((dwRet & SCROLL_TYPE_MASK) != 0)
		{
			const UIScrollType eType = UIScrollType(dwRet & ~SCROLL_TYPE_MASK);
			if (eType == UIScrollType::ToHere)
			{
				const CPoint pt = UIScreenToClient(ptScreen);
				Scroll(eType, t_bHorz ? pt.x : pt.y);
			}
			else
			{
				Scroll(eType);
			}
		}
	}

	void OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
	{
		if (nIDEvent == TIMER_CHECK_LBUTTONUP)
		{
			if (!UITestKeyDown(VK_LBUTTON))
			{
				OnLButtonUp(0, CPoint(0, 0));
				UIKillTimer(nIDEvent);
			}
		}
		else if (nIDEvent == TIMER_AUTOSCROLL)
		{
			CPoint pt;
			::GetCursorPos(&pt);
			pt = UIScreenToClient(pt);

			const CRect& rcThumb = m_pThumbButton->UIGetRect();
			const int nPos = t_bHorz ? pt.x : pt.y;
			const int nStart = t_bHorz ? rcThumb.left : rcThumb.top;
			const int nEnd = t_bHorz ? rcThumb.right : rcThumb.bottom;
			UIScrollType eScrollType = UIScrollType::Unknown;

			if (m_bLowerTrack)
			{
				if (UIGetClientRect().PtInRect(pt) && nStart > nPos)
					eScrollType = t_bHorz ? UIScrollType::PageLeft : UIScrollType::PageUp;
			}
			else
			{
				if (UIGetClientRect().PtInRect(pt) && nEnd < nPos)
					eScrollType = t_bHorz ? UIScrollType::PageRight : UIScrollType::PageDown;
			}
			if (eScrollType == UIScrollType::Unknown)
				Scroll(eScrollType);
			UISetTimer(TIMER_AUTOSCROLL, AUTOSCROLL_ELAPSE);
		}
	}

private:
	void LButtonDownClick(const UINT& nFlags, const CPoint& point)
	{
		UISetCapture();
		m_bLButtonDown = true;
		UISetTimer(TIMER_CHECK_LBUTTONUP, CHECK_LBUTTONUP_ELAPSE);
		UISetTimer(TIMER_AUTOSCROLL, AUTOSCROLL_DELAY_ELAPSE);

		UIScrollType eScroll = UIScrollType::Unknown;
		CRect rcThumb = m_pThumbButton->UIGetRect();
		if ((t_bHorz ? point.x : point.y) < (t_bHorz ? rcThumb.left : rcThumb.top))
		{
			m_bLowerTrack = true;
			eScroll = t_bHorz ? UIScrollType::PageLeft : UIScrollType::PageUp;
		}
		else
		{
			m_bUpperTrack = true;
			eScroll = t_bHorz ? UIScrollType::PageRight : UIScrollType::PageDown;
		}
		Scroll(eScroll);
	}

protected:
	enum
	{
		CONTEXT_MENU_ITEM_COUNT = 10,
		SCROLL_TYPE_MASK = 0x1000,
	};

	struct ContextMenuItem
	{
		const UINT nFlag;
		const UINT nID;
		wchar_t szText[8];

		// separator
		ContextMenuItem() : nFlag(MF_BYPOSITION | MF_SEPARATOR), nID(0) { szText[0] = 0; }

		// normal item
		ContextMenuItem(UIScrollType eType, const wchar_t* lpszText) : nFlag(MF_BYPOSITION | MFT_STRING), nID((DWORD)eType | SCROLL_TYPE_MASK)
		{
			wcscpy_s(szText, _countof(szText), lpszText);
		}
	};

	using CONTEXT_MENU_INFO_PTR = std::unique_ptr < std::array< ContextMenuItem, CONTEXT_MENU_ITEM_COUNT > >;
	const CONTEXT_MENU_INFO_PTR& GetContextMenuInfo()
	{
		if (!m_pContextMenuInfo)
		{
			const std::array<ContextMenuItem, CONTEXT_MENU_ITEM_COUNT> info =
			{
				ContextMenuItem(UIScrollType::ToHere, L"滚动至此"),
				ContextMenuItem(),
				ContextMenuItem(t_bHorz ? UIScrollType::LeftEdge : UIScrollType::Top, t_bHorz ? L"左边缘" : L"顶部"),
				ContextMenuItem(t_bHorz ? UIScrollType::RightEdge : UIScrollType::Bottom, t_bHorz ? L"右边缘" : L"底部"),
				ContextMenuItem(),
				ContextMenuItem(t_bHorz ? UIScrollType::PageLeft : UIScrollType::PageUp, t_bHorz ? L"向左翻页" : L"向上翻页"),
				ContextMenuItem(t_bHorz ? UIScrollType::PageRight : UIScrollType::PageDown, t_bHorz ? L"向右翻页" : L"向下翻页"),
				ContextMenuItem(),
				ContextMenuItem(t_bHorz ? UIScrollType::Left : UIScrollType::Up, t_bHorz ? L"向左滚动" : L"向上滚动"),
				ContextMenuItem(t_bHorz ? UIScrollType::Right : UIScrollType::Down, t_bHorz ? L"向右滚动" : L"向下滚动"),
			};
			m_pContextMenuInfo.reset(new std::array<ContextMenuItem, CONTEXT_MENU_ITEM_COUNT>(info));
		}
		return m_pContextMenuInfo;
	}

private:
	int UpdateFromThumbPos(CRect& rcThumb)
	{
		const ScrollbarInfo& sbi = GetOwner()->UIGetScrollInfo(t_bHorz);
		int iNewPos = 0;
		if (!UITestStyle(UIScrollStyle_NoTriangle))
		{
			const CRect rc1st = m_pComponent1->UIGetRect();
			const CRect rc2nd = m_pComponent2->UIGetRect();
			if (t_bHorz)
				iNewPos = (int)((rcThumb.left - rc1st.right) * 1.0 / (rc2nd.left - rc1st.right - rcThumb.Width()) * (sbi.iMax - sbi.iMin)) + sbi.iMin;
			else
				iNewPos = (int)((rcThumb.top - rc1st.bottom) * 1.0 / (rc2nd.top - rc1st.bottom - rcThumb.Height()) * (sbi.iMax - sbi.iMin)) + sbi.iMin;
		}
		else
		{
			const CRect rc = UIGetClientRect();
			if (t_bHorz)
				iNewPos = (int)((rcThumb.left - rc.left) * 1.0 / (rc.right - rc.left - rcThumb.Width()) * (sbi.iMax - sbi.iMin)) + sbi.iMin;
			else
				iNewPos = (int)((rcThumb.top - rc.top) * 1.0 / (rc.bottom - rc.top - rcThumb.Height()) * (sbi.iMax - sbi.iMin)) + sbi.iMin;
		}

		return std::min(std::max(iNewPos, sbi.iMin), sbi.iMax);
	}

	void MoveThumbButton(int iThumbPos)
	{
		CRect rcThumb = m_pThumbButton->UIGetRect();

		if (t_bHorz)
			rcThumb.OffsetRect(iThumbPos - rcThumb.left, 0);
		else
			rcThumb.OffsetRect(0, iThumbPos - rcThumb.top);

		SetScrollPos(UpdateFromThumbPos(rcThumb));
	}

private:
	ScrollbarPainterInfo* m_pPainter = nullptr;
	std::unique_ptr<CUIObject> m_pComponent1;
	std::unique_ptr<CUIObject> m_pComponent2;
	std::unique_ptr<CUIObject> m_pThumbButton;
	bool m_bLButtonDown = false;
	bool m_bLowerTrack = false;
	bool m_bUpperTrack = false;
	CONTEXT_MENU_INFO_PTR m_pContextMenuInfo;

private:
	template<bool t_bHorz, Component t_component>
	class CScrollbarComponentT : public CUIObject
	{
	public:
		explicit CScrollbarComponentT(CUIDefaultScrollbarT<t_bHorz>* pScrollbar) : m_pScrollbar(pScrollbar)
		{
			switch (t_component)
			{
			case Component::left:
				m_eScrollType = UIScrollType::Left;
				break;
			case Component::right:
				m_eScrollType = UIScrollType::Right;
				break;
			case Component::top:
				m_eScrollType = UIScrollType::Up;
				break;
			case Component::bottom:
				m_eScrollType = UIScrollType::Down;
				break;
			default:
				break;
			}
			UIModifyStyle(UIStyle_NoFocus, 0);
		}

		~CScrollbarComponentT()
		{
		}

		BEGIN_UI_EVENT_MAP(CScrollbarComponentT)
			UI_EVENT_PAINT(OnPaint)
			UI_EVENT_STYLECHANGE(OnStyleChanged)
			UI_EVENT_CAPTURECHANGE(OnCaptureChanged)
			UI_EVENT_LBUTTONDOWN(OnLButtonDown)
			UI_EVENT_LBUTTONUP(OnLButtonUp)
			UI_EVENT_LBUTTONDBLCLK(OnLButtonDblClick)
			UI_EVENT_TIMER(OnTimer)
		END_UI_EVENT_MAP()

		void OnStyleChanged(DWORD dwOldStyle)
		{
			if (UIGetParent())
				UIGetParent()->UIRedraw();
		}

		void OnPaint(CDCHandle dc, DWORD dwStatus)
		{
			TuoAssert(UIGetPainter() == NULL && "滚动条不能设置自己的Painter, 需要通过滚动条所属窗口的setScrollPainterInfo来设置.");

			CRect rc(0, 0, m_pScrollbar->GetSize(), m_pScrollbar->GetSize());

			DWORD dwState = 0;
			if (dwStatus & UIStatus_Disabled)
			{
				dwState = UIStatus_Disabled;
			}
			else if (dwStatus & UIStatus_Pressed)
			{
				dwState = UIStatus_Pressed;
			}
			else if (dwState & UIStatus_Hover)
			{
				dwState = UIStatus_Focus;
			}
			else if (UIGetParent()->UIIsHoveredChild() || UIGetParent()->UIIsHovered())
			{
				dwState = UIStatus_Hover;
			}

			OnPaint(dc, rc, dwState);
		}

		void OnCaptureChanged(CUIObject* pCapture)
		{
			UIKillTimer(TIMER_AUTOSCROLL);
		}

		void OnLButtonDown(UINT nFlags, CPoint point)
		{
			LButtonDownClick(nFlags, point);
		}

		void OnLButtonDblClick(UINT nFlags, CPoint point)
		{
			LButtonDownClick(nFlags, point);
		}

		void OnLButtonUp(UINT nFlags, CPoint point)
		{
			UIReleaseCapture();
			UIKillTimer(TIMER_AUTOSCROLL);
		}

		void OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
		{
			if (nIDEvent == TIMER_AUTOSCROLL)
			{
				m_pScrollbar->Scroll(m_eScrollType);
				UIKillTimer(TIMER_AUTOSCROLL);
				UISetTimer(TIMER_AUTOSCROLL, AUTOSCROLL_ELAPSE);
			}
		}

	protected:
		void OnPaint(CDCHandle dc, const CRect& rc, DWORD dwState)
		{
			TuoAssert(UIGetPainter() == NULL && "滚动条不能设置自己的Painter, 需要通过滚动条所属窗口的setScrollPainterInfo来设置.");
			IPainter* pPainter = nullptr;
			ScrollbarPainterInfo* pPainterInfo = m_pScrollbar->m_pPainter;
			if (pPainterInfo)
			{
				switch (t_component)
				{
				case Component::left:
				case Component::top:
					pPainter = pPainterInfo->pTriangle1;
					break;
				case Component::right:
				case Component::bottom:
					pPainter = pPainterInfo->pTriangle2;
					break;
				case Component::h_thumb_button:
				case Component::v_thumb_button:
					pPainter = pPainterInfo->pThumb;
					break;
				default:
					TuoAssert(0);
					break;
				}
			}
			if (!DrawByPainter(pPainter, this, dc, rc, dwState))
				OnDefaultPaint(dc, rc, dwState);
		}

		bool DrawByPainter(IPainter* pPainter, CUIObject* pUI, CDCHandle dc, const CRect& rc, DWORD dwState)
		{
			if (pPainter)
				pPainter->DoPaint(pUI, dc, rc, dwState);
			return pPainter != nullptr;
		}

		virtual void OnDefaultPaint(CDCHandle dc, const CRect& rc, DWORD dwState)
		{
			STATUS_INFO info;
			if (GetStatusInfo(t_component, info))
			{
				CBitmap bmp = Universal::UI::Bitmap::LoadFromBuffer(info.pData, info.dwLen);
				Universal::UI::Paint::DrawImage(dc, rc, bmp, info.imgStatusInfo.ToFraw9Frame(dwState), DrawOption_Opaque);
			}
		}

	private:
		void LButtonDownClick(const UINT& nFlags, const CPoint& point)
		{
			UISetCapture();
			m_pScrollbar->Scroll(m_eScrollType);
			UISetTimer(TIMER_AUTOSCROLL, AUTOSCROLL_DELAY_ELAPSE);
		}

	protected:
		CUIDefaultScrollbarT<t_bHorz>* m_pScrollbar = nullptr;
		UIScrollType m_eScrollType = UIScrollType::Unknown;
	};

	template<bool t_bHorz, Component t_component>
	class CUIDefaultScrollThumbButtonT : public CScrollbarComponentT < t_bHorz, t_component >
	{
	public:
		explicit CUIDefaultScrollThumbButtonT(CUIDefaultScrollbarT<t_bHorz>* pScrollbar) :
			CScrollbarComponentT<t_bHorz, t_component>(pScrollbar),
			m_bLButtonDown(false), m_bMoved(false)
		{
			UIModifyStyle(UIStyle_NoFocus, 0);
		}
		~CUIDefaultScrollThumbButtonT()
		{
		}

		BEGIN_UI_EVENT_MAP(CUIDefaultScrollThumbButtonT)
			UI_EVENT_PAINT(OnPaint)
			UI_EVENT_STYLECHANGE(OnStyleChanged)
			UI_EVENT_CAPTURECHANGE(OnCaptureChanged)
			UI_EVENT_LBUTTONDOWN(OnLButtonDown)
			UI_EVENT_LBUTTONUP(OnLButtonUp)
			UI_EVENT_LBUTTONDBLCLK(OnLButtonDown)
			UI_EVENT_MOUSEMOVE(OnMouseMove)
		END_UI_EVENT_MAP()

		void OnPaint(CDCHandle dc, DWORD dwStatus)
		{
			TuoAssert(UIGetPainter() == NULL && "滚动条不能设置自己的Painter, 需要通过滚动条所属窗口的setScrollPainterInfo来设置.");
			CScrollbarComponentT<t_bHorz, t_component>::OnPaint(dc, UIGetClientRect(), UIGetStatus());
		}

		void OnStyleChanged(DWORD dwOldStyle)
		{
			if (UIGetParent())
				UIGetParent()->UIRedraw();
		}

		void OnCaptureChanged(CUIObject* pCapture)
		{
			m_bLButtonDown = false;
		}

		void OnLButtonDown(UINT nFlags, CPoint point)
		{
			m_ptDown = point;
			m_bLButtonDown = true;
			m_bMoved = false;
			UISetCapture();
		}

		void OnLButtonUp(UINT nFlags, CPoint point)
		{
			if (m_bLButtonDown)
			{
				m_bLButtonDown = false;
				UIReleaseCapture();
			}
		}

		void OnMouseMove(UINT nFlags, CPoint point)
		{
			if (m_bLButtonDown && (m_bMoved || m_ptDown != point))
			{
				m_bMoved = true;
				const CRect rc = m_pScrollbar->UIGetClientRect();
				const CRect rc1st = m_pScrollbar->m_pComponent1->UIGetRect();
				const CRect rc2nd = m_pScrollbar->m_pComponent2->UIGetRect();
				const CRect rcThumb = UIGetRect();

				int iThumbPos = t_bHorz ? (rcThumb.left + (point.x - m_ptDown.x)) : (rcThumb.top + (point.y - m_ptDown.y));

				if ((UIGetParent()->UIGetStyle() & UIScrollStyle_NoTriangle) == 0)
				{
					iThumbPos = std::max<int>(iThumbPos, t_bHorz ? rc1st.right : rc1st.bottom);
					iThumbPos = std::min<int>(iThumbPos, t_bHorz ? rc2nd.left - rcThumb.Width() : rc2nd.top - rcThumb.Height());
				}
				else
				{
					iThumbPos = std::max<int>(iThumbPos, t_bHorz ? rc.left : rc.top);
					iThumbPos = std::min<int>(iThumbPos, t_bHorz ? (rc.right - rcThumb.Width()) : (rc.bottom - rcThumb.Height()));
				}

				m_pScrollbar->MoveThumbButton(iThumbPos);
			}
		}

	private:
		void OnDefaultPaint(CDCHandle dc, const CRect& rc, DWORD dwState) override
		{
			STATUS_INFO info;
			if (!GetStatusInfo(t_component, info))
				return;

			CBitmap bmp = Universal::UI::Bitmap::LoadFromBuffer(info.pData, info.dwLen);

			std::unique_ptr<Draw9> pDraw9(t_bHorz ? (Draw9*)(new Draw9H(3, 3)) : (Draw9*)(new Draw9V(3, 3)));
			Universal::UI::Paint::DrawImage9Grid(dc, rc, bmp, info.imgStatusInfo.ToFraw9Frame(dwState), *pDraw9, DrawOption_Opaque);

			const int nSize = t_bHorz ? rc.Width() : rc.Height();
			const Component eGripper = t_component == Component::h_thumb_button ? Component::h_thumb_gripper : Component::v_thumb_gripper;
			if (nSize > UI_SCALE(30) && GetStatusInfo(eGripper, info))
			{
				CBitmap bmp = Universal::UI::Bitmap::LoadFromBuffer(info.pData, info.dwLen);
				CSize size;
				bmp.GetSize(size);

				CSize szGripper(size.cx / info.imgStatusInfo.count(), abs(size.cy));
				CRect rcGripper;
				rcGripper.left = rc.left + (rc.Width() - szGripper.cx) / 2;
				rcGripper.top = rc.top + (rc.Height() - szGripper.cy) / 2;
				rcGripper.right = rcGripper.left + szGripper.cx;
				rcGripper.bottom = rcGripper.top + szGripper.cy;

				Universal::UI::Paint::DrawImage(dc, rcGripper, bmp, info.imgStatusInfo.ToFraw9Frame(dwState));
			}
		}

	private:
		CPoint m_ptDown;
		bool m_bLButtonDown;
		bool m_bMoved;
	};
};

using CUIDefaultHScrollbar = CUIDefaultScrollbarT < true >;
using CUIDefaultVScrollbar = CUIDefaultScrollbarT < false >;
