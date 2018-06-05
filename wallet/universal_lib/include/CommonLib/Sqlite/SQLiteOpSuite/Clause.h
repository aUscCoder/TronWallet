// NOLINT (all) :)
// Author: WuJian

#pragma once
#include "Types.h"

class CConditionTree;

enum
{
	SQLFuncBegin = 0x7ffffff0,
	SQL_UTC_Time,
	SQL_UTC_TimeElapse,
	SQL_SQLite_GUID,
	SQL_DATETIME,
};


#define TO_SQL_FUNC(x) $((SQLite_Func)x)

#define UTC_Time			TO_SQL_FUNC(SQL_UTC_Time)
#define UTC_TimeElapse		TO_SQL_FUNC(SQL_UTC_TimeElapse)
#define SQL_GUID			TO_SQL_FUNC(SQL_SQLite_GUID)
#define SQL_DATETIME		TO_SQL_FUNC(SQL_DATETIME)


struct UserDefinedFunctionData
{
	SQLite_Func pFunc;
	std::vector<CConditionTree*> params;

	~UserDefinedFunctionData();

	std::string GetFuncName() const
	{
		static LPCSTR _[] = { "UTC_Time", "UTC_Time_Elapsed", "GUID", "SQL_DATETIME" };
		if ((DWORD)pFunc > SQLFuncBegin)
		{
			return _[(int)pFunc - SQLFuncBegin - 1];
		}
		else
		{
			se::stringbuf sb;
			sb << "F_" << (int)pFunc;
			return sb.str();
		}
	}
};


//
// Where(xxx) Update(xxx) ..
// 
class COperator;
class CConditionTree
{
public:
	CConditionTree(bool b) : m_eOp(OP_LeafData), m_data(b ? 1 : 0), m_pLeft(NULL), m_pRight(NULL), m_iColumnNum(0) {}
	CConditionTree(int i) : m_eOp(OP_LeafData), m_data(i), m_pLeft(NULL), m_pRight(NULL), m_iColumnNum(0) {}
	CConditionTree(DWORD dw) : m_eOp(OP_LeafData), m_data(dw), m_pLeft(NULL), m_pRight(NULL), m_iColumnNum(0) {}
	CConditionTree(unsigned int i) : m_eOp(OP_LeafData), m_data(i), m_pLeft(NULL), m_pRight(NULL), m_iColumnNum(0) {}
	CConditionTree(__int64 i) : m_eOp(OP_LeafData), m_data(i), m_pLeft(NULL), m_pRight(NULL), m_iColumnNum(0) {}
	CConditionTree(unsigned __int64 i) : m_eOp(OP_LeafData), m_data(i), m_pLeft(NULL), m_pRight(NULL), m_iColumnNum(0) {}
	CConditionTree(LPCSTR s) : m_eOp(OP_LeafData), m_data(s), m_pLeft(NULL), m_pRight(NULL), m_iColumnNum(0) {}
	CConditionTree(const std::string &s) : m_eOp(OP_LeafData), m_data(s), m_pLeft(NULL), m_pRight(NULL), m_iColumnNum(0) {}
	CConditionTree(_Blob &b) : m_eOp(OP_LeafData), m_data(b.GetBinaryArg(), b.GetBinaryArgLength()), m_pLeft(NULL),
		m_pRight(NULL), m_iColumnNum(0) {}

	CConditionTree(Column i) : m_eOp(OP_LeafColumn), m_iColumnNum(i.iColumnValue), m_pLeft(NULL), m_pRight(NULL), m_data(0) {
		
	}
	CConditionTree(SQLite_Func p) : m_eOp(OP_LeafFunc), m_pLeft(NULL), m_pRight(NULL), m_data(0)
	{
		m_pUserDefined = new UserDefinedFunctionData;
		m_pUserDefined->pFunc = p;
	}
	CConditionTree() : m_eOp(OP_LeafBegin), m_pLeft(NULL), m_pRight(NULL), m_data(0), m_iColumnNum(0) {}
	UNIVERSAL_LIB_API ~CConditionTree();


	enum OPType { EQ, NE, GT, LT, GTE, LTE, ASSIGN, PLUS, MINUS, MULIT, DIVIDE, BITWISE_AND, BITWISE_OR, AND, OR, LIKE,
		OP_LeafBegin, OP_LeafFunc, OP_LeafData, OP_LeafColumn };

	void Free();
	void FreeAndDeleteMyself();

	UNIVERSAL_LIB_API void GetSQLStr(COperator *op, const TableDescriptor &td, std::set<int> *pAffactedColumns);

	void SetLeftChild(CConditionTree *p) { m_pLeft = p; }
	void SetRightChild(CConditionTree *p) { m_pRight = p; }
	void SetOP(OPType e) { m_eOp = e; }

