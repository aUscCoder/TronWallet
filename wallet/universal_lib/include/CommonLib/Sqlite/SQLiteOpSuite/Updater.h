// NOLINT (all) :)
// Author: WuJian

#pragma once

#include "Operator.h"

class CUpdater : public COperator
{
public:
	UNIVERSAL_LIB_API CUpdater(CSQLite3DB *pDB, const TableDescriptor &tab);

	template<typename ... CConditionTreeWithAssign>
	CUpdater& Update(CConditionTreeWithAssign &... args)
	{
		BeginOperation();
		int _[] = { (PushExpr(args.t), 0)... };
		_se_unused_(_); // warning C4189: '_' : local variable is initialized but not referenced
		return (*this);
	}

	UNIVERSAL_LIB_API CUpdater& Where(CConditionTree &t);

	void MakeSQLStr() override;

protected:

	UNIVERSAL_LIB_API void BeginOperation();
};

