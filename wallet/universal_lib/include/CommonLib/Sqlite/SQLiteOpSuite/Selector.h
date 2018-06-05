// Author: WuJian

#pragma once
#include "Operator.h"
#include "SelectResult.h"

// BEGIN_NOLINT(4.5.66 为了美观)
// BEGIN_NOLINT(4.5.67 为了美观)

class CSelector : public COperator
{
public:
	UNIVERSAL_LIB_API CSelector(CSQLite3DB *pDB, const TableDescriptor &tab);
	UNIVERSAL_LIB_API ~CSelector();

	template<typename ... Args>
	CSelector& Select(const Args ... args)
	{
		BeginOperation();
		int _[] = { (AppendSelectKey(args), 0)... };
		_se_unused_(_); // warning C4189: '_' : local variable is initialized but not referenced
		return (*this);
	}

	UNIVERSAL_LIB_API CSelector& SelectCount();		// 默认count(*)，如果要count啥就再加接口
	UNIVERSAL_LIB_API CSelector& SelectMin(Column i);
	UNIVERSAL_LIB_API CSelector& SelectMax(Column i);
	UNIVERSAL_LIB_API CSelector& SelectDistinct(Column i);

	UNIVERSAL_LIB_API CSelector& Where(CConditionTree &w);

	UNIVERSAL_LIB_API CSelector& OrderBy(int iOrder, Column i1);
	UNIVERSAL_LIB_API CSelector& OrderBy(int iOrder, Column i1, Column i2);
	UNIVERSAL_LIB_API CSelector& OrderBy(int iOrder, Column i1, Column i2, Column i3);

	UNIVERSAL_LIB_API CSelector& GroupBy(Column i);

	UNIVERSAL_LIB_API CSelector& Limit(int iLimit);
	UNIVERSAL_LIB_API CSelector& Limit(int iStart, int iCount);

	UNIVERSAL_LIB_API CSelectResult* ExecSelect();
	UNIVERSAL_LIB_API bool ExecQuickSelectInt64(__int64 *pOut);		// 返回true是成功，false是EOF
	UNIVERSAL_LIB_API __int64 ExecSelectCount();
	void MakeSQLStr() override;

private:

	UNIVERSAL_LIB_API void BeginOperation();
	UNIVERSAL_LIB_API void AppendSelectKey(Column i);

private:

	enum class SelectWhat
	{
		normal,
		count,
		min,
		max,
		distinct
	};
	
	SelectWhat m_eSelectWhat;
	COrderbyClause m_orderby;
	CGroupbyClause m_groupby;
	CLimitClause m_limit;
};

// END_NOLINT
// END_NOLINT
