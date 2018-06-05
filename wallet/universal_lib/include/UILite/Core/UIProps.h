// Author: KeJinjin
#pragma once


//////////////////////////////////////////////////////////////////////////

enum UIPropAlignType
{
	UIPropAlign_None,
	UIPropAlign_Head,	// "left"		"Top"
	UIPropAlign_Center,	// "Center"		"Middle"
	UIPropAlign_Tail,	// "Right"		"Bottom"
};

enum UIPropIntType
{
	UIPropInt_Normal,
	UIPropInt_Percent,
	UIPropInt_Auto,
};

struct UIPropInt
{
	UIPropIntType eType;
	int iValue;

	UIPropInt() : eType(UIPropInt_Normal), iValue(0)
	{

	}

	explicit UIPropInt(int iValue, UIPropIntType type = UIPropInt_Normal) : eType(type), iValue(iValue)
	{

	}

	UIPropInt(const UIPropInt &that) : eType(that.eType), iValue(that.iValue)
	{

	}

	UIPropInt &operator = (int i)
	{
		eType = UIPropInt_Normal;
		iValue = i;
		return *this;
	}

	UIPropInt &operator = (const UIPropInt &that)
	{
		this->eType = that.eType;
		this->iValue = that.iValue;
		return *this;
	}

	operator int() const
	{
		switch (eType)
		{
		case UIPropInt_Normal:
			return iValue;
		default:
			break;
		}

		return 0;
	}

	bool IsValue() const
	{
		switch (eType)
		{
		case UIPropInt_Normal:
		case UIPropInt_Percent:
			return true;
		default:
			break;
		}

		return false;
	}

	bool IsAuto() const
	{
		return eType == UIPropInt_Auto;
	}

	int GetInt(int iParent = 0) const
	{
		switch (eType)
		{
		case UIPropInt_Normal:
			return iValue;
		case UIPropInt_Percent:
			return iValue * iParent / 100;
		default:
			break;
		}
		return 0;
	}
};

struct UIPropPoint
{
	UIPropInt x;
	UIPropInt y;

	UIPropPoint() : x(0), y(0)
	{

	}

	UIPropPoint(int x, int y) : x(x), y(y)
	{

	}

	UIPropPoint(const UIPropInt & x, const UIPropInt & y) : x(x), y(y)
	{

	}

	explicit UIPropPoint(const POINT &pt) : x(pt.x), y(pt.y)
	{

	}

	UIPropPoint &operator = (const POINT &pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}

	UIPropPoint &operator = (const UIPropPoint &that)
	{
		this->x = that.x;
		this->y = that.y;
		return *this;
	}

	operator POINT() const
	{
		POINT pt = { (int)x, (int)y };
		return pt;
	}

	POINT GetPoint(const SIZE &szParent) const
	{
		POINT pt = { x.GetInt(szParent.cx), y.GetInt(szParent.cy) };
		return pt;
	}
};

struct UIPropSize
{
	UIPropInt cx;
	UIPropInt cy;

	UIPropSize() : cx(0), cy(0)
	{

	}

	UIPropSize(int cx, int cy) : cx(cx), cy(cy)
	{

	}

	UIPropSize(const UIPropInt &cx, const UIPropInt &cy) : cx(cx), cy(cy)
	{

	}

	explicit UIPropSize(const SIZE &sz) : cx(sz.cx), cy(sz.cy)
	{

	}

	UIPropSize &operator = (const SIZE &sz)
	{
		cx = sz.cx;
		cy = sz.cy;
		return *this;
	}

	UIPropSize &operator = (const UIPropSize &that)
	{
		this->cx = that.cx;
		this->cy = that.cy;
		return *this;
	}

	operator SIZE() const
	{
		SIZE sz = { (int)cx, (int)cy };
		return sz;
	}

	SIZE GetSize(const SIZE &szParent) const
	{
		SIZE sz = { cx.GetInt(szParent.cx), cy.GetInt(szParent.cy) };
		return sz;
	}
};

struct UIPropRect
{
	UIPropInt x;
	UIPropInt y;
	UIPropInt width;
	UIPropInt height;

	UIPropRect() : x(0), y(0), width(0), height(0)
	{

	}

	UIPropRect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height)
	{

	}

	UIPropRect(const UIPropInt &x, const UIPropInt &y, const UIPropInt &width, const UIPropInt &height) :
		x(x), y(y), width(width), height(height)
	{

	}

	explicit UIPropRect(const RECT &rc) : x(rc.left), y(rc.top), width(rc.right - rc.left), height(rc.bottom - rc.top)
	{

	}

	UIPropRect &operator = (const RECT &rc)
	{
		x = rc.left;
		y = rc.right;
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;
		return *this;
	}

	UIPropRect &operator = (const UIPropRect &that)
	{
		this->x = that.x;
		this->y = that.y;
		this->width = that.width;
		this->height = that.height;
		return *this;
	}

	operator RECT() const
	{
		RECT rc = { (int)x, (int)y, (int)width, (int)height };
		rc.right += rc.left;
		rc.bottom += rc.top;
		return rc;
	}

	RECT GetRect(const SIZE &szParent) const
	{
		RECT rc = { x.GetInt(szParent.cx), y.GetInt(szParent.cy), width.GetInt(szParent.cx), height.GetInt(szParent.cy) };
		rc.right += rc.left;
		rc.bottom += rc.top;
		return rc;
	}
};