	CConditionTree* GetLeftChild() const { return m_pLeft; }
	CConditionTree* GetRightChild() const { return m_pRight; }
	OPType GetOP() const { return m_eOp; }
	LPCSTR GetOPStr() const;
	int GetColumn() const { return (m_eOp == OP_LeafColumn) ? m_iColumnNum : -1; }
	const _D& GetData() const { return m_data; }
	const UserDefinedFunctionData* GetFunc() const { return m_pUserDefined; }

private:
	
	OPType m_eOp;
	CConditionTree *m_pLeft;
	CConditionTree *m_pRight;

	union
	{
		int m_iColumnNum;
		UserDefinedFunctionData *m_pUserDefined;
	};
	_D m_data;


	friend class CConditionTreeForUserDefinedFunc;
};


#define _PD(x)		m_pUserDefined->params.push_back(new CConditionTree(x))

class CConditionTreeForUserDefinedFunc : public CConditionTree
{
public:
	CConditionTreeForUserDefinedFunc(SQLite_Func p) : CConditionTree(p) {}
	~CConditionTreeForUserDefinedFunc() {}

	CConditionTree& operator() () { return *this; }
	CConditionTree& operator() (const CConditionTree &a0) { _PD(a0); return *this; }
	CConditionTree& operator() (const CConditionTree &a0, const CConditionTree &a1) { _PD(a0);_PD(a1); return *this; }
	CConditionTree& operator() (const CConditionTree &a0, const CConditionTree &a1, const CConditionTree &a2) { _PD(a0);_PD(a1);_PD(a2); return *this; }
	CConditionTree& operator() (const CConditionTree &a0, const CConditionTree &a1, const CConditionTree &a2, const CConditionTree &a3)
	{ _PD(a0);_PD(a1);_PD(a2);_PD(a3); return *this; }

	CConditionTree& operator() (const CConditionTree &a0, const CConditionTree &a1, const CConditionTree &a2, const CConditionTree &a3,
		const CConditionTree &a4)
	{ _PD(a0);_PD(a1);_PD(a2);_PD(a3);_PD(a4); return *this; }
	// TODO ...
};


UNIVERSAL_LIB_API CConditionTree operator == (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator != (const CConditionTree &l, const CConditionTree &r);

UNIVERSAL_LIB_API CConditionTree operator > (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator < (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator >= (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator <= (const CConditionTree &l, const CConditionTree &r);

// CWhereCondition operator = (const CWhereCondition &l, const CWhereCondition &r);
UNIVERSAL_LIB_API CConditionTree operator + (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator - (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator * (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator / (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator & (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator | (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator && (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator || (const CConditionTree &l, const CConditionTree &r);
UNIVERSAL_LIB_API CConditionTree operator % (const CConditionTree &l, const CConditionTree &r);

CConditionTree operator | (const CConditionTree &l, const CConditionTree &r);

inline CConditionTreeForUserDefinedFunc $(SQLite_Func p) { return CConditionTreeForUserDefinedFunc(p); }

class CClause
{
public:

	explicit CClause(const TableDescriptor& td) : m_td(td) {}
	~CClause() {}
	const se::stringbuf& GetClauseSQLStr() const { return m_sb; }
	void Clear() { m_sb.clear(); }

protected:

	TableDescriptor m_td;
	se::stringbuf m_sb;
};

se::stringbuf& operator<< (se::stringbuf &sb, const CClause &w);


class CConditionTreeWithAssign
{
public:
	CConditionTree t;
};

//
// Group by
//
class CGroupbyClause : public CClause
{
public:
	CGroupbyClause(const TableDescriptor& td) : CClause(td) { }
	~CGroupbyClause() {}
	UNIVERSAL_LIB_API void GroupBy(int iColumnValue);
};

//
// Order by
//
class COrderbyClause : public CClause
{
public:
	COrderbyClause(const TableDescriptor& td) : CClause(td) { }
	~COrderbyClause() {}
	UNIVERSAL_LIB_API void OrderBy(std::vector<int>& vColumnValue, int iOrder);

	enum { asc, desc };
};

//
// Limit
//

class CLimitClause : public CClause
{
public:
	CLimitClause(const TableDescriptor& td) : CClause(td) { }
	~CLimitClause() {}
	UNIVERSAL_LIB_API void Limit(int i);
	UNIVERSAL_LIB_API void Limit(int iStart, int iCount);
};


// Column的实现，放到这是因为CConditionTreeWithAssign的定义在Clause.h里
inline CConditionTreeWithAssign Column::operator = (const CConditionTree &r)
{
	CConditionTreeWithAssign c;
	c.t.SetOP(CConditionTree::ASSIGN);
	c.t.SetLeftChild(new CConditionTree(*this));
	c.t.SetRightChild(new CConditionTree(r));
	return c;
}