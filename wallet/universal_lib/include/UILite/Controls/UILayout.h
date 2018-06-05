// Author: miaojinquan
#pragma once
#include "../../Universal.h"
#include "../Core/UIObject.h"
#include "../Util/ManagedPtr.h"
#include "../UICrack.h"
#include "UIControlEvent.h"
#include "UIControls.h"


struct UILayoutObject
{
	CUIObject*	pObj;
	CSize		szSize;
};

// 布局基类

class CUILayout : public CUIObject
{
public:

	explicit CUILayout(bool bAutoDelete = false);

	~CUILayout() { }

	UNIVERSAL_LIB_API void AddObject(CUIObject* pObj, const CSize &szSize = CSize(-1, -1));
	UNIVERSAL_LIB_API bool IsObjectAdded(CUIObject* pObj);
	UNIVERSAL_LIB_API void RemoveObject(CUIObject* pObj);
	UNIVERSAL_LIB_API void RemoveAllObject();
	void SetMargin(int iLeft, int iTop = 0, int iRight = 0, int iBottom = 0)
	{
		m_iContentsMargins[0] = iLeft;
		m_iContentsMargins[1] = iTop;
		m_iContentsMargins[2] = iRight;
		m_iContentsMargins[3] = iBottom;
		if (UIIsCreated())
			Relayout();
	}


	void OnUICreate();
	void OnUISize();
	bool OnUIMeasureChild(CUIObject* pChild, SIZE& rc);

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

protected:

	virtual void Relayout() = 0;
	
	std::vector<UILayoutObject> m_Objs;
	int			m_iContentsMargins[4];  // { left, top, right, bottom }
};

//////////////////////////////////////////////////////////////////////////
// t_HDirection == true ? 水平布局:垂直布局
template <bool t_HDirection>
class CUIBoxLayout : public CUILayout
{

public:

	explicit CUIBoxLayout(bool bCenter = false, bool bAutoDelete = false)
		: CUILayout(bAutoDelete), m_iSpace(2), m_bCenter(bCenter) { }

	~CUIBoxLayout() {}
	
	void SetSpace(const int iSpace)
	{
		m_iSpace = iSpace;
		if (UIIsCreated())
			Relayout();
	}

	// 水平布局垂直居中；垂直布局水平居中
	void SetAlignCenter(bool bCenter = true) { m_bCenter = bCenter; }