struct UIPropMargin
{
	UIPropInt left;
	UIPropInt top;
	UIPropInt right;
	UIPropInt bottom;

	UIPropMargin() : left(0), top(0), right(0), bottom(0)
	{

	}

	UIPropMargin(int left, int top, int right, int bottom) : left(left), top(top), right(right), bottom(bottom)
	{

	}

	UIPropMargin(const UIPropInt &left, const UIPropInt &top, const UIPropInt &right, const UIPropInt &bottom) :
		left(left), top(top), right(right), bottom(bottom)
	{

	}

	explicit UIPropMargin(const RECT &rc) : left(rc.left), top(rc.top), right(rc.right), bottom(rc.bottom)
	{

	}

	UIPropMargin &operator = (const RECT &rc)
	{
		left = rc.left;
		top = rc.top;
		right = rc.right;
		bottom = rc.bottom;
		return *this;
	}

	UIPropMargin &operator = (const UIPropMargin &that)
	{
		this->left = that.left;
		this->top = that.top;
		this->right = that.right;
		this->bottom = that.bottom;
		return *this;
	}

	operator RECT() const
	{
		RECT rc = { (int)left, (int)top, (int)right, (int)bottom };
		return rc;
	}

	RECT GetRect(const SIZE &szParent) const
	{
		RECT rc = { left.GetInt(szParent.cx), top.GetInt(szParent.cy), right.GetInt(szParent.cx), bottom.GetInt(szParent.cy) };
		return rc;
	}
};

const UIPropInt AUTO_SIZE(-2);
const UIPropInt AUTO_FILL(-1);
const UIPropSize AutoSize(AUTO_SIZE, AUTO_SIZE);
const UIPropSize AutoFill(AUTO_FILL, AUTO_FILL);

#define IS_AUTO_SIZE(x) ((x) == -2)
#define IS_AUTO_FILL(x) ((x) == -1)

//////////////////////////////////////////////////////////////////////////
//
// 推荐使用NewUIProps / MakeUIProps 函数来初始化UIProps结构
//

#define PROP_MASK_HALIGN			0x00000001
#define PROP_MASK_VALIGN			0x00000002
#define PROP_MASK_OVERFLOW_HALIGN	0x00000004
#define PROP_MASK_OVERFLOW_VALIGN	0x00000008
#define PROP_MASK_SIZE				0x00000010
#define PROP_MASK_POS				0x00000020
#define PROP_MASK_MARGIN			0x00000040

struct UIProps
{
	UINT uMask;
	UIPropAlignType eHAlign;
	UIPropAlignType eVAlign;
	UIPropAlignType eOverflowHAlign;
	UIPropAlignType eOverflowVAlign;
	UIPropSize size;
	UIPropPoint pos;
	UIPropMargin margin;

	// Helper functions

	UIProps() : uMask(0), eHAlign(UIPropAlign_Head), eVAlign(UIPropAlign_Head), eOverflowHAlign(UIPropAlign_Head), eOverflowVAlign(UIPropAlign_Head)
	{

	}
	~UIProps()
	{
	
	}

	UIProps *SetHAlign(UIPropAlignType e) { eHAlign = e; uMask |= PROP_MASK_HALIGN; return this; }
	void ClearHAlign() { uMask &= (~PROP_MASK_HALIGN); }
	bool HasHAlign() const { return (uMask & PROP_MASK_HALIGN) != 0; }

	UIProps *SetVAlign(UIPropAlignType e) { eVAlign = e; uMask |= PROP_MASK_VALIGN; return this; }
	void ClearVAlign() { uMask &= (~PROP_MASK_VALIGN); }
	bool HasVAlign() const { return (uMask & PROP_MASK_VALIGN) != 0; }

	UIProps *SetOverflowHAlign(UIPropAlignType e) { eOverflowHAlign = e; uMask |= PROP_MASK_OVERFLOW_HALIGN; return this; }
	void ClearOverflowHAlign() { uMask &= (~PROP_MASK_OVERFLOW_HALIGN); }
	bool HasOverflowHAlign() const { return (uMask & PROP_MASK_OVERFLOW_HALIGN) != 0; }

	UIProps *SetOverflowVAlign(UIPropAlignType e) { eOverflowVAlign = e; uMask |= PROP_MASK_OVERFLOW_VALIGN; return this; }
	void ClearOverflowVAlign() { uMask &= (~PROP_MASK_OVERFLOW_VALIGN); }
	bool HasOverflowVAlign() const { return (uMask & PROP_MASK_OVERFLOW_VALIGN) != 0; }

	UIProps *SetSize(const UIPropSize &sz) { size = sz; uMask |= PROP_MASK_SIZE; return this; }
	UIProps *SetSize(const SIZE &sz) { size = sz; uMask |= PROP_MASK_SIZE; return this; }
	UIProps *SetSize(int cx, int cy) { size.cx = cx; size.cy = cy; uMask |= PROP_MASK_SIZE; return this; }
	UIProps *SetAutoSize() { size = AutoSize; return this; }
	void ClearSize() { uMask &= (~PROP_MASK_SIZE);  }
	bool HasSize() const { return (uMask & PROP_MASK_SIZE) != 0; }

	UIProps *SetPos(const UIPropPoint &p) { pos = p; uMask |= PROP_MASK_POS; return this; }
	UIProps *SetPos(const POINT &p) { pos = p; uMask |= PROP_MASK_POS; return this; }
	UIProps *SetPos(int x, int y) { pos.x = x; pos.y = y; uMask |= PROP_MASK_POS; return this; }
	void ClearPos() { uMask &= (~PROP_MASK_POS); }
	bool HasPos() const { return (uMask & PROP_MASK_POS) != 0; }

	UIProps *SetMargin(const UIPropMargin &m) { margin = m; uMask |= PROP_MASK_MARGIN; return this; }
	UIProps *SetMargin(const RECT &m) { margin = m; uMask |= PROP_MASK_MARGIN; return this; }
	UIProps *SetMargin(int left, int top, int right, int bottom)
	{
		margin.left = left;
		margin.top = top;
		margin.right = right;
		margin.bottom = bottom;
		uMask |= PROP_MASK_MARGIN;
		return this;
	}
	void ClearMargin() { uMask &= (~PROP_MASK_MARGIN); }
	bool HasMargin()  const { return (uMask & PROP_MASK_MARGIN) != 0; }
};

//////////////////////////////////////////////////////////////////////////

//
// new一个UIProps并设置初始化值，使用者需要负责delete内存
//
// 接受的参数：
//		SIZE	- size, 大小
//		POINT	- pos, 位置
//		MARGIN	- margin, 边距
//
// 参数只和类型有关，和顺序没有关系，如：
//		UIProps* pProps = NewUIProps(CSize(20, 20), CPoint(10, 10));
//		UIProps* pProps = NewUIProps(CPoint(10, 10), CSize(20, 20));
// 都是new一个UIProps结构并设置大小为20*20，位置在(10, 10)。
//
// 函数参数个数不定，如：
// UIProps* pProps = NewUIProps(CPoint(10, 10));
// UIProps* pProps = NewUIProps(CSize(10, 10));
// UIProps* pProps = NewUIProps(CSize(10, 10), CPoint(10, 10), CMargin(0, 0, 1, 1));
// UIProps* pProps = NewUIProps(CPoint(10, 10), CSize(10, 10), CMargin(0, 0, 1, 1));

template<typename... types>
UIProps* NewUIProps(types... args/* size, pos, margin */)
{
	UIProps* p = new UIProps;
	SetUIProps(p, args...);
	return p;
}

// 用法参见 NewUIProps()
template<typename... types>
UIProps MakeUIProps(types... args/* size, pos, margin */)
{
	UIProps props;
	SetUIProps(&props, args...);
	return props;
}

inline UIProps MakeUIProps()
{
	return UIProps();
}

// 用法参见 NewUIProps()
template<typename TValue, typename... types>
void SetUIProps(UIProps* pProps, TValue v, types... args)
{
	SetUIProp(pProps, v);
	SetUIProps(pProps, args...);
}

// 用法参见 NewUIProps()
template<typename TObject, typename... types>
TObject* BindUIProps(TObject* pObj, types... args)
{
	pObj->UISetProps(NewUIProps(args...), true);
	return pObj;
}

//////////////////////////////////////////////////////////////////////////
inline void SetUIProp(UIProps* pProps, const SIZE& size) { pProps->SetSize(size); }
inline void SetUIProp(UIProps* pProps, const POINT& pos) { pProps->SetPos(pos); }
inline void SetUIProp(UIProps* pProps, const RECT& margin) { pProps->SetMargin(margin); }
inline void SetUIProp(UIProps* pProps, const UIPropSize& size) { pProps->SetSize(size); }
inline void SetUIProp(UIProps* pProps, const UIPropPoint& pos) { pProps->SetPos(pos); }
inline void SetUIProp(UIProps* pProps, const UIPropMargin& margin) { pProps->SetMargin(margin); }
inline void SetUIProps(UIProps* pProps) {}