	void Relayout()
	{
		const CRect rcClient = UIGetClientRect();
		int iPos = 0;
		int iDelta = 0;
		SIZE szTemp;
		for (size_t i = 0; i < m_Objs.size(); i++)
		{
			UILayoutObject &obj = m_Objs.at(i);
			if (obj.szSize == CSize(-1, -1))
				szTemp = obj.pObj->UIMeasure();
			else
				szTemp = obj.szSize;
			CRect rc(0, 0, 0, 0);
			if (t_HDirection)
			{
				rc.left += iPos;
				rc.top = m_bCenter ? ((rcClient.Height() - szTemp.cy) / 2) : 0;
				rc.right = rc.left + szTemp.cx;
				rc.bottom = rc.top + szTemp.cy;
				iDelta = szTemp.cx;
			}
			else
			{
				rc.top += iPos;
				rc.left = m_bCenter ? ((rcClient.Width() - szTemp.cx) / 2) : 0;
				rc.right = rc.left + szTemp.cx;
				rc.bottom = rc.top + szTemp.cy;
				iDelta = szTemp.cy;
			}
			rc.OffsetRect(m_iContentsMargins[0], m_iContentsMargins[1]);
			obj.pObj->UISetPos(rc);
			iPos += iDelta + m_iSpace;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////

	BEGIN_UI_EVENT_MAP(CUIBoxLayout)
		UI_EVENT_MEASURECLIENT(OnUIMeasureClient)
	END_UI_EVENT_MAP()

	bool OnUIMeasureClient(SIZE& sz)
	{
		sz = CSize();
		if (m_Objs.size() > 0)
		{
			SIZE szTemp;
			for (size_t i = 0; i < m_Objs.size(); i++)
			{
				UILayoutObject &obj = m_Objs.at(i);
				if (obj.szSize == CSize(-1, -1))
					szTemp = obj.pObj->UIMeasure();
				else
					szTemp = obj.szSize;

				if (t_HDirection)	// 水平布局
				{
					sz.cy = std::max(sz.cy, szTemp.cy);
					sz.cx	+= szTemp.cx + m_iSpace;
				}
				else
				{
					sz.cx = std::max(sz.cx, szTemp.cx);
					sz.cy	+= szTemp.cy + m_iSpace;
				}
			}

			if (t_HDirection)	// 水平布局
				sz.cx -= m_iSpace;
			else
				sz.cy -= m_iSpace;
		}
		sz.cx  += m_iContentsMargins[0] + m_iContentsMargins[2];
		sz.cx += m_iContentsMargins[1] + m_iContentsMargins[3];
		return true;
	}

protected:

	int		m_iSpace;
	bool	m_bCenter;			// 是否居中对齐
};


typedef CUIBoxLayout<true> CUIHorizontalLayout;
typedef CUIBoxLayout<false> CUIVerticalLayout;

//////////////////////////////////////////////////////////////////////////
//
// class CUIFlowLayout : public CUILayout
// {
//
// public:
//
// 	explicit CUIFlowLayout(const int iHSpace = 2, const int iVSpace = 2, bool bAutoDelete = false)
// 		: CUILayout(bAutoDelete), m_iHSpace(iHSpace), m_iVSpace(iVSpace) { }
// 	~CUIFlowLayout();
//
// 	void SetSpace(const int iHSpace, const int iVSpace)
// 	{
// 		m_iHSpace = iVSpace;
// 		if (UIIsCreated())
// 			Relayout();
// 	}
//
// 	void Relayout();
//
// protected:
//
// 	int m_iHSpace;
// 	int m_iVSpace;
// };

//////////////////////////////////////////////////////////////////////////

class CUILayoutBase: public CUIControl
{
public:

	explicit CUILayoutBase(DWORD dwID) : CUIControl(dwID), m_bLockElementCount(false), m_bNeedLayout(false), m_bNeedMeasure(true)
	{ UIAddOrRemoveStyle(true, UIStyle_Transparent); }
	~CUILayoutBase() {}

	UNIVERSAL_LIB_API int AddElement(CUIObject* pObj, bool bManaged = false);
	UNIVERSAL_LIB_API bool RemoveElement(int iIndex);
	UNIVERSAL_LIB_API bool RemoveAllElements();
	void SetElementVisible(int iIndex, bool bVisible = true);

	CUIObject* GetElement(int iIndex) const { return iIndex >= 0 && iIndex < (int)m_vecElement.size() ? m_vecElement[iIndex].get() : NULL; }
	int GetElementCount() const { return (int)m_vecElement.size(); }
	int GetElementIndex(CUIObject* pObj) const;
	bool GetElementVisible(int iIndex) const;
	RECT GetElementRect(int iIndex) const;

	virtual void UpdateLayout() = 0;

	//////////////////////////////////////////////////////////////////////////
	// 因为要导出, 所以...
	UNIVERSAL_LIB_API bool OnUIEvent(DEvent* pEvent, LRESULT& lResult) override;
	DECLARE_UI_EVENT_MAP2();

	void OnUICreate();
	void OnUIPrePaint();
	bool OnUIGetKidsZOrder(std::vector<CUIObject*>* pResultVec);

	bool OnUIMeasureClient(SIZE& sz)
	{
		if (m_bNeedMeasure)
			UpdateLayout();

		sz = m_szLastMeasured;
		return true;
	}

protected:

	void LockElementCount(bool bLock) { m_bLockElementCount = bLock; }
	void SetLayout() { m_bNeedLayout = true; m_bNeedMeasure = true; UIRedraw(); }
	void LayoutUpdated() { m_bNeedLayout = false; }

protected:
	CSize m_szLastMeasured;
	bool m_bNeedMeasure;

private:

	void CreateElement(CUIObject* pObj);

	typedef se::managed_ptr<CUIObject> ManagedObject;
	typedef std::vector<ManagedObject> ManagedObjectVec;

	ManagedObjectVec m_vecElement;
	bool m_bLockElementCount;
	bool m_bNeedLayout;
};

inline void CUILayoutBase::SetElementVisible(int iIndex, bool bVisible /* = true */)
{
	UICHECK_OBJECT_VALID();
	CUIObject* pElement = GetElement(iIndex);
	if (pElement && pElement->UIIsVisible() != bVisible)
	{
		SetLayout();
		pElement->UIShow(bVisible);
	}
}

inline bool CUILayoutBase::GetElementVisible(int iIndex) const
{
	UICHECK_OBJECT_VALID();
	CUIObject* pElement = GetElement(iIndex);
	return pElement && pElement->UIIsVisible();
}

inline int CUILayoutBase::GetElementIndex(CUIObject* pObj) const
{
	UICHECK_OBJECT_VALID();
	ManagedObjectVec::const_iterator it = std::find(m_vecElement.begin(), m_vecElement.end(), pObj);
	if (it == m_vecElement.cend())
		return INVALID_ITEM;

	return it - m_vecElement.cbegin();
}

inline RECT CUILayoutBase::GetElementRect(int iIndex) const
{
	UICHECK_OBJECT_VALID();
	CUIObject* pElement = GetElement(iIndex);
	return pElement && pElement->UIIsVisible() ? pElement->UIGetRect() : CRect();
}

inline void CUILayoutBase::OnUIPrePaint()
{
	if (m_bNeedLayout)
		UpdateLayout();
}

//////////////////////////////////////////////////////////////////////////

enum HorzLayoutAlign
{
	HorzLayoutAlign_Top,
	HorzLayoutAlign_Center,
	HorzLayoutAlign_Bottom,
};

enum VertLayoutAlign
{
	VertLayoutAlign_Left,
	VertLayoutAlign_Center,
	VertLayoutAlign_Right,
};

template<bool t_bHorz>
class CUILayoutT : public CUILayoutBase
{
public:

	explicit CUILayoutT(DWORD_PTR dwID = 0) : CUILayoutBase(dwID) {}
	~CUILayoutT() {}

	typedef CUILayoutT<t_bHorz> TClass;
	BEGIN_UI_EVENT_MAP(TClass)
		if (t_bHorz)
			UI_DECLARE_ORIGINAL_OBJECT_TYPE(UIObject_HorzLayout)
		else
			UI_DECLARE_ORIGINAL_OBJECT_TYPE(UIObject_VertLayout)

		UI_EVENT_SIZE(OnUISize)
	END_UI_EVENT_MAP()

	void OnUISize() { UpdateLayout(); }

	void UpdateLayout() override
	{
		// 水平和垂直排盘的算法一样，只需要把左右和上下掉一下就行了，这个函数的作用是
		// 一水平排版为原型写成算法，然后利用模板参数把左右和上下换一下就成了垂直排版
		// 算法了。

		LayoutUpdated();
		m_bNeedMeasure = false;
		m_szLastMeasured.SetSize(0, 0);

		if (GetElementCount() == 0 || !UIIsCreated())
			return;

		LockElementCount(true);

		std::vector<int> vecWidth;
		std::vector<SIZE> vecMeasure;
		std::vector<int> vecAutoSizeIndex;
		CRect rc = UIGetClientRect();

		// 第一遍，先把能定大小的都定下来，不能定大小的记下来
		int iLastRightMargin = 0;
		int iXOffset = 0;
		vecWidth.reserve(GetElementCount());
		for (int i = 0; i < GetElementCount(); i++)
		{
			CUIObject* pElement = GetElement(i);
			const UIProps* pProps = pElement->UIGetProps();

			if (pElement->UIIsVisible())
			{
				if (!pProps || !pProps->HasSize() || IS_AUTO_SIZE(pProps->size.cx) || IS_AUTO_SIZE(pProps->size.cy))
					vecMeasure.push_back(pElement->UIMeasure());
				else
					vecMeasure.push_back(CSize());

				int iWidth = SizeCx(*vecMeasure.rbegin());
				if (pProps && pProps->HasSize() && (SizeCx(pProps->size) >= 0 || IS_AUTO_FILL(SizeCx(pProps->size))))
				{
					iWidth = SizeCx(pProps->size);
					if (IS_AUTO_FILL(SizeCx(pProps->size)))
						vecAutoSizeIndex.push_back(vecWidth.size());
				}

				UIPropMargin margin;
				if (pProps)
					margin = pProps->margin;

				vecWidth.push_back(iWidth);

				iXOffset += (i ? std::max<int>(iLastRightMargin, MarginLeft(margin)) : MarginLeft(margin)) + std::max(iWidth, 0);
				iLastRightMargin = MarginRight(margin);
			}
			else
			{
				vecWidth.push_back(0);
				vecMeasure.push_back(CSize());
			}
		}

		// 第二遍，如果有自适应大小的话啊，计算自适应宽度
		if (vecAutoSizeIndex.size() > 0)
		{
			int iAutoSizeWidth = std::max(0, (RectWidth(rc) - iXOffset - iLastRightMargin) / (int)vecAutoSizeIndex.size());
			int iAutoSizeAdjust = std::max(RectWidth(rc) - iXOffset - iLastRightMargin - iAutoSizeWidth * (int)vecAutoSizeIndex.size(), 0);
			for (size_t n = 0; n < vecAutoSizeIndex.size(); ++n)
			{
				int iWidth = iAutoSizeWidth;
				if (iAutoSizeAdjust)
				{
					iWidth++;
					iAutoSizeAdjust--;
				}

				UIASSERT(vecAutoSizeIndex[n] >= 0 && vecAutoSizeIndex[n] < (int)vecWidth.size());
				vecWidth[vecAutoSizeIndex[n]] = iWidth > 0 ? iWidth : 0;
			}
		}
		vecAutoSizeIndex.clear();

		// 第三遍，设置每个元素的位置和大小
		iXOffset = 0;
		iLastRightMargin = 0;
		for (int i = 0; i < GetElementCount(); ++i)
		{
			CUIObject* pElement = GetElement(i);
			const UIProps* pProps = pElement->UIGetProps();

			if (pElement->UIIsVisible())
			{
				UIPropMargin margin;
				if (pProps)
					margin = pProps->margin;

				int iHeight = SizeCy(vecMeasure[i]);
				if (pProps && pProps->HasSize())
				{
					if (IS_AUTO_FILL(SizeCy(pProps->size)))
						iHeight = RectHeight(rc) - MarginTop(margin) - MarginBottom(margin);
					else if (!IS_AUTO_SIZE(SizeCy(pProps->size)))
						iHeight = SizeCy(pProps->size);
				}

				RECT rcElement = { 0 };
				RectLeft(rcElement) = iXOffset + (i ? std::max<int>(iLastRightMargin, MarginLeft(margin)) : MarginLeft(margin));
				RectRight(rcElement) = RectLeft(rcElement) + vecWidth[i];

				RectTop(rcElement) = MarginTop(margin);
				if (AlignCenter())
					RectTop(rcElement) = MarginTop(margin) + (RectHeight(rc) - iHeight - MarginTop(margin) - MarginBottom(margin)) / 2;
				else if (AlignBottom())
					RectTop(rcElement) = RectHeight(rc) - MarginBottom(margin) - iHeight;
				RectBottom(rcElement) = RectTop(rcElement) + iHeight;

				iLastRightMargin = MarginRight(margin);
				iXOffset = RectRight(rcElement);

				SizeCy(m_szLastMeasured) = std::max<LONG>(SizeCy(m_szLastMeasured), RectHeight(rcElement) + MarginTop(margin) + MarginBottom(margin));
				SizeCx(m_szLastMeasured) = iXOffset + iLastRightMargin;

				pElement->UISetPos(rcElement);
			}
		}

		LockElementCount(false);
	}

private:

	LONG SizeCx(const SIZE& sz) { return t_bHorz ? sz.cx : sz.cy; }
	LONG SizeCy(const SIZE& sz) { return t_bHorz ? sz.cy : sz.cx; }
	LONG& SizeCx(SIZE& sz) { return t_bHorz ? sz.cx : sz.cy; }
	LONG& SizeCy(SIZE& sz) { return t_bHorz ? sz.cy : sz.cx; }
	int ElementWidth(CUIObject* p) { return t_bHorz ? p->UIGetWidth() : p->UIGetHeight(); }
	int ElementHeight(CUIObject* p) { return t_bHorz ? p->UIGetHeight() : p->UIGetWidth(); }
	int RectWidth(const CRect& rc) { return t_bHorz ? rc.Width() : rc.Height(); }
	int RectHeight(const CRect& rc) { return t_bHorz ? rc.Height() : rc.Width(); }
	UIPropInt& MarginLeft(UIPropMargin& margin) { return t_bHorz ? margin.left : margin.top; }
	UIPropInt& MarginRight(UIPropMargin& margin) { return t_bHorz ? margin.right : margin.bottom; }
	UIPropInt& MarginTop(UIPropMargin& margin) { return t_bHorz ? margin.top : margin.left; }
	UIPropInt& MarginBottom(UIPropMargin& margin) { return t_bHorz ? margin.bottom : margin.right; }
	LONG& RectLeft(RECT& rc) { return t_bHorz ? rc.left : rc.top; }
	LONG& RectTop(RECT& rc) { return t_bHorz ? rc.top : rc.left; }
	LONG& RectRight(RECT& rc) { return t_bHorz ? rc.right : rc.bottom; }
	LONG& RectBottom(RECT& rc) { return t_bHorz ? rc.bottom : rc.right; }
	bool AlignCenter() { return UITestStyle(t_bHorz ? UIHLStyle_Center : UIVLStyle_Center); }
	bool AlignBottom() { return UITestStyle(t_bHorz ? UIHLStyle_Bottom : UIVLStyle_Right); }
};

typedef CUILayoutT<true>	CUIHorzLayout;
typedef CUILayoutT<false>	CUIVertLayout;

#undef IS_AUTO_SIZE
#undef IS_AUTO_FILL

//////////////////////////////////////////////////////////////////////////

class CUIFlowLayout : public CUILayoutBase
{
public:

	explicit CUIFlowLayout(DWORD dwID) : CUILayoutBase(dwID) {}
	~CUIFlowLayout() {}
};

//////////////////////////////////////////////////////////////////////////

class CUIGridLayout : public CUILayoutBase
{
public:
	explicit CUIGridLayout(DWORD dwID) : CUILayoutBase(dwID) {}
	~CUIGridLayout() {}
};
